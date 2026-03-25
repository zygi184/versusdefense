#include "tower.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Tower::_bind_methods() {
}

Tower::Tower() {
    damage = 25.0; // Podstawowe obrażenia
    range = 5.0;   // Zasięg widzenia wroga
}

Tower::~Tower() {}

void Tower::_process(double delta) {
    // zaraz zostanie dodana logika szukania wroga i strzelania!
}