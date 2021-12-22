#pragma once
class asIScriptEngine;
struct tm_allocator_i;
extern "C" {
#include <foundation/string.h>
}
namespace tm_string{
	//Copies the string and creates a new angelscript string, this is needed to destroy the string cleanly from Angelscript
	tm_str_t create_string(const char* str);

	void register_tm_string_interface(asIScriptEngine* engine, tm_allocator_i* allocator);
	void destroy_string_factory(tm_allocator_i* allocator);
}