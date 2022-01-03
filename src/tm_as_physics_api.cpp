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
#include <foundation/carray.inl>
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

	void set_velocity(tm_physx_scene_o* physx, tm_entity_t e, tm_vec3_t vel) {
		tm_physx_scene_api->set_velocity(physx, e, vel);
	}

	tm_vec3_t velocity(tm_physx_scene_o* physx, tm_entity_t e) {
		 return tm_physx_scene_api->velocity(physx, e);
	}

	void set_angular_velocity(tm_physx_scene_o* physx, tm_entity_t e, tm_vec3_t vel) {
		tm_physx_scene_api->set_angular_velocity(physx, e, vel);
	}

	tm_vec3_t angular_velocity(tm_physx_scene_o* physx, tm_entity_t e) {
		return tm_physx_scene_api->angular_velocity(physx, e);
	}

	void set_kinematic(tm_physx_scene_o* physx, tm_entity_t e, bool is_kinematic) {
		tm_physx_scene_api->set_kinematic(physx, e, is_kinematic);
	}

	bool is_kinematic(tm_physx_scene_o* physx, tm_entity_t e) {
		return tm_physx_scene_api->is_kinematic(physx, e);
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

	tm_script_array_t* on_contact(tm_physx_scene_o* physx) {
		 tm_physx_on_contact_t* contacts =  tm_physx_scene_api->on_contact(physx);
		 uint64_t contact_count = tm_carray_size(contacts);
		 if (asIScriptContext* ctx = asGetActiveContext()) {
			 asITypeInfo* type = ctx->GetEngine()->GetTypeInfoByDecl("tm_array_t<tm_physx_on_contact_t>");
			 tm_script_array_t* output_array = tm_array::create(type, 0, nullptr);
			 for (uint64_t i = 0; i < contact_count; ++i) {
				 tm_array::push(&contacts[i], output_array);
			 }
			 return output_array;
		 }
		 return nullptr;
	}

	void update(tm_physx_scene_o* physx, float dt) {
		tm_physx_scene_api->update(physx, dt);
	}

	void register_physics_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {

		AS_CHECK(engine->RegisterObjectType("tm_physx_raycast_hit_t", sizeof(tm_physx_raycast_hit_t), asOBJ_VALUE | asOBJ_POD));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_raycast_hit_t", "tm_vec3_t pos", asOFFSET(tm_physx_raycast_hit_t, pos)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_raycast_hit_t", "tm_vec3_t normal", asOFFSET(tm_physx_raycast_hit_t, normal)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_raycast_hit_t", "float distance", asOFFSET(tm_physx_raycast_hit_t, distance)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_raycast_hit_t", "tm_entity_t body", asOFFSET(tm_physx_raycast_hit_t, body)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_raycast_hit_t", "tm_entity_t shape", asOFFSET(tm_physx_raycast_hit_t, shape)));

		AS_CHECK(engine->RegisterObjectType("tm_physx_on_contact_t", sizeof(tm_physx_on_contact_t), asOBJ_VALUE | asOBJ_POD));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_on_contact_t", "tm_entity_t actor_0", asOFFSET(tm_physx_on_contact_t, actor_0)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_on_contact_t", "tm_entity_t actor_1", asOFFSET(tm_physx_on_contact_t, actor_1)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_on_contact_t", "tm_entity_t shape_0", asOFFSET(tm_physx_on_contact_t, shape_0)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_on_contact_t", "tm_entity_t shape_1", asOFFSET(tm_physx_on_contact_t, shape_1)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_on_contact_t", "tm_vec3_t pos", asOFFSET(tm_physx_on_contact_t, pos)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_on_contact_t", "tm_vec3_t normal", asOFFSET(tm_physx_on_contact_t, normal)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_on_contact_t", "bool first_touch", asOFFSET(tm_physx_on_contact_t, first_touch)));
		AS_CHECK(engine->RegisterObjectProperty("tm_physx_on_contact_t", "bool lost_touch", asOFFSET(tm_physx_on_contact_t, lost_touch)));

		AS_CHECK(engine->RegisterEnum("tm_physx_force_flags"));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__FORCE", TM_PHYSX_FORCE_FLAGS__FORCE));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__IMPULSE", TM_PHYSX_FORCE_FLAGS__IMPULSE));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__ACCELERATION", TM_PHYSX_FORCE_FLAGS__ACCELERATION));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__VELOCITY_CHANGE", TM_PHYSX_FORCE_FLAGS__VELOCITY_CHANGE));
		AS_CHECK(engine->RegisterEnumValue("tm_physx_force_flags", "TM_PHYSX_FORCE_FLAGS__CONTINUOUS", TM_PHYSX_FORCE_FLAGS__CONTINUOUS));

		AS_CHECK(engine->SetDefaultNamespace("tm_physx_scene_api"));
		AS_CHECK(engine->RegisterGlobalFunction("void update(tm_physx_scene_o@ physx,float dt)", asFUNCTION(update), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void add_force(tm_physx_scene_o@ physx, tm_entity_t e, tm_vec3_t force, tm_physx_force_flags flags)", asFUNCTION(add_force), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void add_torque(tm_physx_scene_o@ physx, tm_entity_t e, tm_vec3_t torque, tm_physx_force_flags flags)", asFUNCTION(add_torque), asCALL_CDECL));

		AS_CHECK(engine->RegisterGlobalFunction("void set_velocity(tm_physx_scene_o@ physx, tm_entity_t e, tm_vec3_t vel)", asFUNCTION(set_velocity), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t velocity(tm_physx_scene_o@ physx, tm_entity_t e)", asFUNCTION(velocity), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_angular_velocity(tm_physx_scene_o@ physx, tm_entity_t e, tm_vec3_t vel)", asFUNCTION(set_angular_velocity), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t angular_velocity(tm_physx_scene_o@ physx, tm_entity_t e)", asFUNCTION(angular_velocity), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_kinematic(tm_physx_scene_o@ physx, tm_entity_t e, bool is_kinematic)", asFUNCTION(set_kinematic), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("bool is_kinematic(tm_physx_scene_o@ physx, tm_entity_t e)", asFUNCTION(is_kinematic), asCALL_CDECL));

		AS_CHECK(engine->RegisterGlobalFunction("tm_array_t<tm_physx_raycast_hit_t>@ raycast(tm_physx_scene_o@ physx, tm_vec3_t from, tm_vec3_t dir, float distance, tm_tt_id_t collision_id, bool ignore_static = false, bool ignore_dynamic = false, uint max_touches = 4)", asFUNCTION(raycast), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_array_t<tm_physx_on_contact_t>@ on_contact(tm_physx_scene_o@ physx)", asFUNCTION(on_contact), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}