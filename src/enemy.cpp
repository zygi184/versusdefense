#include "enemy.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/path_follow3d.hpp>

using namespace godot;

void Enemy::_bind_methods() {
}

Enemy::Enemy() {
    hp = 100;
    speed = 2.0;
}

Enemy::~Enemy() {}

void Enemy::_process(double delta) {
    Node* parent = get_parent();
    PathFollow3D* wagonik = Object::cast_to<PathFollow3D>(parent);

    if (wagonik != nullptr) {
        // ruch wroga
        double aktualna_pozycja = wagonik->get_progress();
        wagonik->set_progress(aktualna_pozycja + speed * delta);

        // sprawdzamy, czy dojechalismy do konca trasy
        // get_progress_ratio() zwraca wartosc od 0.0 (start) do 1.0 (koniec)
        if (wagonik->get_progress_ratio() >= 1.0) {
            UtilityFunctions::print("Wrog przedarl sie przez obrone! Baza traci HP!");
            queue_free(); // usuwamy wroga ze sceny, zeby nie stal na koncu torow
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