import os

env = Environment()

# pliki Godota
env.Append(CPPPATH=["godot-cpp/gdextension", "godot-cpp/include", "godot-cpp/gen/include", "src"])
env.Append(LIBPATH=["godot-cpp/bin"])

# ustawienia dla Windowsa i wymuszenie C++17
env.Append(CXXFLAGS=["/std:c++17", "/EHsc"])

env.Append(LIBS=["libgodot-cpp.windows.template_debug.x86_64.lib"])

sources = Glob("src/*.cpp")

env.SharedLibrary("bin/versusdefense.dll", sources)