#ifndef TOWER_H
#define TOWER_H

#include <godot_cpp/classes/node3d.hpp>

namespace godot {

class Tower : public Node3D {
    GDCLASS(Tower, Node3D)

private:
    float damage;
    float range;

protected:
    static void _bind_methods();

public:
    Tower();
    ~Tower();

    void _process(double delta) override;
};

}
#endif