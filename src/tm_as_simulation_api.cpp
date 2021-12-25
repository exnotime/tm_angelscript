#include "tm_as_entity_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering simulation, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <foundation/log.h>
#include <plugins/simulation/simulation.h>
#include <plugins/simulation/simulation_entry.h>
}

namespace tm_simulation {


	tm_simulation_o* single_simulation() {
		tm_simulation_o* sim = tm_simulation_api->single_simulation();
		return sim;
	}

	tm_entity_t camera(tm_simulation_o* sim) {
		return tm_simulation_api->camera(sim);
	}

	void set_camera(tm_simulation_o* sim, tm_entity_t camera) {
		tm_simulation_api->set_camera(sim, camera);
	}

	void register_simulation_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {
		AS_CHECK(engine->RegisterObjectType("tm_simulation_o", sizeof(tm_simulation_o*), asOBJ_REF | asOBJ_NOCOUNT));
		AS_CHECK(engine->RegisterObjectType("tm_simulation_frame_args_t", sizeof(tm_simulation_frame_args_t*), asOBJ_REF | asOBJ_NOCOUNT));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_frame_args_t", "float dt", asOFFSET(tm_simulation_frame_args_t, dt)));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_frame_args_t", "float dt_unscaled", asOFFSET(tm_simulation_frame_args_t, dt_unscaled)));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_frame_args_t", "double time", asOFFSET(tm_simulation_frame_args_t, time)));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_frame_args_t", "double time_unscaled", asOFFSET(tm_simulation_frame_args_t, time_unscaled)));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_frame_args_t", "bool running_in_editor", asOFFSET(tm_simulation_frame_args_t, running_in_editor)));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_frame_args_t", "tm_entity_commands_o@ commands", asOFFSET(tm_simulation_frame_args_t, commands)));

		AS_CHECK(engine->RegisterObjectType("tm_simulation_start_args_t", sizeof(tm_simulation_start_args_t), asOBJ_REF | asOBJ_NOCOUNT));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_start_args_t", "tm_the_truth_o@ tt", asOFFSET(tm_simulation_start_args_t, tt)));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_start_args_t", "tm_entity_context_o@ entity_ctx", asOFFSET(tm_simulation_start_args_t, entity_ctx)));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_start_args_t", "tm_simulation_o@ simulation_ctx", asOFFSET(tm_simulation_start_args_t, simulation_ctx)));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_start_args_t", " tm_tt_id_t asset_root", asOFFSET(tm_simulation_start_args_t, asset_root)));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_start_args_t", " tm_tt_id_t settings_id", asOFFSET(tm_simulation_start_args_t, settings_id)));
		AS_CHECK(engine->RegisterObjectProperty("tm_simulation_start_args_t", " bool running_in_editor", asOFFSET(tm_simulation_start_args_t, running_in_editor)));
	
		AS_CHECK(engine->SetDefaultNamespace("tm_simulation_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_simulation_o@ single_simulation()", asFUNCTION(single_simulation), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_entity_t camera(tm_simulation_o@ sim)", asFUNCTION(camera), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_camera(tm_simulation_o@ sim, tm_entity_t camera)", asFUNCTION(set_camera), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}