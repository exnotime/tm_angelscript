#pragma once 

class asIScriptEngine;

namespace as_compiler{

	void initialize(asIScriptEngine* engine);
	void compile_file_to_bytecode(const char* filename);
	void compiler_string_to_bytecode(const char* filename);
}