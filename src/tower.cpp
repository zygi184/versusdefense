#include "tower.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/area3d.hpp> // oblsuga stref (radarow i hitboxow)

using namespace godot;

void Tower::_bind_methods() {}

Tower::Tower() {
    damage = 25.0;
    range = 5.0;
}

Tower::~Tower() {}

void Tower::_process(double delta) {
    // szukamynaszego radaru (dziecka o nazwie "Zasieg")
    Area3D* radar = Object::cast_to<Area3D>(get_node_or_null("Zasieg"));

    if (radar != nullptr) {
        // pobieramy listę wszystkich hitboxow, ktore weszly w nasz radar
        TypedArray<Area3D> obiekty_w_zasiegu = radar->get_overlapping_areas();

        // przegladamy te liste i szukamy wrogow
        for (int i = 0; i < obiekty_w_zasiegu.size(); i++) {
            Area3D* cel = Object::cast_to<Area3D>(obiekty_w_zasiegu[i]);
            
            // jesli obiekt ma latke "wrogowie", to znaczy, ze znalezlismy cel
            if (cel != nullptr && cel->is_in_group("wrogowie")) {
                UtilityFunctions::print("Widze wroga! Pif paf!");
                
                // przerywamy petle, zeby wieza strzelala tylko do jednego na raz
                break; 
            }
        }
    }
}