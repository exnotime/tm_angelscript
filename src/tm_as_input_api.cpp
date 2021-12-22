#include "tm_as_input_api.h"
#include "tm_angelscript.h"
#include <string.h>
#include <angelscript.h>
#include "tm_as_array.h"
#include "tm_as_string.h"
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering input api, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <foundation/log.h>
#include <foundation/input.h>
#include <foundation/string.h>
#include <foundation/allocator.h>
}
using tm_array::tm_script_array_t;

namespace tm_input {

	tm_str_t input_item_name(tm_input_item_t* obj) {
		return tm_string::create_string(obj->name);
	}

	tm_str_t input_source_name(tm_input_source_i* obj) {
		return tm_string::create_string(obj->controller_name);
	}

	tm_script_array_t* input_source_controllers(tm_input_source_i* obj) {
		uint64_t* controllers;
		uint32_t count = obj->controllers(&controllers);

		if (asIScriptContext* ctx = asGetActiveContext()) {
			asITypeInfo* type = ctx->GetEngine()->GetTypeInfoByName("tm_array_t<uint64>");
			tm_script_array_t* output_array = (tm_script_array_t*)ctx->GetEngine()->CreateScriptObject(type);
			for (uint32_t i = 0; i < count; ++i) {
				tm_array::push(&controllers[i], output_array);
			}
			return output_array;
		}

		return nullptr;
	}

	tm_script_array_t* input_source_items(tm_input_source_i* obj) {
		tm_input_item_t* items;
		uint32_t count = obj->items(&items);

		if (asIScriptContext* ctx = asGetActiveContext()) {
			asITypeInfo* type = ctx->GetEngine()->GetTypeInfoByDecl("tm_array_t<tm_input_item_t>");
			tm_script_array_t* output_array = tm_array::create(type, 0, nullptr);
			for (uint32_t i = 0; i < count; ++i) {
				tm_array::push(&items[i], output_array);
			}
			return output_array;
		}

		return nullptr;
	}

	tm_script_array_t* input_source_events(uint64_t start, uint32_t max_count, tm_input_source_i* obj) {
		if (asIScriptContext* ctx = asGetActiveContext()) {
			asITypeInfo* type = ctx->GetEngine()->GetTypeInfoByDecl("tm_array_t<tm_input_event_t>");
			tm_script_array_t* output_array = tm_array::create(type, 0, nullptr);
			tm_input_event_t* events = (tm_input_event_t*)tm_alloc(get_system()->allocator, sizeof(tm_input_event_t) * max_count);
			uint32_t count = (uint32_t)obj->events(start, events, max_count);
			for (uint32_t i = 0; i < count; ++i) {
				tm_array::push(&events[i], output_array);
			}
			tm_free(get_system()->allocator, events, sizeof(tm_input_event_t) * max_count);
			return output_array;
		}
		return nullptr;
	}

	tm_script_array_t* input_sources() {
		tm_input_source_i** sources;
		uint32_t count = tm_input_api->sources(&sources);

		if (asIScriptContext* ctx = asGetActiveContext()) {
			asITypeInfo* type = ctx->GetEngine()->GetTypeInfoByDecl("tm_array_t<tm_input_source_i@>");
			tm_script_array_t* output_array = tm_array::create(type, 0, nullptr);
			for (uint32_t i = 0; i < count; ++i) {
				tm_array::push(&sources[i], output_array);
			}
			return output_array;
		}

		return nullptr;
	}

	tm_script_array_t* input_events(uint64_t start, uint32_t max_count) {
		if (asIScriptContext* ctx = asGetActiveContext()) {
			asITypeInfo* type = ctx->GetEngine()->GetTypeInfoByDecl("tm_array_t<tm_input_event_t>");
			tm_script_array_t* output_array = tm_array::create(type, 0, nullptr);
			tm_input_event_t* events = (tm_input_event_t*)tm_alloc(get_system()->allocator, sizeof(tm_input_event_t) * max_count);
			uint32_t count = (uint32_t)tm_input_api->events(start, events, max_count);
			for (uint32_t i = 0; i < count; ++i) {
				tm_array::push(&events[i], output_array);
			}
			tm_free(get_system()->allocator, events, sizeof(tm_input_event_t) * max_count);
			return output_array;
		}

		return nullptr;
	}

