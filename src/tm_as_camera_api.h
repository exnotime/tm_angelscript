#pragma once
class asIScriptEngine;
struct tm_allocator_i;
namespace tm_camera {
	void register_camera_interface(asIScriptEngine* engine, tm_allocator_i* allocator);
}