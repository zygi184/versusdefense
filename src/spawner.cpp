#include "spawner.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/path_follow3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "enemy.h" 
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/label.hpp>

using namespace godot;

void Spawner::_bind_methods() {}

Spawner::Spawner() {
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

    tower_scene = re_loader->load("res://Tower.tscn");
    enemy_scene = re_loader->load("res://WagonikZWrogiem.tscn");
}

Spawner::~Spawner() {}

void Spawner::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    timer += delta;

    // sprawdzamy, czy gra czeka na rozpoczecie nowej fali
    if (przerwa_miedzy_falami) {
        if (timer >= czas_do_kolejnej_fali) {
            przerwa_miedzy_falami = false;
            timer = 0.0;
            wyprodukowani_wrogowie = 0;
            UtilityFunctions::print("Rozpoczyna sie fala: ", obecna_fala);
        }
    } 
    // jesli fala trwa, produkujemy wrogow
    else {
        if (timer >= spawn_delay) {
            timer = 0.0;

            if (enemy_scene.is_valid()) {
                Node *nowy_wrog = enemy_scene->instantiate();
                Node *path_node = get_node_or_null("../Path3D"); 
                
                if (path_node != nullptr) {
                    path_node->add_child(nowy_wrog);
                    
                    // dobieramy sie do wroga i zwiekszamy mu HP w zaleznosci od fali
                    // zaczynaja z 100 HP, a kazda kolejna fala daje +50 HP
                    Enemy* kod_wroga = Object::cast_to<Enemy>(nowy_wrog->get_node_or_null("Enemy"));
                    if (kod_wroga != nullptr) {
                        kod_wroga->set_max_hp(100 + (obecna_fala * 50)); 
                    }

                    wyprodukowani_wrogowie++;
                }
            }

            // jesli wyprodukowalismy wszystkich wrogow na te fale, oglaszamy przerwe
            if (wyprodukowani_wrogowie >= wrogowie_w_fali) {
                przerwa_miedzy_falami = true;
                obecna_fala++;
                wrogowie_w_fali += 2; // kazda kolejna fala ma o 2 wrogow wiecej
                timer = 0.0;
                UtilityFunctions::print("Fala pokonana! Nastepna za 5 sekund.");
            }
        }
    }

    // RĘCZNA OBSŁUGA KUPUWANIA WIEŻY
    Button* buy_btn = Object::cast_to<Button>(get_node_or_null("/root/Poziom/CanvasLayer/BuyTowerButton"));
    
    if (buy_btn != nullptr) {
        bool is_pressed = buy_btn->is_pressed();
        
        if (is_pressed && !buy_button_was_pressed) {
            Label* gold_label = Object::cast_to<Label>(get_node_or_null("/root/Poziom/CanvasLayer/GoldLabel"));
            
            // Szukamy znaczników na mapie
            Node* miejsca_na_wieze = get_node_or_null("../MiejscaNaWieze"); 

            if (gold_label != nullptr && tower_scene.is_valid() && miejsca_na_wieze != nullptr) {
                int aktualne_zloto = gold_label->get_text().to_int();
                int koszt_wiezy = 200;
                int dostepne_miejsca = miejsca_na_wieze->get_child_count();

                if (aktualne_zloto >= koszt_wiezy) {
                    // Sprawdzamy czy mamy jeszcze wolne sloty na mapie
                    if (zbudowane_wieze < dostepne_miejsca) {
                        
                        // Pobieramy konkretny znacznik (Marker3D) z listy wg numeru
                        Node3D* znacznik = Object::cast_to<Node3D>(miejsca_na_wieze->get_child(zbudowane_wieze));

                        if (znacznik != nullptr) {
                            // Zabieramy zloto
                            gold_label->set_text(String::num_int64(aktualne_zloto - koszt_wiezy));
                            
                            // Klonujemy nowa wieze
                            Node* nowa_wieza = tower_scene->instantiate();
                            get_parent()->add_child(nowa_wieza);
                            
                            // Ustawiamy wieze DOKLADNIE w globalnej pozycji znacznika
                            Node3D* wieza_3d = Object::cast_to<Node3D>(nowa_wieza);
                            if (wieza_3d != nullptr) {
                                wieza_3d->set_global_position(znacznik->get_global_position());
                            }

                            zbudowane_wieze++; // Podbijamy licznik postawionych wiez
                            UtilityFunctions::print("Nowa wieza postawiona na miejscu nr: ", zbudowane_wieze);
                        }
                    } else {
                        UtilityFunctions::print("Brak wolnych miejsc na mapie!");
                    }
                } else {
                    UtilityFunctions::print("Za malo zlota na nowa wieze!");
                }
            }
        }
        buy_button_was_pressed = is_pressed;
    }

}