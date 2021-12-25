#pragma once
extern "C" {
#include <foundation/api_types.h>
}
class asIScriptEngine;

struct tm_ref_transform_t {
	tm_transform_t t;
	uint32_t ref_count = 0;
};

void register_tm_types(asIScriptEngine* engine);