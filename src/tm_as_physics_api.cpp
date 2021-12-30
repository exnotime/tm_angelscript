#include "tm_as_physics_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include "tm_as_types.h"//for transform
#include "tm_as_array.h"
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering physics to Angelscript, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <plugins/entity/entity.h>
#include <foundation/log.h>
#include <plugins/physx/physx_scene.h>
#include <foundation/temp_allocator.h>
}
using namespace tm_array;
//Combines physx and physics api for convenience
namespace tm_physics {

	void add_force(tm_physx_scene_o* physx, tm_entity_t e, tm_vec3_t force, uint32_t flags) {
		tm_physx_scene_api->add_force(physx, e, force, flags);
	}

	void add_torque(tm_physx_scene_o* physx, tm_entity_t e, tm_vec3_t torque, uint32_t flags) {
		tm_physx_scene_api->add_torque(physx, e, torque, flags);
	}

	tm_script_array_t* raycast(tm_physx_scene_o* physx, tm_vec3_t from, tm_vec3_t dir, float distance, tm_tt_id_t collision_id, bool ignore_static, bool ignore_dynamic, uint32_t max_touches) {
		TM_INIT_TEMP_ALLOCATOR(ta);
		tm_physx_raycast_flags_t flags = { ignore_static , ignore_dynamic };
		tm_physx_raycast_hit_t* touches = (tm_physx_raycast_hit_t*)tm_temp_alloc(ta, sizeof(tm_physx_raycast_hit_t) * max_touches);
		tm_physx_raycast_t raycast = tm_physx_scene_api->raycast(physx, from, dir, distance, collision_id, flags, touches, max_touches);
		
		if (asIScriptContext* ctx = asGetActiveContext()) {
			asITypeInfo* type = ctx->GetEngine()->GetTypeInfoByDecl("tm_array_t<tm_physx_raycast_hit_t>");
			tm_script_array_t* output_array = tm_array::create(type, 0, nullptr);
			for (uint32_t i = 0; i < raycast.num_touches; ++i) {
				tm_array::push(&touches[i], output_array);
			}
			TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
			return output_array;
		}

		TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
		return nullptr;
	}

	void register_physics_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {

		AS_CHECK(engine->RegisterObjectType("tm_physx_raycast_hit_t", sizeof(tm_physx_raycast_hit_t), asOBJ_VALUE | asOBJ_POD));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_raycast_hit_t", "tm_vec3_t pos", asOFFSET(tm_physx_raycast_hit_t, pos)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_raycast_hit_t", "tm_vec3_t normal", asOFFSET(tm_physx_raycast_hit_t, normal)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_raycast_hit_t", "float distance", asOFFSET(tm_physx_raycast_hit_t, distance)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_raycast_hit_t", "tm_entity_t body", asOFFSET(tm_physx_raycast_hit_t, body)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_raycast_hit_t", "tm_entity_t shape", asOFFSET(tm_physx_raycast_hit_t, shape)));

		AS_CHECK(engine->RegisterEnum("tm_physx_force_flags"));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__FORCE", TM_PHYSX_FORCE_FLAGS__FORCE));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__IMPULSE", TM_PHYSX_FORCE_FLAGS__IMPULSE));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__ACCELERATION", TM_PHYSX_FORCE_FLAGS__ACCELERATION));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__VELOCITY_CHANGE", TM_PHYSX_FORCE_FLAGS__VELOCITY_CHANGE));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__CONTINUOUS", TM_PHYSX_FORCE_FLAGS__CONTINUOUS));

		AS_CHECK(engine->SetDefaultNamespace("tm_physx_scene_api"));
		AS_CHECK(engine->RegisterGlobalFunction("void add_force(tm_physx_scene_o@ physx, tm_entity_t e, tm_vec3_t force, tm_physx_force_flags flags)", asFUNCTION(add_force), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void add_torque(tm_physx_scene_o@ physx, tm_entity_t e, tm_vec3_t torque, tm_physx_force_flags flags)", asFUNCTION(add_torque), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_array_t<tm_physx_raycast_hit_t>@ raycast(tm_physx_scene_o@ physx, tm_vec3_t from, tm_vec3_t dir, float distance, tm_tt_id_t collision_id, bool ignore_static = false, bool ignore_dynamic = false, uint max_touches = 4)", asFUNCTION(raycast), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}