#ifndef ENEMY_H
#define ENEMY_H

#include <godot_cpp/classes/node3d.hpp>

namespace godot {

class Enemy : public Node3D {
    GDCLASS(Enemy, Node3D)

private:
    int hp;
    double speed;

protected:
    static void _bind_methods();

public:
    Enemy();
    ~Enemy();

    void _process(double delta) override;
    void take_damage(int amount);
    void set_max_hp(int new_hp);
};

}

#endif