#include "tm_as_physics_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include "tm_as_types.h"//for transform
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering physics to Angelscript, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <plugins/entity/entity.h>
#include <foundation/log.h>
#include <plugins/physx/physx_scene.h>
}

//Combines physx and physics api for convenience
namespace tm_physics {

	void add_force(tm_physx_scene_o* physx, tm_entity_t e, tm_vec3_t force, uint32_t flags) {
		tm_physx_scene_api->add_force(physx, e, force, flags);
	}

	void add_torque(tm_physx_scene_o* physx, tm_entity_t e, tm_vec3_t torque, uint32_t flags) {
		tm_physx_scene_api->add_torque(physx, e, torque, flags);
	}

	void register_physics_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {

		AS_CHECK(engine->RegisterEnum("tm_physx_force_flags"));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__FORCE", TM_PHYSX_FORCE_FLAGS__FORCE));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__IMPULSE", TM_PHYSX_FORCE_FLAGS__IMPULSE));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__ACCELERATION", TM_PHYSX_FORCE_FLAGS__ACCELERATION));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__VELOCITY_CHANGE", TM_PHYSX_FORCE_FLAGS__VELOCITY_CHANGE));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__CONTINUOUS", TM_PHYSX_FORCE_FLAGS__CONTINUOUS));

		AS_CHECK(engine->SetDefaultNamespace("tm_physx_scene_api"));
		AS_CHECK(engine->RegisterGlobalFunction("void add_force(tm_physx_scene_o@ physx, tm_entity_t e, tm_vec3_t force, uint flags)", asFUNCTION(add_force), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void add_torque(tm_physx_scene_o@ physx, tm_entity_t e, tm_vec3_t torque, uint flags)", asFUNCTION(add_torque), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}