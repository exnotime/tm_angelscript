#include "tm_as_math_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include "tm_as_types.h"//for transform
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering math to Angelscript, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <plugins/entity/entity.h>
#include <foundation/log.h>
#include <foundation/math.h>
#include <foundation/math.inl>
}
namespace tm_math {

#pragma region tm_vec2_t
	void construct_vec2(float x, float y, tm_vec2_t* obj) {
		obj->x = x; obj->y = y;
	}

	void construct_vec2(float x, tm_vec2_t* obj) {
		construct_vec2(x, x, obj);
	}

	
	tm_vec2_t vec2_opSub(const tm_vec2_t other, tm_vec2_t* obj) {
		return tm_vec2_sub(*obj, other);
	}
	
	tm_vec2_t vec2_opAdd(const tm_vec2_t other, tm_vec2_t* obj) {
		return tm_vec2_add(*obj, other);
	}
	
	tm_vec2_t vec2_opMul(const tm_vec2_t other, tm_vec2_t* obj) {
		return tm_vec2_element_mul(*obj, other);
	}
	tm_vec2_t vec2_opDiv(const tm_vec2_t other, tm_vec2_t* obj) {
		return tm_vec2_element_div(*obj, other);
	}

	void vec2_opAddAssign(const tm_vec2_t other, tm_vec2_t* obj) {
		obj->x += other.x;
		obj->y += other.y;
	}
	void vec2_opSubAssign(const tm_vec2_t other, tm_vec2_t* obj) {
		obj->x -= other.x;
		obj->y -= other.y;
	}
	void vec2_opMulAssign(const tm_vec2_t other, tm_vec2_t* obj) {
		obj->x *= other.x;
		obj->y *= other.y;
	}
	void vec2_opDivAssign(const tm_vec2_t other, tm_vec2_t* obj) {
		obj->x /= other.x;
		obj->y /= other.y;
	}

	void vec2_opSubAssign_f(const float x, tm_vec2_t* obj) {
		obj->x -= x;
		obj->y -= x;
	}
	void vec2_opAddAssign_f(const float x, tm_vec2_t* obj) {
		obj->x += x;
		obj->y += x;
	}
	void vec2_opMulAssign_f(const float x, tm_vec2_t* obj) {
		obj->x *= x;
		obj->y *= x;
	}
	void vec2_opDivAssign_f(const float x, tm_vec2_t* obj) {
		obj->x /= x;
		obj->y /= x;
	}

	tm_vec2_t vec2_opAdd_f(const float x, tm_vec2_t* obj) {
		tm_vec2_t r;
		r.x = obj->x + x;
		r.y = obj->y + x;
		return r;
	}
	tm_vec2_t vec2_opSub_f(const float x, tm_vec2_t* obj) {
		tm_vec2_t r;
		r.x = obj->x - x;
		r.y = obj->y - x;
		return r;
	}
	
	tm_vec2_t vec2_opMul_f(const float x, tm_vec2_t* obj) {
		return tm_vec2_mul(*obj, x);
	}
	tm_vec2_t vec2_opDiv_f(const float x, tm_vec2_t* obj) {
		tm_vec2_t r;
		r.x = obj->x / x;
		r.y = obj->y / x;
		return r;
	}

#pragma endregion
#pragma region tm_vec3_t
	//Vec3
	void construct_vec3(float x, float y, float z, tm_vec3_t* obj) {
		obj->x = x; obj->y = y; obj->z = z;
	}

	void construct_vec3(float x, tm_vec3_t* obj) {
		construct_vec3(x, x, x, obj);
	}

	tm_vec3_t vec3_opSub(const tm_vec3_t other, tm_vec3_t* obj) {
		return tm_vec3_sub(*obj, other);
	}

	tm_vec3_t vec3_opAdd(const tm_vec3_t other, tm_vec3_t* obj) {
		return tm_vec3_add(*obj, other);
	}

	tm_vec3_t vec3_opMul(const tm_vec3_t other, tm_vec3_t* obj) {
		return tm_vec3_element_mul(*obj, other);
	}
	tm_vec3_t vec3_opDiv(const tm_vec3_t other, tm_vec3_t* obj) {
		return tm_vec3_element_div(*obj, other);
	}

