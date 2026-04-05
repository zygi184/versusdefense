#include "register_types.h"
#include "enemy.h" //  plik z wrogiem
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include "tower.h" // plik z wieżą
#include "spawner.h" // plik z spawnerem

using namespace godot;

void initialize_versusdefense_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    // rejestrujemy klasę:
    ClassDB::register_class<Enemy>();
    ClassDB::register_class<Tower>();
    ClassDB::register_class<Spawner>();
}

void uninitialize_versusdefense_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
GDExtensionBool GDE_EXPORT versusdefense_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_versusdefense_module);
    init_obj.register_terminator(uninitialize_versusdefense_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}