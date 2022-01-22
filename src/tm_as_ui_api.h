#pragma once
class asIScriptEngine;
struct tm_allocator_i;
namespace tm_ui {
	void register_ui_interface(asIScriptEngine* engine, tm_allocator_i* allocator);
}