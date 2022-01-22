#include "tm_as_entity_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering ui, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <foundation/log.h>
#include <plugins/ui/ui.h>
#include <plugins/ui/ui_custom.h>
#include <plugins/ui/draw2d.h>
}

//Since the plugin is focused on gameplay scripting we will only expose things that "draw" to the screen without taking any input.
namespace tm_ui {

	tm_rect_t text(tm_ui_o* ui, const tm_ui_style_t* style, const tm_str_t string, const tm_rect_t rect, const tm_color_srgb_t color) {
		if (ui == nullptr || style == nullptr)
			return rect;

		tm_ui_text_t t = {};
		t.text = string.data;
		t.color = &color;
		t.rect = rect;
		return tm_ui_api->text(ui, style, &t);
	}

	void register_ui_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {
	
		AS_CHECK(engine->SetDefaultNamespace("tm_ui_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_rect_t text(tm_ui_o@ ui, const tm_ui_style_t@ style, const tm_str_t string, const tm_rect_t rect, const tm_color_srgb_t color)", asFUNCTION(text), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}