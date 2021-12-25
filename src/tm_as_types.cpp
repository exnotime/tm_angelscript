#include "tm_as_types.h"
extern "C" {
	#include <foundation/api_types.h>
	#include <plugins/entity/entity_api_types.h>
	#include <foundation/log.h>
	#include <foundation/allocator.h>
}
#include "tm_angelscript.h"

#include <angelscript.h>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering Angelscript type");} } while(false)

tm_ref_transform_t* transform_factory() {
	tm_ref_transform_t* t = (tm_ref_transform_t*)tm_alloc(get_system()->allocator, sizeof(tm_ref_transform_t));
	t->ref_count = 1;
	return t;
}

void transform_add_ref(tm_ref_transform_t* t) {
	t->ref_count++;
}

void transform_release(tm_ref_transform_t* t) {
	t->ref_count--;
	if (t->ref_count == 0) {
		tm_free(get_system()->allocator, t, sizeof(tm_ref_transform_t));
	}
}

//Registers base types of the machinery api
void register_tm_types(asIScriptEngine* engine)
{
	AS_CHECK(engine->RegisterObjectType("tm_vec2_t", sizeof(tm_vec2_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_vec2_t>()));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec2_t", "float x", asOFFSET(tm_vec2_t, x)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec2_t", "float y", asOFFSET(tm_vec2_t, y)));

	AS_CHECK(engine->RegisterObjectType("tm_vec3_t", sizeof(tm_vec3_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_vec3_t>()));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec3_t", "float x", asOFFSET(tm_vec3_t, x)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec3_t", "float y", asOFFSET(tm_vec3_t, y)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec3_t", "float z", asOFFSET(tm_vec3_t, z)));

	AS_CHECK(engine->RegisterObjectType("tm_vec4_t", sizeof(tm_vec4_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_vec4_t>()));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float x", asOFFSET(tm_vec4_t, x)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float y", asOFFSET(tm_vec4_t, y)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float z", asOFFSET(tm_vec4_t, z)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float w", asOFFSET(tm_vec4_t, w)));

	AS_CHECK(engine->RegisterObjectType("tm_mat44_t", sizeof(tm_mat44_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_mat44_t>()));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float xx", asOFFSET(tm_mat44_t, xx)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float xy", asOFFSET(tm_mat44_t, xy)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float xz", asOFFSET(tm_mat44_t, xz)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float xw", asOFFSET(tm_mat44_t, xw)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float yx", asOFFSET(tm_mat44_t, yx)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float yy", asOFFSET(tm_mat44_t, yy)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float yz", asOFFSET(tm_mat44_t, yz)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float yw", asOFFSET(tm_mat44_t, yw)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float zx", asOFFSET(tm_mat44_t, zx)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float zy", asOFFSET(tm_mat44_t, zy)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float zz", asOFFSET(tm_mat44_t, zz)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float zw", asOFFSET(tm_mat44_t, zw)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float wx", asOFFSET(tm_mat44_t, wx)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float wy", asOFFSET(tm_mat44_t, wy)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float wz", asOFFSET(tm_mat44_t, wz)));
	AS_CHECK(engine->RegisterObjectProperty("tm_mat44_t", "float ww", asOFFSET(tm_mat44_t, ww)));

	AS_CHECK(engine->RegisterObjectType("tm_transform_t", sizeof(tm_transform_t), asOBJ_REF | asOBJ_NOCOUNT));
	AS_CHECK(engine->RegisterObjectProperty("tm_transform_t", "tm_vec3_t pos", asOFFSET(tm_transform_t, pos)));
	AS_CHECK(engine->RegisterObjectProperty("tm_transform_t", "tm_vec4_t rot", asOFFSET(tm_transform_t, rot)));
	AS_CHECK(engine->RegisterObjectProperty("tm_transform_t", "tm_vec3_t scl", asOFFSET(tm_transform_t, scl)));
	//AS_CHECK(engine->RegisterObjectBehaviour("tm_transform_t", asBEHAVE_FACTORY, "tm_transform_t@ f()", asFUNCTION(transform_factory), asCALL_CDECL));
	//AS_CHECK(engine->RegisterObjectBehaviour("tm_transform_t", asBEHAVE_ADDREF, "void f()", asFUNCTION(transform_add_ref), asCALL_CDECL_OBJLAST));
	//AS_CHECK(engine->RegisterObjectBehaviour("tm_transform_t", asBEHAVE_RELEASE, "void f()", asFUNCTION(transform_release), asCALL_CDECL_OBJLAST));


	AS_CHECK(engine->RegisterObjectType("tm_rect_t", sizeof(tm_rect_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_rect_t>()));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float x", asOFFSET(tm_rect_t, x)));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float y", asOFFSET(tm_rect_t, y)));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float w", asOFFSET(tm_rect_t, w)));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float h", asOFFSET(tm_rect_t, h)));

	AS_CHECK(engine->RegisterTypedef("tm_clock_o", "uint64"));

	//Do we need to expose the a/b values of the UUID?
	AS_CHECK(engine->RegisterObjectType("tm_uuid_t", sizeof(tm_uuid_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_uuid_t>()));

	AS_CHECK(engine->RegisterObjectType("tm_color_srgb_t", sizeof(tm_color_srgb_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_color_srgb_t>()));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 x", asOFFSET(tm_color_srgb_t, r)));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 y", asOFFSET(tm_color_srgb_t, g)));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 z", asOFFSET(tm_color_srgb_t, b)));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 w", asOFFSET(tm_color_srgb_t, a)));
	AS_CHECK(engine->RegisterTypedef("tm_tt_type_t", "uint64"));
	AS_CHECK(engine->RegisterTypedef("tm_tt_id_t", "uint64"));
	AS_CHECK(engine->RegisterGlobalFunction("tm_tt_type_t tm_tt_type(tm_tt_id_t id)", asFUNCTION(tm_tt_type), asCALL_CDECL));
	AS_CHECK(engine->RegisterObjectType("tm_component_type_t", sizeof(tm_component_type_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_component_type_t>()));
	
}
