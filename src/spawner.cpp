#include "spawner.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/path_follow3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void Spawner::_bind_methods() {}

Spawner::Spawner() {
    timer = 0.0;
    spawn_delay = 4.0; // nowy wrog co 4 sekundy
    
    // automatyczne ladowanie sceny wagonika z wrogiem
    ResourceLoader *re_loader = ResourceLoader::get_singleton();
    enemy_scene = re_loader->load("res://WagonikZWrogiem.tscn");
}

Spawner::~Spawner() {}

void Spawner::_process(double delta) {
   
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    
    timer += delta;

    if (timer >= spawn_delay) {
        timer = 0.0;

        if (enemy_scene.is_valid()) {
            // tworzymy nowa instancje wroga
            Node *nowy_wrog = enemy_scene->instantiate();
            
            // szukamy Path3D, do ktorego mamy dodac wroga
            // zakladamy, ze Spawner jest dzieckiem lub sasiadem Path3D
            Node *path_node = get_node_or_null("../Path3D"); 
            
            if (path_node != nullptr) {
                path_node->add_child(nowy_wrog);
                UtilityFunctions::print("Nowy wrog nadchodzi!");
            }
        }
    }
}