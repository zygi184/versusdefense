#include "enemy.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/path_follow3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

using namespace godot;

void Enemy::_bind_methods() {
}

Enemy::Enemy() {
    hp = 100;
    speed = 2.0;
}

Enemy::~Enemy() {}

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
            Label* hp_label = Object::cast_to<Label>(get_node_or_null("/root/Poziom/CanvasLayer/BaseHPLabel"));
            
            if (hp_label != nullptr) {
                int aktualne_hp = hp_label->get_text().to_int();
                int nowe_hp = aktualne_hp - 10;
                
                // Blokujemy HP, zeby nie spadlo ponizej zera
                if (nowe_hp <= 0) {
                    hp_label->set_text("0");
                } else {
                    hp_label->set_text(String::num_int64(nowe_hp)); 
                }
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
        UtilityFunctions::print("Wrog pokonany! Zarabiasz zloto.");
        
        Label* gold_label = Object::cast_to<Label>(get_node_or_null("/root/Poziom/CanvasLayer/GoldLabel"));
        
        if (gold_label != nullptr) {
            int aktualne_zloto = gold_label->get_text().to_int();
            gold_label->set_text(String::num_int64(aktualne_zloto + 15)); 
        }

        queue_free();
    }
}

void Enemy::set_max_hp(int new_hp) {
    hp = new_hp;
}

void Enemy::set_speed(double new_speed) {
    speed = new_speed;
}