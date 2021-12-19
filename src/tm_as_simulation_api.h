#pragma once
class asIScriptEngine;
struct tm_allocator_i;
namespace tm_simulation {
	void register_simulation_interface(asIScriptEngine* engine, tm_allocator_i* allocator);
}