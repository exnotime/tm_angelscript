#pragma once
class asIScriptEngine;
struct tm_allocator_i;

namespace tm_string{
	void register_tm_string_interface(asIScriptEngine* engine, tm_allocator_i* allocator);
	void destroy_string_factory(tm_allocator_i* allocator);
}