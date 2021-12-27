#pragma once
class asIScriptEngine;
struct tm_allocator_i;
namespace tm_script_component {
	void register_script_component_interface(asIScriptEngine* engine, tm_allocator_i* allocator);
}