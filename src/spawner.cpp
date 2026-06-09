#include "spawner.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/path_follow3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "enemy.h" 
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
//zestaw bibliotek do obslugi klikniec myszka i raycasta
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters3d.hpp>
#include <godot_cpp/variant/dictionary.hpp>


using namespace godot;

int Spawner::zabici_wrogowie = 0;

void Spawner::_bind_methods() {}

Spawner::Spawner() {
    is_building_mode = false;
    mouse_was_clicking = false;
    timer = 0.0;
    spawn_delay = 1.0; // wrogowie wychodza co 1 sekunde w trakcie fali
    
    obecna_fala = 1;
    wrogowie_w_fali = 3; // pierwsza fala ma 3 wrogow
    wyprodukowani_wrogowie = 0;
    czas_do_kolejnej_fali = 5.0; // 5 sekund odpoczynku przed nowa fala
    przerwa_miedzy_falami = true; 

    ResourceLoader *re_loader = ResourceLoader::get_singleton();

    buy_button_was_pressed = false;
    zbudowane_wieze = 0;

    buy_sniper_button_was_pressed = false;
    is_building_sniper_mode = false;

    gra_rozpoczeta = false;
    mnoznik_trudnosci = 1;
    btn_normal_was_pressed = false;
    btn_hard_was_pressed = false;

    sniper_scene = re_loader->load("res://Sniper.tscn");
    tower_scene = re_loader->load("res://Tower.tscn");
    enemy_scene = re_loader->load("res://WagonikZWrogiem.tscn");
}

Spawner::~Spawner() {}

void Spawner::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // --- LOGIKA GAME OVER I STATYSTYKI ---
    Label* hp_label = Object::cast_to<Label>(get_node_or_null("/root/Poziom/CanvasLayer/BaseHPLabel"));
    if (hp_label != nullptr && hp_label->get_text().to_int() <= 0) {
        
        Control* game_over_panel = Object::cast_to<Control>(get_node_or_null("/root/Poziom/CanvasLayer/GameOverPanel"));
        if (game_over_panel != nullptr) {
            
            // 1. Zanim pokazemy panel, chwytamy EndLabel i ladujemy w niego statystyki
            Label* end_label = Object::cast_to<Label>(game_over_panel->get_node_or_null("EndLabel"));
            if (end_label != nullptr) {
                // Skladamy wielolinijkowy tekst uzywajac znaku nowej linii: \n
                String statystyki = "KONIEC GRY\n\n";
                // Odejmujemy 1, bo jesli gracz zginal na fali 5, to znaczy ze przetrwal pelne 4
                statystyki += "Przetrwane fale: " + String::num_int64(obecna_fala - 1) + "\n";
                statystyki += "Zbudowane wieze: " + String::num_int64(zbudowane_wieze) + "\n";
                statystyki += "Zabici wrogowie: " + String::num_int64(zabici_wrogowie);
                
                end_label->set_text(statystyki);
            }

            // 2. Pokazujemy gotowy panel
            game_over_panel->show(); 

            // 3. Sprawdzamy przycisk restartu
            Button* btn_restart = Object::cast_to<Button>(game_over_panel->get_node_or_null("BtnRestart"));
            if (btn_restart != nullptr && btn_restart->is_pressed()) {
                get_tree()->reload_current_scene(); 
            }
        }
        
        return; // Zatrzymujemy dzialanie Spawnera, zeby wiecej wrogow nie wychodzilo
    }

    if (!gra_rozpoczeta) {
        Control* lobby_panel = Object::cast_to<Control>(get_node_or_null("/root/Poziom/CanvasLayer/LobbyPanel"));
        Button* btn_normal = Object::cast_to<Button>(get_node_or_null("/root/Poziom/CanvasLayer/LobbyPanel/BtnNormal"));
        Button* btn_hard = Object::cast_to<Button>(get_node_or_null("/root/Poziom/CanvasLayer/LobbyPanel/BtnHard"));

        if (lobby_panel != nullptr && btn_normal != nullptr && btn_hard != nullptr) {
            bool is_normal_pressed = btn_normal->is_pressed();
            bool is_hard_pressed = btn_hard->is_pressed();

            if (is_normal_pressed && !btn_normal_was_pressed) {
                mnoznik_trudnosci = 1;
                gra_rozpoczeta = true;
                lobby_panel->hide(); 
                UtilityFunctions::print("Start gry: Poziom Normalny");
            }

            if (is_hard_pressed && !btn_hard_was_pressed) {
                mnoznik_trudnosci = 2; 
                gra_rozpoczeta = true;
                lobby_panel->hide(); 
                UtilityFunctions::print("Start gry: Poziom Trudny");
            }

            btn_normal_was_pressed = is_normal_pressed;
            btn_hard_was_pressed = is_hard_pressed;
        }
        
        return; 
    }

    timer += delta;

    if (przerwa_miedzy_falami) {
        if (timer >= czas_do_kolejnej_fali) {
            przerwa_miedzy_falami = false;
            timer = 0.0;
            wyprodukowani_wrogowie = 0;
            UtilityFunctions::print("Rozpoczyna sie fala: ", obecna_fala);
        }
    } 
    else {
        if (timer >= spawn_delay) {
            timer = 0.0;

            if (enemy_scene.is_valid()) {
                Node *nowy_wrog = enemy_scene->instantiate();
                Node *path_node = get_node_or_null("../Path3D"); 
                
                if (path_node != nullptr) {
                    path_node->add_child(nowy_wrog);
                    
                    Enemy* kod_wroga = Object::cast_to<Enemy>(nowy_wrog->get_node_or_null("Enemy"));
                    if (kod_wroga != nullptr) {
                        kod_wroga->set_max_hp((100 + (obecna_fala * 100)) * mnoznik_trudnosci); 
                
                        double poczatkowa_predkosc = 2.0; // Standardowa predkosc
                        
                        if (obecna_fala >= 10) {
                            double dopalacz = (obecna_fala - 9) * 0.2; 
                            poczatkowa_predkosc += dopalacz;
                            UtilityFunctions::print("Uwaga! Wrogowie sa szybsi! Predkosc: ", poczatkowa_predkosc);
                        }
                        
                        kod_wroga->set_speed(poczatkowa_predkosc);
                    }

                    wyprodukowani_wrogowie++;
                }
            }

            if (wyprodukowani_wrogowie >= wrogowie_w_fali) {
                przerwa_miedzy_falami = true;
                obecna_fala++;
                wrogowie_w_fali += 2; 
                timer = 0.0;
                UtilityFunctions::print("Fala pokonana! Nastepna za 5 sekund.");
            }
        }
    }

