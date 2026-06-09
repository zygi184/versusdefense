#include "sniper.h"
#include "spawner.h" 
#include "enemy.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void Sniper::_bind_methods() {}

Sniper::Sniper() {
    damage = 800.0; // potezne obrazenia
    range = 15.0; // wielki zasieg
    fire_rate = 2.0; // strzela raz na 2 sekundy
    time_since_last_shot = 0.0;
    laser_timer = 0.0;
}

Sniper::~Sniper() {}

void Sniper::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return; 
    }

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

    // strzela TYLKO, jesli minely 3 sekundy od ostatniego strzalu
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
                        time_since_last_shot = 0.0; // Resetujemy stoper
                        
                        Node3D* laser = Object::cast_to<Node3D>(get_node_or_null("Laser"));
                        Node3D* cel_3d = Object::cast_to<Node3D>(cel);
                        
                        if (laser != nullptr && cel_3d != nullptr) {
                            Vector3 pozycja_wiezy = get_global_position();
                            pozycja_wiezy.y += 1.0; 
                            
                            Vector3 pozycja_wroga = cel_3d->get_global_position();
                            double dystans = pozycja_wiezy.distance_to(pozycja_wroga);
                            
                            laser->set_global_position(pozycja_wiezy.lerp(pozycja_wroga, 0.5));
                            laser->set_scale(Vector3(1.0, dystans, 1.0));
                            laser->look_at(pozycja_wroga, Vector3(0, 1, 0));
                            laser->rotate_object_local(Vector3(1, 0, 0), 1.5708);
                            
                            laser->show();
                            laser_timer = 0.2; // laser snajpera swieci dluzej
                        }
                    }
                    
                    break; // snajper uderza tylko jednego wroga naraz
                }
            }
        }
    }
}