	void vec3_opAddAssign(const tm_vec3_t other, tm_vec3_t* obj) {
		obj->x += other.x;
		obj->y += other.y;
		obj->z += other.z;
	}
	void vec3_opSubAssign(const tm_vec3_t other, tm_vec3_t* obj) {
		obj->x -= other.x;
		obj->y -= other.y;
		obj->z -= other.z;
	}
	void vec3_opMulAssign(const tm_vec3_t other, tm_vec3_t* obj) {
		obj->x *= other.x;
		obj->y *= other.y;
		obj->z *= other.z;
	}
	void vec3_opDivAssign(const tm_vec3_t other, tm_vec3_t* obj) {
		obj->x /= other.x;
		obj->y /= other.y;
		obj->z /= other.z;
	}

	void vec3_opSubAssign_f(const float x, tm_vec3_t* obj) {
		obj->x -= x;
		obj->y -= x;
		obj->z -= x;
	}
	void vec3_opAddAssign_f(const float x, tm_vec3_t* obj) {
		obj->x += x;
		obj->y += x;
		obj->z += x;
	}
	void vec3_opMulAssign_f(const float x, tm_vec3_t* obj) {
		obj->x *= x;
		obj->y *= x;
		obj->z *= x;
	}
	void vec3_opDivAssign_f(const float x, tm_vec3_t* obj) {
		obj->x /= x;
		obj->y /= x;
		obj->z /= x;
	}

	tm_vec3_t vec3_opAdd_f(const float x, tm_vec3_t* obj) {
		tm_vec3_t r;
		r.x = obj->x + x;
		r.y = obj->y + x;
		r.z = obj->z + x;
		return r;
	}
	tm_vec3_t vec3_opSub_f(const float x, tm_vec3_t* obj) {
		tm_vec3_t r;
		r.x = obj->x - x;
		r.y = obj->y - x;
		r.z = obj->z - x;
		return r;
	}

	tm_vec3_t vec3_opMul_f(const float x, tm_vec3_t* obj) {
		return tm_vec3_mul(*obj, x);
	}
	tm_vec3_t vec3_opDiv_f(const float x, tm_vec3_t* obj) {
		tm_vec3_t r;
		r.x = obj->x / x;
		r.y = obj->y / x;
		r.z = obj->z / x;
		return r;
	}
#pragma endregion
#pragma region tm_vec4_t
	//Vec4
	void construct_vec4(float x, float y, float z, float w, tm_vec4_t* obj) {
		obj->x = x; obj->y = y; obj->z = z; obj->w = w;
	}

	void construct_vec4(float x, tm_vec4_t* obj) {
		construct_vec4(x, x, x, x, obj);
	}

	tm_vec4_t vec4_opSub(const tm_vec4_t other, tm_vec4_t* obj) {
		return tm_vec4_sub(*obj, other);
	}

	tm_vec4_t vec4_opAdd(const tm_vec4_t other, tm_vec4_t* obj) {
		return tm_vec4_add(*obj, other);
	}

	tm_vec4_t vec4_opMul(const tm_vec4_t other, tm_vec4_t* obj) {
		return tm_vec4_element_mul(*obj, other);
	}
	tm_vec4_t vec4_opDiv(const tm_vec4_t other, tm_vec4_t* obj) {
		return tm_vec4_element_div(*obj, other);
	}

	void vec4_opAddAssign(const tm_vec4_t other, tm_vec4_t* obj) {
		obj->x += other.x;
		obj->y += other.y;
		obj->z += other.z;
		obj->w += other.w;
	}
	void vec4_opSubAssign(const tm_vec4_t other, tm_vec4_t* obj) {
		obj->x -= other.x;
		obj->y -= other.y;
		obj->z -= other.z;
		obj->w -= other.w;
	}
	void vec4_opMulAssign(const tm_vec4_t other, tm_vec4_t* obj) {
		obj->x *= other.x;
		obj->y *= other.y;
		obj->z *= other.z;
		obj->w *= other.w;
	}
	void vec4_opDivAssign(const tm_vec4_t other, tm_vec4_t* obj) {
		obj->x /= other.x;
		obj->y /= other.y;
		obj->z /= other.z;
		obj->w /= other.w;
	}

