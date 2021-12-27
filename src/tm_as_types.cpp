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
#pragma region tm_vec2_t
void construct_vec2(float x, float y, tm_vec2_t* obj) {
	obj->x = x; obj->y = y;
}

void construct_vec2(float x, tm_vec2_t* obj) {
	construct_vec2(x, x, obj);
}

void vec2_opSubAssign(const tm_vec2_t other, tm_vec2_t* obj) {
	obj->x -= other.x;
	obj->y -= other.y;
}
tm_vec2_t vec2_opSub(const tm_vec2_t other, tm_vec2_t* obj) {
	tm_vec2_t r;
	r.x = obj->x - other.x;
	r.y = obj->y - other.y;
	return r;
}
void vec2_opAddAssign(const tm_vec2_t other, tm_vec2_t* obj) {
	obj->x += other.x;
	obj->y += other.y;
}
tm_vec2_t vec2_opAdd(const tm_vec2_t other, tm_vec2_t* obj) {
	tm_vec2_t r;
	r.x = obj->x + other.x;
	r.y = obj->y + other.y;
	return r;
}
void vec2_opMulAssign(const tm_vec2_t other, tm_vec2_t* obj) {
	obj->x *= other.x;
	obj->y *= other.y;
}
tm_vec2_t vec2_opMul(const tm_vec2_t other, tm_vec2_t* obj) {
	tm_vec2_t r;
	r.x = obj->x * other.x;
	r.y = obj->y * other.y;
	return r;
}

void vec2_opSubAssign_f(const float x, tm_vec2_t* obj) {
	obj->x -= x;
	obj->y -= x;
}
tm_vec2_t vec2_opSub_f(const float x, tm_vec2_t* obj) {
	tm_vec2_t r;
	r.x = obj->x - x;
	r.y = obj->y - x;
	return r;
}
void vec2_opAddAssign_f(const float x, tm_vec2_t* obj) {
	obj->x += x;
	obj->y += x;
}
tm_vec2_t vec2_opAdd_f(const float x, tm_vec2_t* obj) {
	tm_vec2_t r;
	r.x = obj->x + x;
	r.y = obj->y + x;
	return r;
}
void vec2_opMulAssign_f(const float x, tm_vec2_t* obj) {
	obj->x *= x;
	obj->y *= x;
}
tm_vec2_t vec2_opMul_f(const float x, tm_vec2_t* obj) {
	tm_vec2_t r;
	r.x = obj->x * x;
	r.y = obj->y * x;
	return r;
}

#pragma endregion

//Vec3
void construct_vec3(float x, float y, float z, tm_vec3_t* obj) {
	obj->x = x; obj->y = y; obj->z = z;
}

void construct_vec3(float x, tm_vec3_t* obj) {
	construct_vec3(x, x, x, obj);
}

//Vec4
void construct_vec4(float x, float y, float z, float w, tm_vec4_t* obj) {
	obj->x = x; obj->y = y; obj->z = z; obj->w = w;
}

void construct_vec4(float x, tm_vec4_t* obj) {
	construct_vec4(x, x, x, x, obj);
}

