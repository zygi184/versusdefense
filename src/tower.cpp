#include "tower.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/area3d.hpp>
#include "enemy.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/button.hpp>

using namespace godot;

void Tower::_bind_methods() {}

Tower::Tower() {
    damage = 100.0;
    range = 5.0;
    fire_rate = 0.5; 
    time_since_last_shot = 0.0;
    button_was_pressed = false; 
    laser_timer = 0.0;
}

Tower::~Tower() {}

void Tower::upgrade_tower() {
    Label* gold_label = Object::cast_to<Label>(get_node_or_null("/root/Poziom/CanvasLayer/GoldLabel"));
    
    if (gold_label != nullptr) {
        int aktualne_zloto = gold_label->get_text().to_int();
        int koszt_ulepszenia = 50;

        if (aktualne_zloto >= koszt_ulepszenia) {
            // zabieramy kase
            gold_label->set_text(String::num_int64(aktualne_zloto - koszt_ulepszenia));
            
            // zwiekszamy statystyki (np. szybkosc strzalu o 10 procent i 25 damage)
            fire_rate *= 0.9; 
            damage += 25.0;
            
            UtilityFunctions::print("Wieza ulepszona! Nowy damage: ", damage);
        } else {
            UtilityFunctions::print("Za malo zlota na ulepszenie!");
        }
    }
}

void Tower::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
         return; 
    }
    
    Button* btn = Object::cast_to<Button>(get_node_or_null("/root/Poziom/CanvasLayer/Button"));
    
    if (btn != nullptr) {
        bool is_pressed = btn->is_pressed();
        
        // jesli przycisk jest wcisniety TERAZ, a klatke wczesniej NIE BYL
        if (is_pressed && !button_was_pressed) {
            upgrade_tower(); // wywolujemy funkcje ulepszenia!
        }
        
        button_was_pressed = is_pressed; // zapamietujemy stan na nastepna klatke
    }

    // zwiekszamy stoper o czas, ktory minal od ostatniej klatki
    time_since_last_shot += delta;

    if (laser_timer > 0.0) {
        laser_timer -= delta;
        if (laser_timer <= 0.0) {
            Node3D* laser = Object::cast_to<Node3D>(get_node_or_null("Laser"));
            if (laser != nullptr) {
                laser->hide(); 
            }
        }
    }

    // wieza w ogole probuje szukac celu TYLKO, jesli minela juz sekunda od ostatniego strzalu
    if (time_since_last_shot >= fire_rate) {
        
        Area3D* radar = Object::cast_to<Area3D>(get_node_or_null("Zasieg"));

        if (radar != nullptr) {
            TypedArray<Area3D> obiekty_w_zasiegu = radar->get_overlapping_areas();

            for (int i = 0; i < obiekty_w_zasiegu.size(); i++) {
                Area3D* cel = Object::cast_to<Area3D>(obiekty_w_zasiegu[i]);
                
                if (cel != nullptr && cel->is_in_group("wrogowie")) {
                    
                    Node* rodzic = cel->get_parent();
                    Enemy* wrog = Object::cast_to<Enemy>(rodzic);

                    if (wrog != nullptr) {
                        wrog->take_damage((int)damage); 
                        time_since_last_shot = 0.0; 
                        
                        Node3D* laser = Object::cast_to<Node3D>(get_node_or_null("Laser"));
                        Node3D* cel_3d = Object::cast_to<Node3D>(cel);
                        
                        if (laser != nullptr && cel_3d != nullptr) {
                            Vector3 pozycja_wiezy = get_global_position();
                            
                            // punkt startowy o 1 metr do gory, zeby strzelalo z lufy, a nie z ziemi
                            pozycja_wiezy.y += 1.0; 
                            
                            Vector3 pozycja_wroga = cel_3d->get_global_position();
                            
                            // odleglosc
                            double dystans = pozycja_wiezy.distance_to(pozycja_wroga);
                            
                            // laser dokladnie w polowie drogi miedzy wieza a wrogiem
                            laser->set_global_position(pozycja_wiezy.lerp(pozycja_wroga, 0.5));
                            
                            // rozciagamy laser (os Y) na dlugosc calej odleglosci
                            laser->set_scale(Vector3(1.0, dystans, 1.0));
                            
                            // obracamy laser, zeby wylot lufy patrzyl na wroga
                            laser->look_at(pozycja_wroga, Vector3(0, 1, 0));
                            
                            // Godot rysuje cylinder w pionie, a my celujemy przodem (os Z),
                            // musimy go "polozyc" o 90 stopni (1.5708 radianow)
                            laser->rotate_object_local(Vector3(1, 0, 0), 1.5708);
                            
                            // pokazujemy go graczowi i nastawiamy stoper na 0.1 sekundy
                            laser->show();
                            laser_timer = 0.1;
                        }
                    }
                    
                    break; 
                }
            }
        }
    }
}