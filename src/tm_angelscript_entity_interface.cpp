#include "tm_angelscript_entity_interface.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include <new>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering something Angelscript, %d", __LINE__);} } while(false)

extern "C" {
#include <plugins/entity/entity.h>
#include <foundation/log.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_assets.h>
#include <the_machinery/the_machinery.h>
#include <the_machinery/scene_tab.h>
}

namespace tm_entity {

	tm_entity_context_o* create_context(tm_entity_create_components flags) {
		return tm_entity_api->create_context(get_system()->allocator, get_system()->tt, flags);
	}

	void destroy_context(tm_entity_context_o* ctx) {
		return tm_entity_api->destroy_context(ctx);
	}

	/*tm_component_type_t register_component(tm_entity_context_o* ctx, const struct tm_component_i* com) {
		return tm_entity_api->register_component(ctx, com);
	}*/

	tm_entity_t create_entity(tm_entity_context_o* ctx) {/*
		tm_tt_id_t sphere = tm_the_truth_assets_api->asset_from_path(get_system()->tt, get_system()->asset_root, "entities/sphere");
		const tm_the_truth_object_o* entity_read_obj = tm_the_truth_api->read(get_system()->tt, sphere);
		tm_tt_id_t sphere_asset = tm_the_truth_api->get_subobject(get_system()->tt, entity_read_obj, TM_TT_PROP__ASSET__OBJECT);*/
		tm_entity_t e = tm_entity_api->create_entity(ctx);
		return e;
	}

	void register_tm_entity_interface(asIScriptEngine* engine) {
		int r = engine->RegisterObjectType("tm_entity_context_o", sizeof(tm_entity_context_o*), asOBJ_REF | asOBJ_NOCOUNT);  AS_CHECK(r);
		r = engine->RegisterObjectType("tm_component_i", sizeof(tm_component_i), asOBJ_REF | asOBJ_NOCOUNT);  AS_CHECK(r);
		r = engine->RegisterObjectType("tm_entity_t", sizeof(tm_entity_t), asOBJ_VALUE | asOBJ_POD);  AS_CHECK(r);

		r = engine->RegisterObjectType("tm_component_type_t", sizeof(tm_component_type_t), asOBJ_VALUE | asOBJ_POD);  AS_CHECK(r);

		AS_CHECK(engine->RegisterEnum("tm_entity_create_components")); 
		AS_CHECK(engine->RegisterEnumValue("tm_entity_create_components", "TM_ENTITY_CREATE_COMPONENTS_NONE", TM_ENTITY_CREATE_COMPONENTS_NONE));
		AS_CHECK(engine->RegisterEnumValue("tm_entity_create_components", "TM_ENTITY_CREATE_COMPONENTS_ALL", TM_ENTITY_CREATE_COMPONENTS_ALL));
		AS_CHECK(engine->RegisterEnumValue("tm_entity_create_components", "TM_ENTITY_CREATE_COMPONENTS_EDITOR", TM_ENTITY_CREATE_COMPONENTS_EDITOR));
		
		r = engine->SetDefaultNamespace("tm_entity_api");
		r = engine->RegisterGlobalFunction("tm_entity_context_o@ create_context(tm_entity_create_components flags)", asFUNCTION(create_context), asCALL_CDECL);
		r = engine->RegisterGlobalFunction("void destroy_context(tm_entity_context_o@ ctx)", asFUNCTION(destroy_context), asCALL_CDECL);
		r = engine->RegisterGlobalFunction("tm_entity_t create_entity(tm_entity_context_o@ ctx)", asFUNCTION(create_entity), asCALL_CDECL);
		r = engine->SetDefaultNamespace("");
	}
}