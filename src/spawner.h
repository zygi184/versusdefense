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
    double spawn_delay; // co ile sekund ma pojawic sie wrog
    Ref<PackedScene> enemy_scene; // tu bedzie przechowywana scena wroga (.tscn)

protected:
    static void _bind_methods();

public:
    Spawner();
    ~Spawner();

    void _process(double delta) override;
};

}

#endif