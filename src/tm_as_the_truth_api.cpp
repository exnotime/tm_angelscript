#include "tm_as_entity_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include <new>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering the truth Angelscript, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <plugins/entity/entity.h>
#include <foundation/log.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_assets.h>
}
//Combined the_truth and the_truth_assets api.
//Right now this is focused on getting the functionaility needed to create entities
namespace tm_the_truth {

	//The Truth
	const tm_the_truth_object_o* read(tm_the_truth_o* tt, tm_tt_id_t object) {
		return tm_the_truth_api->read(tt, object);
	}

	tm_tt_id_t get_subobject(tm_the_truth_o* tt, const tm_the_truth_object_o* object, uint32_t property) {
		return tm_the_truth_api->get_subobject(tt, object, property);
	}

	//The Truth Assets
	tm_tt_id_t asset_from_path(tm_the_truth_o* tt, tm_tt_id_t asset_root, tm_str_t& name) {
		return tm_the_truth_assets_api->asset_from_path(tt, asset_root, name.data);
	}

	void register_the_truth_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {

		//Object declarations TODO: Move to separate file to sort out init order
		AS_CHECK(engine->RegisterObjectType("tm_the_truth_o", sizeof(tm_the_truth_o*), asOBJ_REF | asOBJ_NOCOUNT));
		AS_CHECK(engine->RegisterObjectType("tm_the_truth_object_o", sizeof(tm_the_truth_object_o*), asOBJ_REF | asOBJ_NOCOUNT));

		//The Truth
		AS_CHECK(engine->SetDefaultNamespace("tm_the_truth_api"));
		AS_CHECK(engine->RegisterGlobalFunction("const tm_the_truth_object_o@ read(tm_the_truth_o@ tt, tm_tt_id_t object)", asFUNCTION(read), asCALL_CDECL));
		//The Truth Assets
		AS_CHECK(engine->SetDefaultNamespace("tm_the_truth_assets_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_tt_id_t asset_from_path(tm_the_truth_o@ tt, tm_tt_id_t asset_root, tm_str_t name)", asFUNCTION(asset_from_path), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}