	void vec4_opSubAssign_f(const float x, tm_vec4_t* obj) {
		obj->x -= x;
		obj->y -= x;
		obj->z -= x;
		obj->w -= x;
	}
	void vec4_opAddAssign_f(const float x, tm_vec4_t* obj) {
		obj->x += x;
		obj->y += x;
		obj->z += x;
		obj->w += x;
	}
	void vec4_opMulAssign_f(const float x, tm_vec4_t* obj) {
		obj->x *= x;
		obj->y *= x;
		obj->z *= x;
		obj->w *= x;
	}
	void vec4_opDivAssign_f(const float x, tm_vec4_t* obj) {
		obj->x /= x;
		obj->y /= x;
		obj->z /= x;
		obj->w /= x;
	}

	tm_vec4_t vec4_opAdd_f(const float x, tm_vec4_t* obj) {
		tm_vec4_t r;
		r.x = obj->x + x;
		r.y = obj->y + x;
		r.z = obj->z + x;
		r.w = obj->w + x;
		return r;
	}
	tm_vec4_t vec4_opSub_f(const float x, tm_vec4_t* obj) {
		tm_vec4_t r;
		r.x = obj->x - x;
		r.y = obj->y - x;
		r.z = obj->z - x;
		r.w = obj->w - x;
		return r;
	}

	tm_vec4_t vec4_opMul_f(const float x, tm_vec4_t* obj) {
		return tm_vec4_mul(*obj, x);
	}
	tm_vec4_t vec4_opDiv_f(const float x, tm_vec4_t* obj) {
		tm_vec4_t r;
		r.x = obj->x / x;
		r.y = obj->y / x;
		r.z = obj->z / x;
		r.w = obj->w / x;
		return r;
	}
#pragma endregion