//Registers base types of the machinery api
void register_tm_types(asIScriptEngine* engine)
{
	AS_CHECK(engine->RegisterObjectType("tm_vec2_t", sizeof(tm_vec2_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_vec2_t>()));
	AS_CHECK(engine->RegisterObjectType("tm_vec3_t", sizeof(tm_vec3_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_vec3_t>()));
	AS_CHECK(engine->RegisterObjectType("tm_vec4_t", sizeof(tm_vec4_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_vec4_t>()));

	
	AS_CHECK(engine->RegisterObjectProperty("tm_vec2_t", "float x", asOFFSET(tm_vec2_t, x)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec2_t", "float y", asOFFSET(tm_vec2_t, y)));
	AS_CHECK(engine->RegisterObjectBehaviour("tm_vec2_t", asBEHAVE_CONSTRUCT, "void f(float x, float y)", asFUNCTIONPR(construct_vec2, (float, float, tm_vec2_t*), void), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectBehaviour("tm_vec2_t", asBEHAVE_CONSTRUCT, "void f(float x)", asFUNCTIONPR(construct_vec2, (float, tm_vec2_t*), void), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opSub(const tm_vec2_t v)", asFUNCTION(vec2_opSub), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opAdd(const tm_vec2_t v)", asFUNCTION(vec2_opAdd), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opMul(const tm_vec2_t v)", asFUNCTION(vec2_opMul), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opSubAssign(const tm_vec2_t v)", asFUNCTION(vec2_opSubAssign), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opAddAssign(const tm_vec2_t v)", asFUNCTION(vec2_opAddAssign), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opMulAssign(const tm_vec2_t v)", asFUNCTION(vec2_opMulAssign), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opSub(const float x)", asFUNCTION(vec2_opSub_f), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opAdd(const float x)", asFUNCTION(vec2_opAdd_f), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opMul(const float x)", asFUNCTION(vec2_opMul_f), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opSubAssign(const float x)", asFUNCTION(vec2_opSubAssign_f), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opAddAssign(const float x)", asFUNCTION(vec2_opAddAssign_f), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opMulAssign(const float x)", asFUNCTION(vec2_opMulAssign_f), asCALL_CDECL_OBJLAST));


	AS_CHECK(engine->RegisterObjectProperty("tm_vec3_t", "float x", asOFFSET(tm_vec3_t, x)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec3_t", "float y", asOFFSET(tm_vec3_t, y)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec3_t", "float z", asOFFSET(tm_vec3_t, z)));
	AS_CHECK(engine->RegisterObjectBehaviour("tm_vec3_t", asBEHAVE_CONSTRUCT, "void f(float x, float y, float z)", asFUNCTIONPR(construct_vec3, (float, float, float, tm_vec3_t*), void), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectBehaviour("tm_vec3_t", asBEHAVE_CONSTRUCT, "void f(float x)", asFUNCTIONPR(construct_vec3, (float, tm_vec3_t*), void), asCALL_CDECL_OBJLAST));

	
	AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float x", asOFFSET(tm_vec4_t, x)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float y", asOFFSET(tm_vec4_t, y)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float z", asOFFSET(tm_vec4_t, z)));
	AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float w", asOFFSET(tm_vec4_t, w)));
	AS_CHECK(engine->RegisterObjectBehaviour("tm_vec4_t", asBEHAVE_CONSTRUCT, "void f(float x, float y, float z, float w)", asFUNCTIONPR(construct_vec4, (float, float, float, float, tm_vec4_t*), void), asCALL_CDECL_OBJLAST));
	AS_CHECK(engine->RegisterObjectBehaviour("tm_vec4_t", asBEHAVE_CONSTRUCT, "void f(float x)", asFUNCTIONPR(construct_vec4, (float, tm_vec4_t*), void), asCALL_CDECL_OBJLAST));

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


	AS_CHECK(engine->RegisterObjectType("tm_rect_t", sizeof(tm_rect_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_rect_t>()));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float x", asOFFSET(tm_rect_t, x)));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float y", asOFFSET(tm_rect_t, y)));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float w", asOFFSET(tm_rect_t, w)));
	AS_CHECK(engine->RegisterObjectProperty("tm_rect_t", "float h", asOFFSET(tm_rect_t, h)));

	AS_CHECK(engine->RegisterTypedef("tm_clock_o", "uint64"));

	//Do we need to expose the a/b values of the UUID?
	AS_CHECK(engine->RegisterObjectType("tm_uuid_t", sizeof(tm_uuid_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_uuid_t>()));

	AS_CHECK(engine->RegisterObjectType("tm_color_srgb_t", sizeof(tm_color_srgb_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_color_srgb_t>()));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 r", asOFFSET(tm_color_srgb_t, r)));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 g", asOFFSET(tm_color_srgb_t, g)));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 b", asOFFSET(tm_color_srgb_t, b)));
	AS_CHECK(engine->RegisterObjectProperty("tm_color_srgb_t", "uint8 a", asOFFSET(tm_color_srgb_t, a)));
	AS_CHECK(engine->RegisterTypedef("tm_tt_type_t", "uint64"));
	AS_CHECK(engine->RegisterTypedef("tm_tt_id_t", "uint64"));
	AS_CHECK(engine->RegisterGlobalFunction("tm_tt_type_t tm_tt_type(tm_tt_id_t id)", asFUNCTION(tm_tt_type), asCALL_CDECL));
	AS_CHECK(engine->RegisterObjectType("tm_component_type_t", sizeof(tm_component_type_t), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<tm_component_type_t>()));
	
}
