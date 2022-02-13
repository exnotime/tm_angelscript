#include "tm_as_entity_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering ui, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <foundation/allocator.h>
#include <foundation/log.h>
#include <plugins/ui/ui.h>
#include <plugins/ui/ui_custom.h>
#include <plugins/ui/draw2d.h>
}

//Since the plugin is focused on gameplay scripting we will only expose things that "draw" to the screen without taking any input.
namespace tm_ui {
	tm_allocator_i* _allocator;

	struct tm_draw2d_context {
		tm_draw2d_vbuffer_t* vbuffer;
		tm_draw2d_ibuffer_t** ibuffer;
		tm_draw2d_style_t* style;
	};

	tm_rect_t text(tm_ui_o* ui, const tm_ui_style_t* style, const tm_str_t string, const tm_rect_t rect, const tm_color_srgb_t color) {
		if (ui == nullptr || style == nullptr)
			return rect;

		tm_ui_text_t t = {};
		t.text = string.data;
		t.color = &color;
		t.rect = rect;
		return tm_ui_api->text(ui, style, &t);
	}

	tm_draw2d_context* create_draw_context(tm_ui_o* ui, const tm_ui_style_t* style) {
		tm_ui_buffers_t buffers = tm_ui_api->buffers(ui);
		tm_draw2d_context* ctx = (tm_draw2d_context*)tm_alloc(_allocator, sizeof(tm_draw2d_context));
		ctx->ibuffer = buffers.ibuffers;
		ctx->vbuffer = buffers.vbuffer;
		ctx->style = tm_ui_api->to_draw_style(ui, nullptr, style);
		return ctx;
	}

	void destroy_draw_context(tm_draw2d_context* ctx) {
		tm_free(_allocator, ctx, sizeof(tm_draw2d_context));
	}

	void fill_rect(tm_draw2d_context* ctx, tm_rect_t r) {
		tm_draw2d_api->fill_rect(ctx->vbuffer, *ctx->ibuffer, ctx->style, r);
	}

	void fill_circle(tm_draw2d_context* ctx, tm_vec2_t pos, float radius) {
		tm_draw2d_api->fill_circle(ctx->vbuffer, *ctx->ibuffer, ctx->style, pos, radius);
	}

	void fill_rounded_rect(tm_draw2d_context* ctx, tm_rect_t r, float radius) {
		tm_draw2d_api->fill_rounded_rect(ctx->vbuffer, *ctx->ibuffer, ctx->style, r, radius);
	}

	void register_ui_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {
		_allocator = allocator;
		AS_CHECK(engine->RegisterObjectType("tm_draw2d_context", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));

		AS_CHECK(engine->SetDefaultNamespace("tm_ui_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_rect_t text(tm_ui_o@ ui, const tm_ui_style_t@ style, const tm_str_t string, const tm_rect_t rect, const tm_color_srgb_t color)", asFUNCTION(text), asCALL_CDECL));

		AS_CHECK(engine->RegisterGlobalFunction("tm_draw2d_context@ create_draw_context(tm_ui_o@ ui, const tm_ui_style_t@ style)", asFUNCTION(create_draw_context), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void destroy_draw_context(tm_draw2d_context@ ctx)", asFUNCTION(destroy_draw_context), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void fill_rect(tm_draw2d_context@ ctx, const tm_rect_t rect)", asFUNCTION(fill_rect), asCALL_CDECL));

		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}