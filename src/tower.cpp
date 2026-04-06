#include "tower.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/area3d.hpp>
#include "enemy.h"
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void Tower::_bind_methods() {}

Tower::Tower() {
    damage = 100.0;
    range = 5.0;
    fire_rate = 0.5; // wieza strzela dokladnie 1 raz na sekunde
    time_since_last_shot = 0.0; // stoper startuje od zera
}

Tower::~Tower() {}

void Tower::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
         return; 
        }
    // zwiekszamy stoper o czas, ktory minal od ostatniej klatki
    time_since_last_shot += delta;

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
                        
                        // po udanym strzale zerujemy stoper, wieza znowu musi czekac sekunde
                        time_since_last_shot = 0.0; 
                    }
                    
                    break; 
                }
            }
        }
    }
}