#pragma once
class asIScriptEngine;
struct tm_allocator_i;
namespace tm_physics {
	void register_physics_interface(asIScriptEngine* engine, tm_allocator_i* allocator);
}