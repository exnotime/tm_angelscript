#pragma once
class asIScriptEngine;
struct tm_allocator_i;
namespace tm_the_truth {
	void register_the_truth_interface(asIScriptEngine* engine, tm_allocator_i* allocator);
}