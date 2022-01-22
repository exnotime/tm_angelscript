#include "tm_as_types.h"
extern "C" {
	#include <foundation/api_types.h>
	#include <plugins/entity/entity_api_types.h>
	#include <foundation/log.h>
	#include <foundation/allocator.h>
	#include <plugins/physx/physx_scene.h>
}
#include "tm_angelscript.h"

#include <angelscript.h>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering Angelscript type");} } while(false)

void construct_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a, tm_color_srgb_t* obj) {
	obj->r = r; obj->g = g; obj->b = b; obj->a = a;
}

//Registers base types of the machinery api
void register_tm_types(asIScriptEngine* engine)
{
	AS_CHECK(engine->RegisterObjectType("tm_vec2_t", sizeof(tm_vec2_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_vec2_t>()));
	AS_CHECK(engine->RegisterObjectType("tm_vec3_t", sizeof(tm_vec3_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_vec3_t>()));
	AS_CHECK(engine->RegisterObjectType("tm_vec4_t", sizeof(tm_vec4_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_vec4_t>()));
	AS_CHECK(engine->RegisterObjectType("tm_mat44_t", sizeof(tm_mat44_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_mat44_t>()));
	
	AS_CHECK(engine->RegisterObjectType("tm_transform_t", sizeof(tm_transform_t), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectProperty("tm_transform_t", "tm_vec3_t pos", asOFFSET(tm_transform_t, pos)));
	AS_CHECK(engine->RegisterObjectProperty("tm_transform_t", "tm_vec4_t rot", asOFFSET(tm_transform_t, rot)));
	AS_CHECK(engine->RegisterObjectProperty("tm_transform_t", "tm_vec3_t scl", asOFFSET(tm_transform_t, scl)));


	AS_CHECK(engine->RegisterObjectType("tm_rect_t", sizeof(tm_rect_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_rect_t>()));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float x", asOFFSET(tm_rect_t, x)));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float y", asOFFSET(tm_rect_t, y)));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float w", asOFFSET(tm_rect_t, w)));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float h", asOFFSET(tm_rect_t, h)));

	//Do we need to expose the a/b values of the UUID?
	AS_CHECK(engine->RegisterObjectType("tm_uuid_t", sizeof(tm_uuid_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_uuid_t>()));

	AS_CHECK(engine->RegisterObjectType("tm_color_srgb_t", sizeof(tm_color_srgb_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_color_srgb_t>()));
	AS_CHECK(engine->RegisterObjectBehaviour("tm_color_srgb_t", asBEHAVE_CONSTRUCT, "void f(uint8 r, uint8 g, uint8 b, uint8 a)", asFUNCTION(construct_color), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 r", asOFFSET(tm_color_srgb_t, r)));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 g", asOFFSET(tm_color_srgb_t, g)));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 b", asOFFSET(tm_color_srgb_t, b)));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 a", asOFFSET(tm_color_srgb_t, a)));

	AS_CHECK(engine->RegisterTypedef("tm_tt_type_t", "uint64"));
	AS_CHECK(engine->RegisterTypedef("tm_tt_id_t", "uint64"));
	AS_CHECK(engine->RegisterTypedef("tm_entity_t", "uint64"));
	AS_CHECK(engine->RegisterTypedef("tm_clock_o", "uint64"));
	AS_CHECK(engine->RegisterTypedef("tm_strhash_t", "uint64"));

	AS_CHECK(engine->RegisterGlobalFunction("tm_tt_type_t tm_tt_type(tm_tt_id_t id)", asFUNCTION(tm_tt_type), asCALL_CDECL));
	AS_CHECK(engine->RegisterObjectType("tm_component_type_t", sizeof(tm_component_type_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_component_type_t>()));

	AS_CHECK(engine->RegisterObjectType("tm_entity_context_o", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_component_i", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_entity_commands_o", sizeof(void**), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_the_truth_o", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_the_truth_object_o", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_physx_scene_o", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_simulation_o", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_simulation_frame_args_t", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_simulation_start_args_t", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_component_manager_o", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_camera_t", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_ui_o", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectType("tm_ui_style_t", sizeof(void*), asOBJ_REF | asOBJ_NOCOUNT));
}