// --- OBSLUGA PRZYCISKOW ---
    Button* buy_btn = Object::cast_to<Button>(get_node_or_null("/root/Poziom/CanvasLayer/BuyTowerButton"));
    Button* buy_sniper_btn = Object::cast_to<Button>(get_node_or_null("/root/Poziom/CanvasLayer/BuySniperButton"));
    
    // zwykla wieza
    if (buy_btn != nullptr) {
        bool is_pressed = buy_btn->is_pressed();
        if (is_pressed && !buy_button_was_pressed) {
            is_building_mode = !is_building_mode; 
            is_building_sniper_mode = false; // Zabezpieczenie: wylaczamy tryb snajpera
            if (is_building_mode) UtilityFunctions::print("Tryb: Zwykla Wieza");
        }
        buy_button_was_pressed = is_pressed;
    }

    // snajper
    if (buy_sniper_btn != nullptr) {
        bool is_pressed = buy_sniper_btn->is_pressed();
        if (is_pressed && !buy_sniper_button_was_pressed) {
            is_building_sniper_mode = !is_building_sniper_mode;
            is_building_mode = false; // zabezpieczenie: wylaczamy tryb zwyklej wiezy
            if (is_building_sniper_mode) UtilityFunctions::print("Tryb: Snajper");
        }
        buy_sniper_button_was_pressed = is_pressed;
    }

    // --- WSPOLNY RAYCASTING DLA OBU WIEZ ---
    if (is_building_mode || is_building_sniper_mode) {
        Input* input = Input::get_singleton();
        bool is_clicking = input->is_mouse_button_pressed((MouseButton)1);

        if (is_clicking && !mouse_was_clicking) {
            Viewport* vp = get_viewport();
            Camera3D* camera = vp->get_camera_3d();
            
            if (camera != nullptr) {
                Vector2 mouse_pos = vp->get_mouse_position();
                Vector3 from = camera->project_ray_origin(mouse_pos);
                Vector3 to = from + camera->project_ray_normal(mouse_pos) * 1000.0;
                
                PhysicsDirectSpaceState3D* space_state = get_world_3d()->get_direct_space_state();
                Ref<PhysicsRayQueryParameters3D> query = PhysicsRayQueryParameters3D::create(from, to);
                Dictionary result = space_state->intersect_ray(query);
                
                if (!result.is_empty()) {
                    Node* uderzony_obiekt = Object::cast_to<Node>(result["collider"]);
                    
                    if (uderzony_obiekt != nullptr && uderzony_obiekt->is_in_group("ziemia")) {
                        Vector3 pozycja_na_mapie = result["position"];
                        Label* gold_label = Object::cast_to<Label>(get_node_or_null("/root/Poziom/CanvasLayer/GoldLabel"));
                        
                        if (gold_label != nullptr) {
                            int aktualne_zloto = gold_label->get_text().to_int();
                            
                            // wybieramy cene i model na podstawie aktywnego trybu
                            int koszt = is_building_mode ? 200 : 400;
                            Ref<PackedScene> wybrana_scena = is_building_mode ? tower_scene : sniper_scene;
                            
                            if (aktualne_zloto >= koszt && wybrana_scena.is_valid()) {
                                // zabieramy kase
                                gold_label->set_text(String::num_int64(aktualne_zloto - koszt));
                                
                                // stawiamy budynek
                                Node* nowa_wieza = wybrana_scena->instantiate();
                                get_parent()->add_child(nowa_wieza);
                                
                                Node3D* wieza_3d = Object::cast_to<Node3D>(nowa_wieza);
                                if (wieza_3d != nullptr) {
                                    wieza_3d->set_global_position(pozycja_na_mapie);
                                }
                                
                                zbudowane_wieze++;
                                UtilityFunctions::print("Zbudowano! Pozostalo zlota: ", aktualne_zloto - koszt);
                                
                                is_building_mode = false;
                                is_building_sniper_mode = false;
                            } else {
                                UtilityFunctions::print("Za malo zlota!");
                                is_building_mode = false;
                                is_building_sniper_mode = false;
                            }
                        }
                    } else {
                        UtilityFunctions::print("Tu nie mozesz budowac!");
                        is_building_mode = false;
                        is_building_sniper_mode = false;
                    }
                }
            }
        }
        mouse_was_clicking = is_clicking;
    } else {
        mouse_was_clicking = false;
    }

}