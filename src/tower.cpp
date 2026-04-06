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