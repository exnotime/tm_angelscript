#include "tm_as_component_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include "tm_as_types.h"
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering something Angelscript, %d", __LINE__);} } while(false)

extern "C" {
#include <plugins/entity/entity.h>
#include <foundation/log.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_assets.h>

#include <foundation/camera.h>
#include <plugins/entity/camera_component.h>
#include <plugins/entity/transform_component.h>
}

namespace tm_component {
	//Component lookup
	tm_component_type_t lookup_component_type(tm_entity_context_o* ctx, tm_strhash_t hashed_name) {
		return tm_entity_api->lookup_component_type(ctx, hashed_name);
	}

	tm_component_manager_o* component_manager(tm_entity_context_o* ctx, tm_component_type_t type) {	
		return tm_entity_api->component_manager(ctx, type);
	}

	//TODO: Expose engine component hashes instead of dicrete functions to keep the interface pure?
	tm_component_manager_o* transform_component_manager(tm_entity_context_o* ctx) {
		return tm_entity_api->component_manager_by_hash(ctx, TM_TT_TYPE_HASH__TRANSFORM_COMPONENT);
	}

	//Transform component
	tm_vec3_t get_position(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_position(man, e);
	}

	void set_position(tm_component_manager_o* manager, tm_entity_t e, tm_vec3_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_position(man, e, v);
	}

	tm_vec3_t get_scale(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_scale(man, e);
	}

	void set_scale(tm_component_manager_o* manager, tm_entity_t e, tm_vec3_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_scale(man, e, v);
	}

	tm_vec4_t get_rotation(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_rotation(man, e);
	}

	void set_rotation(tm_component_manager_o* manager, tm_entity_t e, tm_vec4_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_rotation(man, e, v);
	}

	const tm_transform_t* get_transform(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_transform(man, e);
	}

	void set_transform(tm_component_manager_o* manager, tm_entity_t e, const tm_transform_t* t) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_transform(man, e, t);
	}

	tm_vec3_t get_local_position(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_local_position(man, e);
	}

	void set_local_position(tm_component_manager_o* manager, tm_entity_t e, tm_vec3_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_local_position(man, e, v);
	}

	tm_vec3_t get_local_scale(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_local_scale(man, e);
	}

	void set_local_scale(tm_component_manager_o* manager, tm_entity_t e, tm_vec3_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_local_scale(man, e, v);
	}

	tm_vec4_t get_local_rotation(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_local_rotation(man, e);
	}

	void set_local_rotation(tm_component_manager_o* manager, tm_entity_t e, tm_vec4_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_local_rotation(man, e, v);
	}

	const tm_transform_t* get_local_transform(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_local_transform(man, e);
	}

	void set_local_transform(tm_component_manager_o* manager, tm_entity_t e, const tm_transform_t* t) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_local_transform(man, e, t);
	}

	void register_component_interface(asIScriptEngine* engine) {
		AS_CHECK(engine->RegisterObjectType("tm_component_manager_o", sizeof(tm_component_manager_o*), asOBJ_REF | asOBJ_NOCOUNT));
		
		

		AS_CHECK(engine->SetDefaultNamespace("tm_entity_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_component_type_t lookup_component_type(tm_entity_context_o@ ctx, tm_strhash_t hashed_name)", asFUNCTION(lookup_component_type), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_component_manager_o@ component_manager(tm_entity_context_o@ ctx, tm_component_type_t type)", asFUNCTION(component_manager), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_component_manager_o@ transform_component_manager(tm_entity_context_o@ ctx)", asFUNCTION(transform_component_manager), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace("tm_transform_component_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t get_position(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_position), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_position(tm_component_manager_o@ manager, tm_entity_t e, tm_vec3_t v)", asFUNCTION(set_position), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t get_scale(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_scale), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_scale(tm_component_manager_o@ manager, tm_entity_t e, tm_vec3_t v)", asFUNCTION(set_scale), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec4_t get_rotation(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_rotation), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_rotation(tm_component_manager_o@ manager, tm_entity_t e, tm_vec4_t v)", asFUNCTION(set_rotation), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("const tm_transform_t@ get_transform(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_transform), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_transform(tm_component_manager_o@ manager, tm_entity_t e,const tm_transform_t@ t)", asFUNCTION(set_transform), asCALL_CDECL));
		
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t get_local_position(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_local_position), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_local_position(tm_component_manager_o@ manager, tm_entity_t e, tm_vec3_t v)", asFUNCTION(set_local_position), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t get_local_scale(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_local_scale), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_local_scale(tm_component_manager_o@ manager, tm_entity_t e, tm_vec3_t v)", asFUNCTION(set_local_scale), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec4_t get_local_rotation(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_local_rotation), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_local_rotation(tm_component_manager_o@ manager, tm_entity_t e, tm_vec4_t v)", asFUNCTION(set_local_rotation), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("const tm_transform_t@ get_local_transform(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_local_transform), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_local_transform(tm_component_manager_o@ manager, tm_entity_t e, const tm_transform_t@ t)", asFUNCTION(set_local_transform), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}