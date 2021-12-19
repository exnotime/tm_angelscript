#include "tm_as_component_api.h"
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
}

namespace tm_component {


	void register_component_interface(asIScriptEngine* engine) {

	}
}