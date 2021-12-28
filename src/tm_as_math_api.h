#pragma once
class asIScriptEngine;
struct tm_allocator_i;
namespace tm_math {
	void register_math_interface(asIScriptEngine* engine, tm_allocator_i* allocator);
}