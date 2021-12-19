#pragma once
class asIScriptEngine;
struct tm_allocator_i;
class asITypeInfo;
namespace tm_array {

	struct tm_script_array_t {
		mutable int ref_count;
		asITypeInfo* type;
		int sub_type_id;
		int element_size;
		bool gc;
		char* array;
	};

	void register_tm_array(asIScriptEngine* engine, tm_allocator_i* allocator);
}