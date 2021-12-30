#include "tm_as_camera_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include "tm_as_types.h"//for transform
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering camera to Angelscript, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <plugins/entity/entity.h>
#include <foundation/log.h>
#include <foundation/camera.h>
#include <foundation/math.inl>
}
namespace tm_camera {

	void update_free_flight(tm_transform_t* transform, tm_vec3_t translation, tm_vec2_t rotation) {
		tm_camera_api->update_free_flight(transform, translation, rotation);
	}

	void update_pan(tm_transform_t* transform, tm_vec3_t& focus_position, tm_vec2_t pan, bool has_focus_position) {
		tm_camera_api->update_pan(transform, has_focus_position ? &focus_position : nullptr, pan);
	}

	void update_maya(tm_transform_t* transform, tm_vec3_t focus_position, float zoom, tm_vec2_t rotation) {
		tm_camera_api->update_maya(transform, focus_position, zoom, rotation);
	}
	//TODO: Expose function that takes in an array of positions
	tm_vec3_t screen_to_world(const tm_camera_t* camera, tm_rect_t viewport, tm_vec3_t screen_pos) {
		tm_vec3_t world_pos;
		tm_camera_api->screen_to_world(camera, TM_CAMERA_TRANSFORM_DEFAULT, viewport, &screen_pos, &world_pos, 1);
		return world_pos;
	}

	tm_vec3_t world_to_screen(const tm_camera_t* camera, tm_rect_t viewport, tm_vec3_t world_pos) {
		tm_vec3_t screen_pos;
		tm_camera_api->screen_to_world(camera, TM_CAMERA_TRANSFORM_DEFAULT, viewport, &screen_pos, &world_pos, 1);
		return screen_pos;
	}

	//Convenience functions to get unit vectors from a camera
	tm_vec3_t camera_right(tm_camera_t* camera) {
		return *tm_mat44_x(&camera->view[TM_CAMERA_TRANSFORM_DEFAULT]);
	}

	tm_vec3_t camera_up(tm_camera_t* camera) {
		return *tm_mat44_y(&camera->view[TM_CAMERA_TRANSFORM_DEFAULT]);
	}

	tm_vec3_t camera_forward(tm_camera_t* camera) {
		return *tm_mat44_z(&camera->view[TM_CAMERA_TRANSFORM_DEFAULT]);
	}

	void look_at(tm_transform_t* transform, tm_vec3_t pos, tm_vec3_t global_up) {
		tm_vec3_t forward = tm_vec3_normalize(tm_vec3_sub(pos, transform->pos));
		tm_vec3_t right = tm_vec3_normalize(tm_vec3_cross(forward, global_up));
		tm_vec3_t up = tm_vec3_normalize(tm_vec3_cross(forward, right));
		transform->rot = tm_quaternion_from_direction(forward, up);
	}

	void look_at_distance(tm_transform_t* transform, tm_vec3_t pos, tm_vec3_t global_up, float distance) {
		tm_vec3_t forward = tm_vec3_normalize(tm_vec3_sub(pos, transform->pos));
		tm_vec3_t right = tm_vec3_normalize(tm_vec3_cross(forward, global_up));
		tm_vec3_t up = tm_vec3_normalize(tm_vec3_cross(right,forward));
		transform->rot = tm_quaternion_from_direction(forward, up);
		transform->pos = tm_vec3_sub( tm_vec3_mul(forward, distance), pos);
	}
	//Custom version of update maya function. This one will always perform the transform and allows you to set the wanted distance
	// Copied from the camera.c file in the engine repo and then modified
	void update_maya_ext(tm_transform_t* tm, tm_vec3_t focus, tm_vec2_t rot, float distance)
	{
		const tm_vec3_t p = tm_vec3_sub(tm->pos, focus);
		float r = tm_vec3_length(p);
		if (r < 1e-5f)
			r = 1e-5f;
		float tau = asinf(p.y / r);
		float phi = atan2f(p.z, p.x);

		phi += rot.x * 0.005f;
		tau += rot.y * 0.005f;
		r = distance;

		tau = tau > 1.5f ? 1.5f : tau < -1.5f ? -1.5f : tau;

		const tm_vec3_t z = { cosf(phi) * cosf(tau), sinf(tau), sinf(phi) * cosf(tau) };
		const tm_vec3_t t = { focus.x + r * z.x, focus.y + r * z.y, focus.z + r * z.z };
		const tm_vec3_t up = { 0, 1, 0 };
		const tm_vec3_t x = tm_vec3_normalize(tm_vec3_cross(up, z));
		const tm_vec3_t y = tm_vec3_cross(z, x);

		tm_mat44_t c = *tm_mat44_identity();
		c.xx = x.x, c.xy = x.y, c.xz = x.z;
		c.yx = y.x, c.yy = y.y, c.yz = y.z;
		c.zx = z.x, c.zy = z.y, c.zz = z.z;
		c.wx = t.x, c.wy = t.y, c.wz = t.z;
		tm_mat44_to_translation_quaternion_scale(&tm->pos, &tm->rot, &tm->scl, &c);
	}

	void register_camera_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {
		AS_CHECK(engine->SetDefaultNamespace("tm_camera_api"));
		AS_CHECK(engine->RegisterGlobalFunction("void update_free_flight(const tm_transform_t@ transform, tm_vec3_t t, tm_vec2_t rot)", asFUNCTION(update_free_flight), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void update_pan(const tm_transform_t@ transform, tm_vec3_t focus, tm_vec2_t pan, bool has_focus)", asFUNCTION(update_pan), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void update_maya(const tm_transform_t@ transform, tm_vec3_t focus, float zoom, tm_vec2_t rot)", asFUNCTION(update_maya), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t screen_to_world(const tm_camera_t@ camera, tm_rect_t viewport, tm_vec3_t screen_pos)", asFUNCTION(update_maya), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t world_to_screen(const tm_camera_t@ camera, tm_rect_t viewport, tm_vec3_t world_pos)", asFUNCTION(world_to_screen), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace("tm_camera_ext_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t camera_right(tm_camera_t@ camera)", asFUNCTION(camera_right), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t camera_up(tm_camera_t@ camera)", asFUNCTION(camera_up), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t camera_forward(tm_camera_t@ camera)", asFUNCTION(camera_forward), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void look_at(const tm_transform_t@ transform, tm_vec3_t pos, tm_vec3_t global_up)", asFUNCTION(look_at), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void look_at_distance(const tm_transform_t@ transform, tm_vec3_t pos, tm_vec3_t global_up, float distance)", asFUNCTION(look_at_distance), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void update_maya(const tm_transform_t@ transform, tm_vec3_t focus, tm_vec2_t rot, float distance)", asFUNCTION(update_maya_ext), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}