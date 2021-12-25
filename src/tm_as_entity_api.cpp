#include "tm_as_entity_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering entity Angelscript, %s:%d", __FILE__, __LINE__);} } while(false)

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

	tm_entity_t create_entity(tm_entity_context_o* ctx) {
		tm_entity_t e = tm_entity_api->create_entity(ctx);
		return e;
	}

	void destroy_entity(tm_entity_context_o* ctx, tm_entity_t e) {
		tm_entity_api->destroy_entity(ctx, e);
	}

	void clear_world(tm_entity_context_o* ctx) {
		tm_entity_api->clear_world(ctx);
	}

	void destroy_entity_command(tm_entity_commands_o* cmd, tm_entity_t e) {
		tm_entity_commands_api->destroy_entity(cmd, e);
	}

	void clear_world_command(tm_entity_commands_o* cmd) {
		tm_entity_commands_api->clear_world(cmd);
	}

	//Extensions
	//Creates an entity from an asset name
	tm_entity_t create_entity_from_asset(tm_the_truth_o* tt, tm_tt_id_t asset_root, tm_entity_context_o* ctx, tm_str_t& name) {
		tm_tt_id_t asset_id = tm_the_truth_assets_api->asset_from_path(tt, asset_root, name.data);
		const tm_the_truth_object_o* entity_asset_read_obj = tm_the_truth_api->read(tt, asset_id);
		tm_tt_id_t asset = tm_the_truth_api->get_subobject(tt, entity_asset_read_obj, TM_TT_PROP__ASSET__OBJECT);
		return tm_entity_api->create_entity_from_asset(ctx, asset);
	}

	void register_tm_entity_interface(asIScriptEngine* engine) {
		int r = engine->RegisterObjectType("tm_entity_context_o", sizeof(tm_entity_context_o*), asOBJ_REF | asOBJ_NOCOUNT);  AS_CHECK(r);
		r = engine->RegisterObjectType("tm_component_i", sizeof(tm_component_i), asOBJ_REF | asOBJ_NOCOUNT);  AS_CHECK(r);
		r = engine->RegisterObjectType("tm_entity_commands_o", sizeof(tm_entity_commands_o*), asOBJ_REF | asOBJ_NOCOUNT);  AS_CHECK(r);
		r = engine->RegisterTypedef("tm_entity_t", "uint64");  AS_CHECK(r);

		AS_CHECK(engine->RegisterEnum("tm_entity_create_components")); 
		AS_CHECK(engine->RegisterEnumValue("tm_entity_create_components", "TM_ENTITY_CREATE_COMPONENTS_NONE", TM_ENTITY_CREATE_COMPONENTS_NONE));
		AS_CHECK(engine->RegisterEnumValue("tm_entity_create_components", "TM_ENTITY_CREATE_COMPONENTS_ALL", TM_ENTITY_CREATE_COMPONENTS_ALL));
		AS_CHECK(engine->RegisterEnumValue("tm_entity_create_components", "TM_ENTITY_CREATE_COMPONENTS_EDITOR", TM_ENTITY_CREATE_COMPONENTS_EDITOR));
		
		AS_CHECK(engine->SetDefaultNamespace("tm_entity_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_entity_context_o@ create_context(tm_entity_create_components flags)", asFUNCTION(create_context), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void destroy_context(tm_entity_context_o@ ctx)", asFUNCTION(destroy_context), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_entity_t create_entity(tm_entity_context_o@ ctx)", asFUNCTION(create_entity), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void destroy_entity(tm_entity_context_o@ ctx, tm_entity_t e)", asFUNCTION(destroy_entity), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void clear_world(tm_entity_context_o@ ctx)", asFUNCTION(clear_world), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace("tm_entity_commands_api"));
		AS_CHECK(engine->RegisterGlobalFunction("void destroy_entity(tm_entity_commands_o@ cmds, tm_entity_t e)", asFUNCTION(destroy_entity), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void clear_world(tm_entity_commands_o@ cmds)", asFUNCTION(clear_world_command), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace("tm_entity_ext_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_entity_t create_entity_from_asset(tm_the_truth_o@ tt, tm_tt_id_t asset_root, tm_entity_context_o@ ctx, tm_str_t name)", asFUNCTION(create_entity_from_asset), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}