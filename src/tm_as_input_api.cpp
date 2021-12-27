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

	tm_input_data_t input_source_state(uint64_t controller, uint64_t item, tm_input_source_i* obj) {
		return obj->state(controller, item);
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
		AS_CHECK(engine->RegisterObjectMethod("tm_input_source_i", "tm_input_data_t state(uint64 controller, uint64 item)", asFUNCTION(input_source_state), asCALL_CDECL_OBJLAST));

		AS_CHECK(engine->RegisterEnum("tm_input_event_type"));
		AS_CHECK(engine->RegisterEnumValue("tm_input_event_type", "TM_INPUT_EVENT_TYPE_NONE", TM_INPUT_EVENT_TYPE_NONE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_event_type", "TM_INPUT_EVENT_TYPE_DATA_CHANGE", TM_INPUT_EVENT_TYPE_DATA_CHANGE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_event_type", "TM_INPUT_EVENT_TYPE_TEXT", TM_INPUT_EVENT_TYPE_TEXT));


		AS_CHECK(engine->SetDefaultNamespace("tm_input_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_array_t<tm_input_source_i@>@ sources()", asFUNCTION(input_sources), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_array_t<tm_input_event_t>@ events(uint64 start, uint max_count = 8)", asFUNCTION(input_events), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_str_t keyboard_item_names(uint k)", asFUNCTION(keyboard_item_names), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));

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

		AS_CHECK(engine->RegisterEnum("tm_input_mouse_item"));
		AS_CHECK(engine->RegisterEnumValue("tm_input_mouse_item", "TM_INPUT_MOUSE_ITEM_NONE", TM_INPUT_MOUSE_ITEM_NONE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_mouse_item", "TM_INPUT_MOUSE_ITEM_BUTTON_LEFT", TM_INPUT_MOUSE_ITEM_BUTTON_LEFT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_mouse_item", "TM_INPUT_MOUSE_ITEM_BUTTON_RIGHT", TM_INPUT_MOUSE_ITEM_BUTTON_RIGHT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_mouse_item", "TM_INPUT_MOUSE_ITEM_BUTTON_MIDDLE", TM_INPUT_MOUSE_ITEM_BUTTON_MIDDLE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_mouse_item", "TM_INPUT_MOUSE_ITEM_BUTTON_4", TM_INPUT_MOUSE_ITEM_BUTTON_4));
		AS_CHECK(engine->RegisterEnumValue("tm_input_mouse_item", "TM_INPUT_MOUSE_ITEM_BUTTON_5", TM_INPUT_MOUSE_ITEM_BUTTON_5));
		AS_CHECK(engine->RegisterEnumValue("tm_input_mouse_item", "TM_INPUT_MOUSE_ITEM_WHEEL", TM_INPUT_MOUSE_ITEM_WHEEL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_mouse_item", "TM_INPUT_MOUSE_ITEM_MOVE", TM_INPUT_MOUSE_ITEM_MOVE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_mouse_item", "TM_INPUT_MOUSE_ITEM_POSITION", TM_INPUT_MOUSE_ITEM_POSITION));

		AS_CHECK(engine->RegisterEnum("tm_input_keyboard_item"));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NONE", TM_INPUT_KEYBOARD_ITEM_NONE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LBUTTON", TM_INPUT_KEYBOARD_ITEM_LBUTTON));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_RBUTTON", TM_INPUT_KEYBOARD_ITEM_RBUTTON));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_CANCEL", TM_INPUT_KEYBOARD_ITEM_CANCEL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MBUTTON", TM_INPUT_KEYBOARD_ITEM_MBUTTON));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_XBUTTON1", TM_INPUT_KEYBOARD_ITEM_XBUTTON1));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_XBUTTON2", TM_INPUT_KEYBOARD_ITEM_XBUTTON2));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BACKSPACE", TM_INPUT_KEYBOARD_ITEM_BACKSPACE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_TAB", TM_INPUT_KEYBOARD_ITEM_TAB));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_CLEAR", TM_INPUT_KEYBOARD_ITEM_CLEAR));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_ENTER", TM_INPUT_KEYBOARD_ITEM_ENTER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_SHIFT", TM_INPUT_KEYBOARD_ITEM_SHIFT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_CONTROL", TM_INPUT_KEYBOARD_ITEM_CONTROL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MENU", TM_INPUT_KEYBOARD_ITEM_MENU));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PAUSE", TM_INPUT_KEYBOARD_ITEM_PAUSE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_CAPSLOCK", TM_INPUT_KEYBOARD_ITEM_CAPSLOCK));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_KANA", TM_INPUT_KEYBOARD_ITEM_KANA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_JUNJA", TM_INPUT_KEYBOARD_ITEM_JUNJA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_FINAL", TM_INPUT_KEYBOARD_ITEM_FINAL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_HANJA", TM_INPUT_KEYBOARD_ITEM_HANJA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_ESCAPE", TM_INPUT_KEYBOARD_ITEM_ESCAPE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_CONVERT", TM_INPUT_KEYBOARD_ITEM_CONVERT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NONCONVERT", TM_INPUT_KEYBOARD_ITEM_NONCONVERT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_ACCEPT", TM_INPUT_KEYBOARD_ITEM_ACCEPT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MODECHANGE", TM_INPUT_KEYBOARD_ITEM_MODECHANGE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_SPACE", TM_INPUT_KEYBOARD_ITEM_SPACE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PAGEUP", TM_INPUT_KEYBOARD_ITEM_PAGEUP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PAGEDOWN", TM_INPUT_KEYBOARD_ITEM_PAGEDOWN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_END", TM_INPUT_KEYBOARD_ITEM_END));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_HOME", TM_INPUT_KEYBOARD_ITEM_HOME));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LEFT", TM_INPUT_KEYBOARD_ITEM_LEFT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_UP", TM_INPUT_KEYBOARD_ITEM_UP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_RIGHT", TM_INPUT_KEYBOARD_ITEM_RIGHT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_DOWN", TM_INPUT_KEYBOARD_ITEM_DOWN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_SELECT", TM_INPUT_KEYBOARD_ITEM_SELECT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PRINT", TM_INPUT_KEYBOARD_ITEM_PRINT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_EXECUTE", TM_INPUT_KEYBOARD_ITEM_EXECUTE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PRINTSCREEN", TM_INPUT_KEYBOARD_ITEM_PRINTSCREEN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_INSERT", TM_INPUT_KEYBOARD_ITEM_INSERT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_DELETE", TM_INPUT_KEYBOARD_ITEM_DELETE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_HELP", TM_INPUT_KEYBOARD_ITEM_HELP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_0", TM_INPUT_KEYBOARD_ITEM_0));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_1", TM_INPUT_KEYBOARD_ITEM_1));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_2", TM_INPUT_KEYBOARD_ITEM_2));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_3", TM_INPUT_KEYBOARD_ITEM_3));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_4", TM_INPUT_KEYBOARD_ITEM_4));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_5", TM_INPUT_KEYBOARD_ITEM_5));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_6", TM_INPUT_KEYBOARD_ITEM_6));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_7", TM_INPUT_KEYBOARD_ITEM_7));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_8", TM_INPUT_KEYBOARD_ITEM_8));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_9", TM_INPUT_KEYBOARD_ITEM_9));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_A", TM_INPUT_KEYBOARD_ITEM_A));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_B", TM_INPUT_KEYBOARD_ITEM_B));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_C", TM_INPUT_KEYBOARD_ITEM_C));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_D", TM_INPUT_KEYBOARD_ITEM_D));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_E", TM_INPUT_KEYBOARD_ITEM_E));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F", TM_INPUT_KEYBOARD_ITEM_F));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_G", TM_INPUT_KEYBOARD_ITEM_G));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_H", TM_INPUT_KEYBOARD_ITEM_H));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_I", TM_INPUT_KEYBOARD_ITEM_I));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_J", TM_INPUT_KEYBOARD_ITEM_J));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_K", TM_INPUT_KEYBOARD_ITEM_K));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_L", TM_INPUT_KEYBOARD_ITEM_L));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_M", TM_INPUT_KEYBOARD_ITEM_M));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_N", TM_INPUT_KEYBOARD_ITEM_N));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_O", TM_INPUT_KEYBOARD_ITEM_O));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_P", TM_INPUT_KEYBOARD_ITEM_P));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_Q", TM_INPUT_KEYBOARD_ITEM_Q));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_R", TM_INPUT_KEYBOARD_ITEM_R));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_S", TM_INPUT_KEYBOARD_ITEM_S));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_T", TM_INPUT_KEYBOARD_ITEM_T));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_U", TM_INPUT_KEYBOARD_ITEM_U));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_V", TM_INPUT_KEYBOARD_ITEM_V));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_W", TM_INPUT_KEYBOARD_ITEM_W));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_X", TM_INPUT_KEYBOARD_ITEM_X));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_Y", TM_INPUT_KEYBOARD_ITEM_Y));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_Z", TM_INPUT_KEYBOARD_ITEM_Z));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LWIN", TM_INPUT_KEYBOARD_ITEM_LWIN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_RWIN", TM_INPUT_KEYBOARD_ITEM_RWIN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_APPS", TM_INPUT_KEYBOARD_ITEM_APPS));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_SLEEP", TM_INPUT_KEYBOARD_ITEM_SLEEP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPAD0", TM_INPUT_KEYBOARD_ITEM_NUMPAD0));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPAD1", TM_INPUT_KEYBOARD_ITEM_NUMPAD1));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPAD2", TM_INPUT_KEYBOARD_ITEM_NUMPAD2));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPAD3", TM_INPUT_KEYBOARD_ITEM_NUMPAD3));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPAD4", TM_INPUT_KEYBOARD_ITEM_NUMPAD4));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPAD5", TM_INPUT_KEYBOARD_ITEM_NUMPAD5));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPAD6", TM_INPUT_KEYBOARD_ITEM_NUMPAD6));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPAD7", TM_INPUT_KEYBOARD_ITEM_NUMPAD7));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPAD8", TM_INPUT_KEYBOARD_ITEM_NUMPAD8));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPAD9", TM_INPUT_KEYBOARD_ITEM_NUMPAD9));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADASTERISK", TM_INPUT_KEYBOARD_ITEM_NUMPADASTERISK));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADPLUS", TM_INPUT_KEYBOARD_ITEM_NUMPADPLUS));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADENTER", TM_INPUT_KEYBOARD_ITEM_NUMPADENTER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADMINUS", TM_INPUT_KEYBOARD_ITEM_NUMPADMINUS));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADDOT", TM_INPUT_KEYBOARD_ITEM_NUMPADDOT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADSLASH", TM_INPUT_KEYBOARD_ITEM_NUMPADSLASH));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F1", TM_INPUT_KEYBOARD_ITEM_F1));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F2", TM_INPUT_KEYBOARD_ITEM_F2));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F3", TM_INPUT_KEYBOARD_ITEM_F3));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F4", TM_INPUT_KEYBOARD_ITEM_F4));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F5", TM_INPUT_KEYBOARD_ITEM_F5));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F6", TM_INPUT_KEYBOARD_ITEM_F6));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F7", TM_INPUT_KEYBOARD_ITEM_F7));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F8", TM_INPUT_KEYBOARD_ITEM_F8));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F9", TM_INPUT_KEYBOARD_ITEM_F9));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F10", TM_INPUT_KEYBOARD_ITEM_F10));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F11", TM_INPUT_KEYBOARD_ITEM_F11));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F12", TM_INPUT_KEYBOARD_ITEM_F12));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F13", TM_INPUT_KEYBOARD_ITEM_F13));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F14", TM_INPUT_KEYBOARD_ITEM_F14));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F15", TM_INPUT_KEYBOARD_ITEM_F15));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F16", TM_INPUT_KEYBOARD_ITEM_F16));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F17", TM_INPUT_KEYBOARD_ITEM_F17));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F18", TM_INPUT_KEYBOARD_ITEM_F18));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F19", TM_INPUT_KEYBOARD_ITEM_F19));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F20", TM_INPUT_KEYBOARD_ITEM_F20));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F21", TM_INPUT_KEYBOARD_ITEM_F21));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F22", TM_INPUT_KEYBOARD_ITEM_F22));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F23", TM_INPUT_KEYBOARD_ITEM_F23));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_F24", TM_INPUT_KEYBOARD_ITEM_F24));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NAVIGATION_VIEW", TM_INPUT_KEYBOARD_ITEM_NAVIGATION_VIEW));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NAVIGATION_MENU", TM_INPUT_KEYBOARD_ITEM_NAVIGATION_MENU));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NAVIGATION_UP", TM_INPUT_KEYBOARD_ITEM_NAVIGATION_UP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NAVIGATION_DOWN", TM_INPUT_KEYBOARD_ITEM_NAVIGATION_DOWN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NAVIGATION_LEFT", TM_INPUT_KEYBOARD_ITEM_NAVIGATION_LEFT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NAVIGATION_RIGHT", TM_INPUT_KEYBOARD_ITEM_NAVIGATION_RIGHT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NAVIGATION_ACCEPT", TM_INPUT_KEYBOARD_ITEM_NAVIGATION_ACCEPT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NAVIGATION_CANCEL", TM_INPUT_KEYBOARD_ITEM_NAVIGATION_CANCEL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMLOCK", TM_INPUT_KEYBOARD_ITEM_NUMLOCK));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_SCROLLLOCK", TM_INPUT_KEYBOARD_ITEM_SCROLLLOCK));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADEQUAL", TM_INPUT_KEYBOARD_ITEM_NUMPADEQUAL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_FJ_MASSHOU", TM_INPUT_KEYBOARD_ITEM_OEM_FJ_MASSHOU));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_FJ_TOUROKU", TM_INPUT_KEYBOARD_ITEM_OEM_FJ_TOUROKU));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_FJ_LOYA", TM_INPUT_KEYBOARD_ITEM_OEM_FJ_LOYA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_FJ_ROYA", TM_INPUT_KEYBOARD_ITEM_OEM_FJ_ROYA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LEFTSHIFT", TM_INPUT_KEYBOARD_ITEM_LEFTSHIFT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_RIGHTSHIFT", TM_INPUT_KEYBOARD_ITEM_RIGHTSHIFT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LEFTCONTROL", TM_INPUT_KEYBOARD_ITEM_LEFTCONTROL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_RIGHTCONTROL", TM_INPUT_KEYBOARD_ITEM_RIGHTCONTROL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LEFTALT", TM_INPUT_KEYBOARD_ITEM_LEFTALT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_RIGHTALT", TM_INPUT_KEYBOARD_ITEM_RIGHTALT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_BACK", TM_INPUT_KEYBOARD_ITEM_BROWSER_BACK));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_FORWARD", TM_INPUT_KEYBOARD_ITEM_BROWSER_FORWARD));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_REFRESH", TM_INPUT_KEYBOARD_ITEM_BROWSER_REFRESH));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_STOP", TM_INPUT_KEYBOARD_ITEM_BROWSER_STOP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_SEARCH", TM_INPUT_KEYBOARD_ITEM_BROWSER_SEARCH));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_FAVORITES", TM_INPUT_KEYBOARD_ITEM_BROWSER_FAVORITES));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_HOME", TM_INPUT_KEYBOARD_ITEM_BROWSER_HOME));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_VOLUME_MUTE", TM_INPUT_KEYBOARD_ITEM_VOLUME_MUTE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_VOLUME_DOWN", TM_INPUT_KEYBOARD_ITEM_VOLUME_DOWN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_VOLUME_UP", TM_INPUT_KEYBOARD_ITEM_VOLUME_UP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MEDIA_NEXT_TRACK", TM_INPUT_KEYBOARD_ITEM_MEDIA_NEXT_TRACK));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MEDIA_PREV_TRACK", TM_INPUT_KEYBOARD_ITEM_MEDIA_PREV_TRACK));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MEDIA_STOP", TM_INPUT_KEYBOARD_ITEM_MEDIA_STOP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MEDIA_PLAY_PAUSE", TM_INPUT_KEYBOARD_ITEM_MEDIA_PLAY_PAUSE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LAUNCH_MAIL", TM_INPUT_KEYBOARD_ITEM_LAUNCH_MAIL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LAUNCH_MEDIA_SELECT", TM_INPUT_KEYBOARD_ITEM_LAUNCH_MEDIA_SELECT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LAUNCH_APP1", TM_INPUT_KEYBOARD_ITEM_LAUNCH_APP1));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LAUNCH_APP2", TM_INPUT_KEYBOARD_ITEM_LAUNCH_APP2));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_SEMICOLON", TM_INPUT_KEYBOARD_ITEM_SEMICOLON));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_EQUAL", TM_INPUT_KEYBOARD_ITEM_EQUAL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_COMMA", TM_INPUT_KEYBOARD_ITEM_COMMA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MINUS", TM_INPUT_KEYBOARD_ITEM_MINUS));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_DOT", TM_INPUT_KEYBOARD_ITEM_DOT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_SLASH", TM_INPUT_KEYBOARD_ITEM_SLASH));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GRAVE", TM_INPUT_KEYBOARD_ITEM_GRAVE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_A", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_A));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_B", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_B));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_X", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_X));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_Y", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_Y));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_SHOULDER", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_SHOULDER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_SHOULDER", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_SHOULDER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_TRIGGER", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_TRIGGER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_TRIGGER", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_TRIGGER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_DPAD_UP", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_DPAD_UP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_DPAD_DOWN", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_DPAD_DOWN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_DPAD_LEFT", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_DPAD_LEFT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_DPAD_RIGHT", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_DPAD_RIGHT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_MENU", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_MENU));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_VIEW", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_VIEW));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_THUMBSTICK_BUTTON", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_THUMBSTICK_BUTTON));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_THUMBSTICK_BUTTON", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_THUMBSTICK_BUTTON));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_THUMBSTICK_UP", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_THUMBSTICK_UP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_THUMBSTICK_DOWN", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_THUMBSTICK_DOWN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_THUMBSTICK_RIGHT", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_THUMBSTICK_RIGHT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_THUMBSTICK_LEFT", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_LEFT_THUMBSTICK_LEFT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_THUMBSTICK_UP", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_THUMBSTICK_UP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_THUMBSTICK_DOWN", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_THUMBSTICK_DOWN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_THUMBSTICK_RIGHT", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_THUMBSTICK_RIGHT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_THUMBSTICK_LEFT", TM_INPUT_KEYBOARD_ITEM_GAMEPAD_RIGHT_THUMBSTICK_LEFT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LEFTBRACE", TM_INPUT_KEYBOARD_ITEM_LEFTBRACE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BACKSLASH", TM_INPUT_KEYBOARD_ITEM_BACKSLASH));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_RIGHTBRACE", TM_INPUT_KEYBOARD_ITEM_RIGHTBRACE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_APOSTROPHE", TM_INPUT_KEYBOARD_ITEM_APOSTROPHE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_8", TM_INPUT_KEYBOARD_ITEM_OEM_8));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_AX", TM_INPUT_KEYBOARD_ITEM_OEM_AX));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_102", TM_INPUT_KEYBOARD_ITEM_OEM_102));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_ICO_HELP", TM_INPUT_KEYBOARD_ITEM_ICO_HELP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_ICO_00", TM_INPUT_KEYBOARD_ITEM_ICO_00));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PROCESSKEY", TM_INPUT_KEYBOARD_ITEM_PROCESSKEY));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_ICO_CLEAR", TM_INPUT_KEYBOARD_ITEM_ICO_CLEAR));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PACKET", TM_INPUT_KEYBOARD_ITEM_PACKET));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_RESET", TM_INPUT_KEYBOARD_ITEM_OEM_RESET));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_JUMP", TM_INPUT_KEYBOARD_ITEM_OEM_JUMP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_PA1", TM_INPUT_KEYBOARD_ITEM_OEM_PA1));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_PA2", TM_INPUT_KEYBOARD_ITEM_OEM_PA2));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_PA3", TM_INPUT_KEYBOARD_ITEM_OEM_PA3));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_WSCTRL", TM_INPUT_KEYBOARD_ITEM_OEM_WSCTRL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_CUSEL", TM_INPUT_KEYBOARD_ITEM_OEM_CUSEL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_ATTN", TM_INPUT_KEYBOARD_ITEM_OEM_ATTN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_FINISH", TM_INPUT_KEYBOARD_ITEM_OEM_FINISH));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_COPY", TM_INPUT_KEYBOARD_ITEM_COPY));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_AUTO", TM_INPUT_KEYBOARD_ITEM_OEM_AUTO));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_ENLW", TM_INPUT_KEYBOARD_ITEM_OEM_ENLW));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_BACKTAB", TM_INPUT_KEYBOARD_ITEM_OEM_BACKTAB));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_ATTN", TM_INPUT_KEYBOARD_ITEM_ATTN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_CRSEL", TM_INPUT_KEYBOARD_ITEM_CRSEL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_EXSEL", TM_INPUT_KEYBOARD_ITEM_EXSEL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_EREOF", TM_INPUT_KEYBOARD_ITEM_EREOF));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PLAY", TM_INPUT_KEYBOARD_ITEM_PLAY));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_ZOOM", TM_INPUT_KEYBOARD_ITEM_ZOOM));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NONAME", TM_INPUT_KEYBOARD_ITEM_NONAME));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PA1", TM_INPUT_KEYBOARD_ITEM_PA1));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_CLEAR", TM_INPUT_KEYBOARD_ITEM_OEM_CLEAR));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_KANJI", TM_INPUT_KEYBOARD_ITEM_KANJI));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OEM_FJ_JISHO", TM_INPUT_KEYBOARD_ITEM_OEM_FJ_JISHO));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_HASHTILDE", TM_INPUT_KEYBOARD_ITEM_HASHTILDE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_102ND", TM_INPUT_KEYBOARD_ITEM_102ND));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_COMPOSE", TM_INPUT_KEYBOARD_ITEM_COMPOSE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_POWER", TM_INPUT_KEYBOARD_ITEM_POWER));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_OPEN", TM_INPUT_KEYBOARD_ITEM_OPEN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PROPS", TM_INPUT_KEYBOARD_ITEM_PROPS));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_FRONT", TM_INPUT_KEYBOARD_ITEM_FRONT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_STOP", TM_INPUT_KEYBOARD_ITEM_STOP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_AGAIN", TM_INPUT_KEYBOARD_ITEM_AGAIN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_UNDO", TM_INPUT_KEYBOARD_ITEM_UNDO));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_CUT", TM_INPUT_KEYBOARD_ITEM_CUT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_PASTE", TM_INPUT_KEYBOARD_ITEM_PASTE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_FIND", TM_INPUT_KEYBOARD_ITEM_FIND));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADCOMMA", TM_INPUT_KEYBOARD_ITEM_NUMPADCOMMA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_RO", TM_INPUT_KEYBOARD_ITEM_RO));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_KATAKANAHIRAGANA", TM_INPUT_KEYBOARD_ITEM_KATAKANAHIRAGANA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_YEN", TM_INPUT_KEYBOARD_ITEM_YEN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_HENKAN", TM_INPUT_KEYBOARD_ITEM_HENKAN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MUHENKAN", TM_INPUT_KEYBOARD_ITEM_MUHENKAN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADJPCOMMA", TM_INPUT_KEYBOARD_ITEM_NUMPADJPCOMMA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_INTERNATIONAL_7", TM_INPUT_KEYBOARD_ITEM_INTERNATIONAL_7));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_INTERNATIONAL_8", TM_INPUT_KEYBOARD_ITEM_INTERNATIONAL_8));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_INTERNATIONAL_9", TM_INPUT_KEYBOARD_ITEM_INTERNATIONAL_9));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_HANGEUL", TM_INPUT_KEYBOARD_ITEM_HANGEUL));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_KATAKANA", TM_INPUT_KEYBOARD_ITEM_KATAKANA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_HIRAGANA", TM_INPUT_KEYBOARD_ITEM_HIRAGANA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_ZENKAKUHANKAKU", TM_INPUT_KEYBOARD_ITEM_ZENKAKUHANKAKU));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LANG_6", TM_INPUT_KEYBOARD_ITEM_LANG_6));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LANG_7", TM_INPUT_KEYBOARD_ITEM_LANG_7));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LANG_8", TM_INPUT_KEYBOARD_ITEM_LANG_8));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LANG_9", TM_INPUT_KEYBOARD_ITEM_LANG_9));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADLEFTPAREN", TM_INPUT_KEYBOARD_ITEM_NUMPADLEFTPAREN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_NUMPADRIGHTPAREN", TM_INPUT_KEYBOARD_ITEM_NUMPADRIGHTPAREN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_LEFTMETA", TM_INPUT_KEYBOARD_ITEM_LEFTMETA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_RIGHTMETA", TM_INPUT_KEYBOARD_ITEM_RIGHTMETA));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MEDIA_EJECT", TM_INPUT_KEYBOARD_ITEM_MEDIA_EJECT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MEDIA_VOLUME_UP", TM_INPUT_KEYBOARD_ITEM_MEDIA_VOLUME_UP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MEDIA_VOLUME_DOWN", TM_INPUT_KEYBOARD_ITEM_MEDIA_VOLUME_DOWN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_MEDIA_MUTE", TM_INPUT_KEYBOARD_ITEM_MEDIA_MUTE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_WWW", TM_INPUT_KEYBOARD_ITEM_BROWSER_WWW));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_SCROLLUP", TM_INPUT_KEYBOARD_ITEM_BROWSER_SCROLLUP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_SCROLLDOWN", TM_INPUT_KEYBOARD_ITEM_BROWSER_SCROLLDOWN));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_EDIT", TM_INPUT_KEYBOARD_ITEM_BROWSER_EDIT));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_SLEEP", TM_INPUT_KEYBOARD_ITEM_BROWSER_SLEEP));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_COFFEE", TM_INPUT_KEYBOARD_ITEM_BROWSER_COFFEE));
		AS_CHECK(engine->RegisterEnumValue("tm_input_keyboard_item", "TM_INPUT_KEYBOARD_ITEM_BROWSER_CALC", TM_INPUT_KEYBOARD_ITEM_BROWSER_CALC));

		
	}
}