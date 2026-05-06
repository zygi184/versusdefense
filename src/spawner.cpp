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

    gra_rozpoczeta = false;
    mnoznik_trudnosci = 1;
    btn_normal_was_pressed = false;
    btn_hard_was_pressed = false;

    tower_scene = re_loader->load("res://Tower.tscn");
    enemy_scene = re_loader->load("res://WagonikZWrogiem.tscn");
}

Spawner::~Spawner() {}

void Spawner::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Label* hp_label = Object::cast_to<Label>(get_node_or_null("/root/Poziom/CanvasLayer/BaseHPLabel"));
    if (hp_label != nullptr && hp_label->get_text().to_int() <= 0) {
        
        Control* game_over_panel = Object::cast_to<Control>(get_node_or_null("/root/Poziom/CanvasLayer/GameOverPanel"));
        if (game_over_panel != nullptr) {
            game_over_panel->show(); 

            Button* btn_restart = Object::cast_to<Button>(game_over_panel->get_node_or_null("BtnRestart"));
            if (btn_restart != nullptr && btn_restart->is_pressed()) {
                get_tree()->reload_current_scene(); 
            }
        }
        
        return; 
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

    // nowa obsluga stawiania wiez (RAYCASTING)
    Button* buy_btn = Object::cast_to<Button>(get_node_or_null("/root/Poziom/CanvasLayer/BuyTowerButton"));
    
    // sprawdzamy przycisk w GUI
    if (buy_btn != nullptr) {
        bool is_pressed = buy_btn->is_pressed();
        
        if (is_pressed && !buy_button_was_pressed) {
            // wlaczamy lub wylaczamy tryb budowania (jak gracz kliknie znowu, to anuluje)
            is_building_mode = !is_building_mode; 
            if (is_building_mode) {
                UtilityFunctions::print("Tryb budowania WLACZONY! Kliknij na mape, by postawic wieze.");
            } else {
                UtilityFunctions::print("Tryb budowania WYLACZONY.");
            }
        }
        buy_button_was_pressed = is_pressed;
    }

    // jesli jestesmy w trybie budowania, czekamy na klikniecie na mapie
    if (is_building_mode) {
        Input* input = Input::get_singleton();
        bool is_clicking = input->is_mouse_button_pressed((MouseButton)1); // lewy przycisk

        if (is_clicking && !mouse_was_clicking) {
            
            Viewport* vp = get_viewport();
            Camera3D* camera = vp->get_camera_3d();
            
            if (camera != nullptr) {
                // konwersja klikniecia 2D na promien w swiecie 3D
                Vector2 mouse_pos = vp->get_mouse_position();
                Vector3 from = camera->project_ray_origin(mouse_pos);
                Vector3 to = from + camera->project_ray_normal(mouse_pos) * 1000.0; // promien o dlugosci 1000 metrow
                
                PhysicsDirectSpaceState3D* space_state = get_world_3d()->get_direct_space_state();
                Ref<PhysicsRayQueryParameters3D> query = PhysicsRayQueryParameters3D::create(from, to);
                
                // puszczamy promien i sprawdzamy, czy w cos uderzyl
                Dictionary result = space_state->intersect_ray(query);
                
                if (!result.is_empty()) {
                    
                    // sprawdzamy w co klinelismy 
                    Node* uderzony_obiekt = Object::cast_to<Node>(result["collider"]);
                    
                    // jesli obiekt istnieje i nalezy do grupy "ziemia"
                    if (uderzony_obiekt != nullptr && uderzony_obiekt->is_in_group("ziemia")) {
                        
                        // mamy punkt uderzenia w dobre miejsce
                        Vector3 pozycja_na_mapie = result["position"];
                        
                        // sprawdzamy kase
                        Label* gold_label = Object::cast_to<Label>(get_node_or_null("/root/Poziom/CanvasLayer/GoldLabel"));
                        if (gold_label != nullptr && tower_scene.is_valid()) {
                            int aktualne_zloto = gold_label->get_text().to_int();
                            int koszt_wiezy = 200;
                            
                            if (aktualne_zloto >= koszt_wiezy) {
                                // zabieramy zloto
                                gold_label->set_text(String::num_int64(aktualne_zloto - koszt_wiezy));
                                
                                // tworzymy i stawiamy wieze
                                Node* nowa_wieza = tower_scene->instantiate();
                                get_parent()->add_child(nowa_wieza);
                                
                                Node3D* wieza_3d = Object::cast_to<Node3D>(nowa_wieza);
                                if (wieza_3d != nullptr) {
                                    wieza_3d->set_global_position(pozycja_na_mapie);
                                }
                                
                                zbudowane_wieze++;
                                UtilityFunctions::print("Wieza postawiona! Pozostalo zlota: ", aktualne_zloto - koszt_wiezy);
                                
                                // wylaczamy tryb budowania
                                is_building_mode = false;
                            } else {
                                UtilityFunctions::print("Za malo zlota na nowa wieze!");
                                is_building_mode = false; // anulujemy budowe, bo nie ma kasy
                            }
                        }
                    } else {
                        // jesli gracz kliknal w sciezke albo cokolwiek innego
                        UtilityFunctions::print("W tym miejscu nie mozesz budowac!");
                        is_building_mode = false; // anulujemy tryb budowania
                    }
                }
            }
        }
        mouse_was_clicking = is_clicking;
    } else {
        mouse_was_clicking = false; // resetujemy myszke, gdy nie jestesmy w trybie budowania
    }

}