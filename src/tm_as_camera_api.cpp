#include "tm_as_entity_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include "tm_as_types.h"//for transform
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering camera to Angelscript, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <plugins/entity/entity.h>
#include <foundation/log.h>
#include <foundation/camera.h>
}
namespace tm_camera {

	void update_free_flight(tm_transform_t* transform, tm_vec3_t translation, tm_vec2_t rotation) {
		tm_camera_api->update_free_flight(transform, translation, rotation);
	}

	void update_pan(tm_transform_t* transform, tm_vec3_t& focus_position, tm_vec2_t pan, bool has_focus_position) {
		tm_camera_api->update_pan(transform, has_focus_position ? &focus_position : nullptr, pan);
	}

	void update_maya(tm_transform_t* transform, tm_vec3_t focus_position, float zoom, tm_vec2_t rotation) {
		tm_camera_api->update_maya(transform, focus_position, zoom, rotation);
	}

	void register_camera_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {
		AS_CHECK(engine->SetDefaultNamespace("tm_camera_api"));
		AS_CHECK(engine->RegisterGlobalFunction("void update_free_flight(const tm_transform_t@ transform, tm_vec3_t t, tm_vec2_t rot)", asFUNCTION(update_free_flight), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void update_pan(const tm_transform_t@ transform, tm_vec3_t focus, tm_vec2_t pan, bool has_focus)", asFUNCTION(update_pan), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void update_maya(const tm_transform_t@ transform, tm_vec3_t focus, float zoom, tm_vec2_t rot)", asFUNCTION(update_maya), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}