	tm_str_t keyboard_item_names(uint32_t i) {
		return tm_str(tm_input_api->keyboard_item_names()[i]);
	}

	void register_input_interface(asIScriptEngine* engine) {
		AS_CHECK(engine->RegisterObjectType("tm_input_item_t", sizeof(tm_input_item_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_input_item_t>()));
		AS_CHECK(engine->RegisterObjectProperty("tm_input_item_t", "uint64 id", asOFFSET(tm_input_item_t, id)));
		AS_CHECK(engine->RegisterObjectProperty("tm_input_item_t", "uint components", asOFFSET(tm_input_item_t, components)));
		AS_CHECK(engine->RegisterObjectMethod("tm_input_item_t", "tm_str_t name()", asFUNCTION(input_item_name), asCALL_CDECL_OBJLAST));

		AS_CHECK(engine->RegisterObjectType("tm_input_data_t", sizeof(tm_input_data_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_input_data_t>()));
		AS_CHECK(engine->RegisterObjectProperty("tm_input_data_t", "tm_vec4_t f", asOFFSET(tm_input_data_t, f)));
		AS_CHECK(engine->RegisterObjectProperty("tm_input_data_t", "uint codepoint", asOFFSET(tm_input_data_t, codepoint)));

		
		AS_CHECK(engine->RegisterObjectType("tm_input_source_i", sizeof(tm_input_source_i), asOBJ_REF | asOBJ_NOCOUNT));

		//We are currently missing the "extra" data. But since we dont know what it will contain it will need a special function for each type
		AS_CHECK(engine->RegisterObjectType("tm_input_event_t", sizeof(tm_input_event_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_input_event_t>()));
		AS_CHECK(engine->RegisterObjectProperty("tm_input_event_t", "uint64 time", asOFFSET(tm_input_event_t, time)));
		AS_CHECK(engine->RegisterObjectProperty("tm_input_event_t", "uint64 controller_id", asOFFSET(tm_input_event_t, controller_id)));
		AS_CHECK(engine->RegisterObjectProperty("tm_input_event_t", "uint64 item_id", asOFFSET(tm_input_event_t, item_id)));
		AS_CHECK(engine->RegisterObjectProperty("tm_input_event_t", "uint64 type", asOFFSET(tm_input_event_t, type)));
		AS_CHECK(engine->RegisterObjectProperty("tm_input_event_t", "tm_input_data_t data", asOFFSET(tm_input_event_t, data)));
		AS_CHECK(engine->RegisterObjectProperty("tm_input_event_t", "tm_input_source_i@ source", asOFFSET(tm_input_event_t, source)));
		
		AS_CHECK(engine->RegisterObjectProperty("tm_input_source_i", "uint controller_type", asOFFSET(tm_input_source_i, controller_type)));
		AS_CHECK(engine->RegisterObjectMethod("tm_input_source_i", "tm_str_t name()", asFUNCTION(input_source_name), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_input_source_i", "tm_array_t<uint64>@ controllers()", asFUNCTION(input_source_controllers), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_input_source_i", "tm_array_t<tm_input_item_t>@ items()", asFUNCTION(input_source_items), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_input_source_i", "tm_array_t<tm_input_event_t>@ events(uint64 start, uint max_count = 8)", asFUNCTION(input_source_events), asCALL_CDECL_OBJLAST));
		

		AS_CHECK(engine->RegisterEnum("tm_input_controller_type"));
		AS_CHECK(engine->RegisterEnumValue("tm_input_controller_type", "TM_INPUT_CONTROLLER_TYPE_NONE", TM_INPUT_CONTROLLER_TYPE_NONE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_controller_type", "TM_INPUT_CONTROLLER_TYPE_KEYBOARD", TM_INPUT_CONTROLLER_TYPE_KEYBOARD));
		AS_CHECK(engine->RegisterEnumValue("tm_input_controller_type", "TM_INPUT_CONTROLLER_TYPE_MOUSE", TM_INPUT_CONTROLLER_TYPE_MOUSE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_controller_type", "TM_INPUT_CONTROLLER_TYPE_GAMEPAD", TM_INPUT_CONTROLLER_TYPE_GAMEPAD));
		AS_CHECK(engine->RegisterEnumValue("tm_input_controller_type", "TM_INPUT_CONTROLLER_TYPE_TOUCH", TM_INPUT_CONTROLLER_TYPE_TOUCH));
		AS_CHECK(engine->RegisterEnumValue("tm_input_controller_type", "TM_INPUT_CONTROLLER_TYPE_PEN", TM_INPUT_CONTROLLER_TYPE_PEN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_controller_type", "TM_INPUT_CONTROLLER_TYPE_OTHER", TM_INPUT_CONTROLLER_TYPE_OTHER));

		AS_CHECK(engine->RegisterEnum("tm_input_gamepad_item"));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_NONE", TM_INPUT_GAMEPAD_ITEM_NONE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_BUTTON_A", TM_INPUT_GAMEPAD_ITEM_BUTTON_A));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_BUTTON_B", TM_INPUT_GAMEPAD_ITEM_BUTTON_B));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_BUTTON_X", TM_INPUT_GAMEPAD_ITEM_BUTTON_X));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_BUTTON_Y", TM_INPUT_GAMEPAD_ITEM_BUTTON_Y));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_BUTTON_LEFT_SHOULDER", TM_INPUT_GAMEPAD_ITEM_BUTTON_LEFT_SHOULDER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_BUTTON_RIGHT_SHOULDER", TM_INPUT_GAMEPAD_ITEM_BUTTON_RIGHT_SHOULDER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_BUTTON_BACK", TM_INPUT_GAMEPAD_ITEM_BUTTON_BACK));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_BUTTON_START", TM_INPUT_GAMEPAD_ITEM_BUTTON_START));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_DPAD_LEFT", TM_INPUT_GAMEPAD_ITEM_DPAD_LEFT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_DPAD_RIGHT", TM_INPUT_GAMEPAD_ITEM_DPAD_RIGHT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_DPAD_UP", TM_INPUT_GAMEPAD_ITEM_DPAD_UP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_DPAD_DOWN", TM_INPUT_GAMEPAD_ITEM_DPAD_DOWN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_BUTTON_LEFT_THUMB", TM_INPUT_GAMEPAD_BUTTON_LEFT_THUMB));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_BUTTON_RIGHT_THUMB", TM_INPUT_GAMEPAD_BUTTON_RIGHT_THUMB));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_BUTTON_LEFT_TRIGGER", TM_INPUT_GAMEPAD_ITEM_BUTTON_LEFT_TRIGGER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_BUTTON_RIGHT_TRIGGER", TM_INPUT_GAMEPAD_ITEM_BUTTON_RIGHT_TRIGGER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_LEFT_STICK", TM_INPUT_GAMEPAD_ITEM_LEFT_STICK));
		AS_CHECK(engine->RegisterEnumValue("tm_input_gamepad_item", "TM_INPUT_GAMEPAD_ITEM_RIGHT_STICK", TM_INPUT_GAMEPAD_ITEM_RIGHT_STICK));

		AS_CHECK(engine->RegisterEnum("tm_input_event_type"));
		AS_CHECK(engine->RegisterEnumValue("tm_input_event_type", "TM_INPUT_EVENT_TYPE_NONE", TM_INPUT_EVENT_TYPE_NONE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_event_type", "TM_INPUT_EVENT_TYPE_DATA_CHANGE", TM_INPUT_EVENT_TYPE_DATA_CHANGE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_event_type", "TM_INPUT_EVENT_TYPE_TEXT", TM_INPUT_EVENT_TYPE_TEXT));


		AS_CHECK(engine->SetDefaultNamespace("tm_input_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_array_t<tm_input_source_i@>@ sources()", asFUNCTION(input_sources), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_array_t<tm_input_event_t>@ events(uint64 start, uint max_count = 8)", asFUNCTION(input_events), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_str_t keyboard_item_names(uint k)", asFUNCTION(keyboard_item_names), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}