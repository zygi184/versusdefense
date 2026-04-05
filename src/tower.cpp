#include "tower.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/area3d.hpp> // obsluga stref (radarow i hitboxow)
#include "enemy.h"

using namespace godot;

void Tower::_bind_methods() {}

Tower::Tower() {
    damage = 25.0;
    range = 5.0;
}

Tower::~Tower() {}

void Tower::_process(double delta) {
    // znajdujemy radar
    Area3D* radar = Object::cast_to<Area3D>(get_node_or_null("Zasieg"));

    if (radar != nullptr) {
        // pobieramy liste wszystkich obiektow w zasiegu radaru
        TypedArray<Area3D> obiekty_w_zasiegu = radar->get_overlapping_areas();

        // przegladamy te liste w poszukiwaniu wroga
        for (int i = 0; i < obiekty_w_zasiegu.size(); i++) {
            Area3D* cel = Object::cast_to<Area3D>(obiekty_w_zasiegu[i]);
            
            // jesli obiekt ma latke "wrogowie", to znaczy, ze znalezlismy ofiare
            if (cel != nullptr && cel->is_in_group("wrogowie")) {
                
                // kod wroga siedzi w obiekcie nadrzednym (rodzicu).
                Node* rodzic = cel->get_parent();
                
                // tłumaczymy pamięci komputera, że ten rodzic to nasza klasa Enemy z C++.
                Enemy* wrog = Object::cast_to<Enemy>(rodzic);

                // jeśli udało się znaleźć wroga, uderzamy bezpośrednio w jego funkcj
                if (wrog != nullptr) {
                    wrog->take_damage(25); // zadajemy 25 obrażeń
                }
                
                // przerywamy petle, zeby wieza strzelala tylko do jednego na raz
                break; 
            }
        }
    }
}