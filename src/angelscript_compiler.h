#pragma once 
#include <stdint.h>

class asIScriptEngine;
class asIScriptModule;
struct tm_temp_allocator_i;

namespace as_compiler{
	void initialize(asIScriptEngine* engine);
	uint64_t compile_file_to_bytecode(asIScriptEngine* engine, const char* project_root, const char* main_file, const char* module_name, void** out_bytecode, tm_temp_allocator_i* ta);
	asIScriptModule* get_module_from_bytecode(const char* name, uint64_t size,const void* bc);
	void compiler_string_to_bytecode(const char* filename);
}