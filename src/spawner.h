#ifndef SPAWNER_H
#define SPAWNER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

namespace godot {

class Spawner : public Node3D {
    GDCLASS(Spawner, Node3D)

private:
    double timer;
    double spawn_delay;
    Ref<PackedScene> tower_scene;
    bool buy_button_was_pressed;
    Ref<PackedScene> enemy_scene;
    
    // obsluga fal
    int obecna_fala;
    int wrogowie_w_fali;
    int wyprodukowani_wrogowie;
    double czas_do_kolejnej_fali;
    bool przerwa_miedzy_falami;

protected:
    static void _bind_methods();

public:
    Spawner();
    ~Spawner();

    void _process(double delta) override;
};

}

#endif