	void register_math_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {

		AS_CHECK(engine->RegisterObjectProperty("tm_vec2_t", "float x", asOFFSET(tm_vec2_t, x)));
		AS_CHECK(engine->RegisterObjectProperty("tm_vec2_t", "float y", asOFFSET(tm_vec2_t, y)));
		AS_CHECK(engine->RegisterObjectBehaviour("tm_vec2_t", asBEHAVE_CONSTRUCT, "void f(float x, float y)", asFUNCTIONPR(construct_vec2, (float, float, tm_vec2_t*), void), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectBehaviour("tm_vec2_t", asBEHAVE_CONSTRUCT, "void f(float x)", asFUNCTIONPR(construct_vec2, (float, tm_vec2_t*), void), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opSub(const tm_vec2_t v)", asFUNCTION(vec2_opSub), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opAdd(const tm_vec2_t v)", asFUNCTION(vec2_opAdd), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opMul(const tm_vec2_t v)", asFUNCTION(vec2_opMul), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opDiv(const tm_vec2_t v)", asFUNCTION(vec2_opDiv), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opSubAssign(const tm_vec2_t v)", asFUNCTION(vec2_opSubAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opAddAssign(const tm_vec2_t v)", asFUNCTION(vec2_opAddAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opMulAssign(const tm_vec2_t v)", asFUNCTION(vec2_opMulAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opDivAssign(const tm_vec2_t v)", asFUNCTION(vec2_opDivAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opSub(const float x)", asFUNCTION(vec2_opSub_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opAdd(const float x)", asFUNCTION(vec2_opAdd_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opMul(const float x)", asFUNCTION(vec2_opMul_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "tm_vec2_t opDiv(const float x)", asFUNCTION(vec2_opDiv_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opSubAssign(const float x)", asFUNCTION(vec2_opSubAssign_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opAddAssign(const float x)", asFUNCTION(vec2_opAddAssign_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opMulAssign(const float x)", asFUNCTION(vec2_opMulAssign_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec2_t", "void opDivAssign(const float x)", asFUNCTION(vec2_opDivAssign_f), asCALL_CDECL_OBJLAST));

		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec2_dot(const tm_vec2_t a, const tm_vec2_t b)", asFUNCTION(tm_vec2_dot), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec2_cross(const tm_vec2_t a, const tm_vec2_t b)", asFUNCTION(tm_vec2_cross), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec2_length(const tm_vec2_t v)", asFUNCTION(tm_vec2_length), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec2_t tm_vec2_normalize(const tm_vec2_t v)", asFUNCTION(tm_vec2_normalize), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("bool tm_vec2_equal(const tm_vec2_t v, const tm_vec2_t)", asFUNCTION(tm_vec2_equal), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec2_t tm_vec2_lerp(const tm_vec2_t a, const tm_vec2_t b, float t)", asFUNCTION(tm_vec2_lerp), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec2_t tm_vec2_min(const tm_vec2_t a, const tm_vec2_t b)", asFUNCTION(tm_vec2_min), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec2_t tm_vec2_max(const tm_vec2_t a, const tm_vec2_t b)", asFUNCTION(tm_vec2_max), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec2_t tm_vec2_clamp(const tm_vec2_t v, const tm_vec2_t lo, const tm_vec2_t hi)", asFUNCTION(tm_vec2_clamp), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec2_dist_sqr(const tm_vec2_t a, const tm_vec2_t b)", asFUNCTION(tm_vec2_dist_sqr), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec2_dist(const tm_vec2_t a, const tm_vec2_t b)", asFUNCTION(tm_vec2_dist), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec2_t tm_vec2_abs(const tm_vec2_t v)", asFUNCTION(tm_vec2_abs), asCALL_CDECL));

		AS_CHECK(engine->RegisterObjectProperty("tm_vec3_t", "float x", asOFFSET(tm_vec3_t, x)));
		AS_CHECK(engine->RegisterObjectProperty("tm_vec3_t", "float y", asOFFSET(tm_vec3_t, y)));
		AS_CHECK(engine->RegisterObjectProperty("tm_vec3_t", "float z", asOFFSET(tm_vec3_t, z)));
		AS_CHECK(engine->RegisterObjectBehaviour("tm_vec3_t", asBEHAVE_CONSTRUCT, "void f(float x, float y, float z)", asFUNCTIONPR(construct_vec3, (float, float, float, tm_vec3_t*), void), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectBehaviour("tm_vec3_t", asBEHAVE_CONSTRUCT, "void f(float x)", asFUNCTIONPR(construct_vec3, (float, tm_vec3_t*), void), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "tm_vec3_t opSub(const tm_vec3_t v)", asFUNCTION(vec3_opSub), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "tm_vec3_t opAdd(const tm_vec3_t v)", asFUNCTION(vec3_opAdd), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "tm_vec3_t opMul(const tm_vec3_t v)", asFUNCTION(vec3_opMul), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "tm_vec3_t opDiv(const tm_vec3_t v)", asFUNCTION(vec3_opDiv), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "void opSubAssign(const tm_vec3_t v)", asFUNCTION(vec3_opSubAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "void opAddAssign(const tm_vec3_t v)", asFUNCTION(vec3_opAddAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "void opMulAssign(const tm_vec3_t v)", asFUNCTION(vec3_opMulAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "void opDivAssign(const tm_vec3_t v)", asFUNCTION(vec3_opDivAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "tm_vec3_t opSub(const float x)", asFUNCTION(vec3_opSub_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "tm_vec3_t opAdd(const float x)", asFUNCTION(vec3_opAdd_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "tm_vec3_t opMul(const float x)", asFUNCTION(vec3_opMul_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "tm_vec3_t opDiv(const float x)", asFUNCTION(vec3_opDiv_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "void opSubAssign(const float x)", asFUNCTION(vec3_opSubAssign_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "void opAddAssign(const float x)", asFUNCTION(vec3_opAddAssign_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "void opMulAssign(const float x)", asFUNCTION(vec3_opMulAssign_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec3_t", "void opDivAssign(const float x)", asFUNCTION(vec3_opDivAssign_f), asCALL_CDECL_OBJLAST));

		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec3_dot(const tm_vec3_t a, const tm_vec3_t b)", asFUNCTION(tm_vec3_dot), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec3_cross(const tm_vec3_t a, const tm_vec3_t b)", asFUNCTION(tm_vec3_cross), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec3_length(const tm_vec3_t v)", asFUNCTION(tm_vec3_length), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t tm_vec3_normalize(const tm_vec3_t v)", asFUNCTION(tm_vec3_normalize), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("bool tm_vec3_equal(const tm_vec3_t v, const tm_vec3_t)", asFUNCTION(tm_vec3_equal), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t tm_vec3_lerp(const tm_vec3_t a, const tm_vec3_t b, float t)", asFUNCTION(tm_vec3_lerp), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t tm_vec3_min(const tm_vec3_t a, const tm_vec3_t b)", asFUNCTION(tm_vec3_min), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t tm_vec3_max(const tm_vec3_t a, const tm_vec3_t b)", asFUNCTION(tm_vec3_max), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t tm_vec3_clamp(const tm_vec3_t v, const tm_vec3_t lo, const tm_vec3_t hi)", asFUNCTION(tm_vec3_clamp), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec3_dist_sqr(const tm_vec3_t a, const tm_vec3_t b)", asFUNCTION(tm_vec3_dist_sqr), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec3_dist(const tm_vec3_t a, const tm_vec3_t b)", asFUNCTION(tm_vec3_dist), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t tm_vec3_abs(const tm_vec3_t v)", asFUNCTION(tm_vec3_abs), asCALL_CDECL));

		AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float x", asOFFSET(tm_vec4_t, x)));
		AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float y", asOFFSET(tm_vec4_t, y)));
		AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float z", asOFFSET(tm_vec4_t, z)));
		AS_CHECK(engine->RegisterObjectProperty("tm_vec4_t", "float w", asOFFSET(tm_vec4_t, w)));
		AS_CHECK(engine->RegisterObjectBehaviour("tm_vec4_t", asBEHAVE_CONSTRUCT, "void f(float x, float y, float z, float w)", asFUNCTIONPR(construct_vec4, (float, float, float, float, tm_vec4_t*), void), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectBehaviour("tm_vec4_t", asBEHAVE_CONSTRUCT, "void f(float x)", asFUNCTIONPR(construct_vec4, (float, tm_vec4_t*), void), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "tm_vec4_t opSub(const tm_vec4_t v)", asFUNCTION(vec4_opSub), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "tm_vec4_t opAdd(const tm_vec4_t v)", asFUNCTION(vec4_opAdd), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "tm_vec4_t opMul(const tm_vec4_t v)", asFUNCTION(vec4_opMul), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "tm_vec4_t opDiv(const tm_vec4_t v)", asFUNCTION(vec4_opDiv), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "void opSubAssign(const tm_vec4_t v)", asFUNCTION(vec4_opSubAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "void opAddAssign(const tm_vec4_t v)", asFUNCTION(vec4_opAddAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "void opMulAssign(const tm_vec4_t v)", asFUNCTION(vec4_opMulAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "void opDivAssign(const tm_vec4_t v)", asFUNCTION(vec4_opDivAssign), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "tm_vec4_t opSub(const float x)", asFUNCTION(vec4_opSub_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "tm_vec4_t opAdd(const float x)", asFUNCTION(vec4_opAdd_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "tm_vec4_t opMul(const float x)", asFUNCTION(vec4_opMul_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "tm_vec4_t opDiv(const float x)", asFUNCTION(vec4_opDiv_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "void opSubAssign(const float x)", asFUNCTION(vec4_opSubAssign_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "void opAddAssign(const float x)", asFUNCTION(vec4_opAddAssign_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "void opMulAssign(const float x)", asFUNCTION(vec4_opMulAssign_f), asCALL_CDECL_OBJLAST));
		AS_CHECK(engine->RegisterObjectMethod("tm_vec4_t", "void opDivAssign(const float x)", asFUNCTION(vec4_opDivAssign_f), asCALL_CDECL_OBJLAST));

		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec4_dot(const tm_vec4_t a, const tm_vec4_t b)", asFUNCTION(tm_vec4_dot), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec4_length(const tm_vec4_t v)", asFUNCTION(tm_vec4_length), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec4_t tm_vec4_normalize(const tm_vec4_t v)", asFUNCTION(tm_vec4_normalize), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("bool tm_vec4_equal(const tm_vec4_t v, const tm_vec4_t)", asFUNCTION(tm_vec4_equal), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec4_t tm_vec4_lerp(const tm_vec4_t a, const tm_vec4_t b, float t)", asFUNCTION(tm_vec4_lerp), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec4_t tm_vec4_min(const tm_vec4_t a, const tm_vec4_t b)", asFUNCTION(tm_vec4_min), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec4_t tm_vec4_max(const tm_vec4_t a, const tm_vec4_t b)", asFUNCTION(tm_vec4_max), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec4_t tm_vec4_clamp(const tm_vec4_t v, const tm_vec4_t lo, const tm_vec4_t hi)", asFUNCTION(tm_vec4_clamp), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec4_dist_sqr(const tm_vec4_t a, const tm_vec4_t b)", asFUNCTION(tm_vec4_dist_sqr), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("float tm_vec4_dist(const tm_vec4_t a, const tm_vec4_t b)", asFUNCTION(tm_vec4_dist), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec4_t tm_vec4_abs(const tm_vec4_t v)", asFUNCTION(tm_vec4_abs), asCALL_CDECL));

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

	}
}