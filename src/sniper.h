#ifndef SNIPER_H
#define SNIPER_H

#include <godot_cpp/classes/node3d.hpp>

namespace godot {

class Sniper : public Node3D {
    GDCLASS(Sniper, Node3D)

private:
    double damage;
    double range;
    double fire_rate;
    double time_since_last_shot;
    double laser_timer;

protected:
    static void _bind_methods();

public:
    Sniper();
    ~Sniper();

    void _process(double delta) override;
};

}

#endif