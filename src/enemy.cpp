#include "enemy.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/path_follow3d.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void Enemy::_bind_methods() {
}

Enemy::Enemy() {
    hp = 100;
    speed = 2.0;
}

Enemy::~Enemy() {}

// Dodaj ten include na górze pliku, jeśli go nie masz
#include <godot_cpp/classes/label.hpp>

void Enemy::_process(double delta) {
    
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Node* parent = get_parent();
    PathFollow3D* wagonik = Object::cast_to<PathFollow3D>(parent);

    if (wagonik != nullptr) {
        double aktualna_pozycja = wagonik->get_progress();
        wagonik->set_progress(aktualna_pozycja + speed * delta);

        if (wagonik->get_progress_ratio() >= 1.0) {
            // szukamy etykiety HP w scenie)
            Label* hp_label = Object::cast_to<Label>(get_node_or_null("/root/Poziom/CanvasLayer/BaseHPLabel"));
            
            if (hp_label != nullptr) {
                // pobieramy aktualny tekst, zamieniamy na liczbę, odejmujemy 1 i zapisujemy z powrotem
                int aktualne_hp = hp_label->get_text().to_int();
                hp_label->set_text(String::num_int64(aktualne_hp - 10)); // baza traci np. 10 HP
            }

            UtilityFunctions::print("Wrog uderzyl w baze!");
            queue_free(); 
        }
    }
}

void Enemy::take_damage(int amount) {
    hp -= amount;
    UtilityFunctions::print("Aua! Zostalo mi: ", hp, " HP");

    if (hp <= 0) {
        UtilityFunctions::print("Wrog pokonany!");
        queue_free();
    }
}