#include "tm_as_component_api.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include "tm_as_types.h"
#include "tm_as_array.h"

#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering component Angelscript, %s:%d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <plugins/entity/entity.h>
#include <foundation/log.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_assets.h>
#include <foundation/temp_allocator.h>
#include <foundation/carray.inl>

#include <foundation/camera.h>
#include <plugins/entity/camera_component.h>
#include <plugins/entity/transform_component.h>
#include <plugins/entity/tag_component.h>
}

using namespace tm_array;

namespace tm_component {
	//Component lookup
	tm_component_type_t lookup_component_type(tm_entity_context_o* ctx, tm_strhash_t hashed_name) {
		return tm_entity_api->lookup_component_type(ctx, hashed_name);
	}

	tm_component_manager_o* component_manager(tm_entity_context_o* ctx, tm_component_type_t type) {
		return tm_entity_api->component_manager(ctx, type);
	}

	//TODO: Expose engine component hashes instead of dicrete functions to keep the interface pure?
	tm_component_manager_o* transform_component_manager(tm_entity_context_o* ctx) {
		return tm_entity_api->component_manager_by_hash(ctx, TM_TT_TYPE_HASH__TRANSFORM_COMPONENT);
	}

	tm_component_manager_o* tag_component_manager(tm_entity_context_o* ctx) {
		return tm_entity_api->component_manager_by_hash(ctx, TM_TT_TYPE_HASH__TAG_COMPONENT);
	}

	//Transform component
	tm_vec3_t get_position(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_position(man, e);
	}

	void set_position(tm_component_manager_o* manager, tm_entity_t e, tm_vec3_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_position(man, e, v);
	}

	tm_vec3_t get_scale(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_scale(man, e);
	}

	void set_scale(tm_component_manager_o* manager, tm_entity_t e, tm_vec3_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_scale(man, e, v);
	}

	tm_vec4_t get_rotation(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_rotation(man, e);
	}

	void set_rotation(tm_component_manager_o* manager, tm_entity_t e, tm_vec4_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_rotation(man, e, v);
	}

	const tm_transform_t* get_transform(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_transform(man, e);
	}

	void set_transform(tm_component_manager_o* manager, tm_entity_t e, const tm_transform_t* t) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_transform(man, e, t);
	}

	tm_vec3_t get_local_position(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_local_position(man, e);
	}

	void set_local_position(tm_component_manager_o* manager, tm_entity_t e, tm_vec3_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_local_position(man, e, v);
	}

	tm_vec3_t get_local_scale(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_local_scale(man, e);
	}

	void set_local_scale(tm_component_manager_o* manager, tm_entity_t e, tm_vec3_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_local_scale(man, e, v);
	}

	tm_vec4_t get_local_rotation(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_local_rotation(man, e);
	}

	void set_local_rotation(tm_component_manager_o* manager, tm_entity_t e, tm_vec4_t v) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_local_rotation(man, e, v);
	}

	const tm_transform_t* get_local_transform(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		return tm_transform_component_api->get_local_transform(man, e);
	}

	void set_local_transform(tm_component_manager_o* manager, tm_entity_t e, const tm_transform_t* t) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->set_local_transform(man, e, t);
	}

	void update_world_transform(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->update_world_transform(man, e);
	}

	void update_local_transform(tm_component_manager_o* manager, tm_entity_t e) {
		tm_transform_component_manager_o* man = (tm_transform_component_manager_o*)manager;
		tm_transform_component_api->update_local_transform(man, e);
	}

	//Tag
	void add_tag(tm_component_manager_o* manager, tm_entity_t e, tm_strhash_t tag) {
		tm_tag_component_manager_o* man = (tm_tag_component_manager_o*)manager;
		tm_tag_component_api->add_tag(man, e, tag);
	}

	void add_tag_optional(tm_component_manager_o* manager, tm_entity_t e, tm_strhash_t tag) {
		tm_tag_component_manager_o* man = (tm_tag_component_manager_o*)manager;
		tm_tag_component_api->add_tag_optional(man, e, tag);
	}

	void remove_tag(tm_component_manager_o* manager, tm_entity_t e, tm_strhash_t tag) {
		tm_tag_component_manager_o* man = (tm_tag_component_manager_o*)manager;
		tm_tag_component_api->remove_tag(man, e, tag);
	}

	bool has_tag(tm_component_manager_o* manager, tm_entity_t e, tm_strhash_t tag) {
		tm_tag_component_manager_o* man = (tm_tag_component_manager_o*)manager;
		return tm_tag_component_api->has_tag(man, e, tag);
	}

	tm_script_array_t* find_all(tm_component_manager_o* manager, tm_strhash_t tag) {
		TM_INIT_TEMP_ALLOCATOR(ta);
		tm_tag_component_manager_o* man = (tm_tag_component_manager_o*)manager;
		tm_entity_t* entities = tm_tag_component_api->find_all(man, tag, ta);
		uint64_t entity_count = tm_carray_size(entities);
		if (asIScriptContext* ctx = asGetActiveContext()) {
			asITypeInfo* type = ctx->GetEngine()->GetTypeInfoByDecl("tm_array_t<tm_entity_t>");
			tm_script_array_t* output_array = tm_array::create(type, 0, nullptr);
			for (uint32_t i = 0; i < entity_count; ++i) {
				tm_array::push(&entities[i], output_array);
			}
			TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
			return output_array;
		}
		return nullptr;
	}

	tm_entity_t find_first(tm_component_manager_o* manager, tm_strhash_t tag) {
		tm_tag_component_manager_o* man = (tm_tag_component_manager_o*)manager;
		return tm_tag_component_api->find_first(man, tag);
	}

	void register_component_interface(asIScriptEngine* engine) {
		AS_CHECK(engine->SetDefaultNamespace("tm_entity_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_component_type_t lookup_component_type(tm_entity_context_o@ ctx, tm_strhash_t hashed_name)", asFUNCTION(lookup_component_type), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_component_manager_o@ component_manager(tm_entity_context_o@ ctx, tm_component_type_t type)", asFUNCTION(component_manager), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_component_manager_o@ transform_component_manager(tm_entity_context_o@ ctx)", asFUNCTION(transform_component_manager), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_component_manager_o@ tag_component_manager(tm_entity_context_o@ ctx)", asFUNCTION(tag_component_manager), asCALL_CDECL));

		AS_CHECK(engine->SetDefaultNamespace("tm_transform_component_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t get_position(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_position), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_position(tm_component_manager_o@ manager, tm_entity_t e, tm_vec3_t v)", asFUNCTION(set_position), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t get_scale(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_scale), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_scale(tm_component_manager_o@ manager, tm_entity_t e, tm_vec3_t v)", asFUNCTION(set_scale), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec4_t get_rotation(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_rotation), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_rotation(tm_component_manager_o@ manager, tm_entity_t e, tm_vec4_t v)", asFUNCTION(set_rotation), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("const tm_transform_t@ get_transform(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_transform), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_transform(tm_component_manager_o@ manager, tm_entity_t e,const tm_transform_t@ t)", asFUNCTION(set_transform), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t get_local_position(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_local_position), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_local_position(tm_component_manager_o@ manager, tm_entity_t e, tm_vec3_t v)", asFUNCTION(set_local_position), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec3_t get_local_scale(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_local_scale), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_local_scale(tm_component_manager_o@ manager, tm_entity_t e, tm_vec3_t v)", asFUNCTION(set_local_scale), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_vec4_t get_local_rotation(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_local_rotation), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_local_rotation(tm_component_manager_o@ manager, tm_entity_t e, tm_vec4_t v)", asFUNCTION(set_local_rotation), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("const tm_transform_t@ get_local_transform(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(get_local_transform), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void set_local_transform(tm_component_manager_o@ manager, tm_entity_t e, const tm_transform_t@ t)", asFUNCTION(set_local_transform), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void update_world_transform(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(update_world_transform), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void update_local_transform(tm_component_manager_o@ manager, tm_entity_t e)", asFUNCTION(update_local_transform), asCALL_CDECL));

		AS_CHECK(engine->SetDefaultNamespace("tm_tag_component_api"));
		AS_CHECK(engine->RegisterGlobalFunction("void add_tag(tm_component_manager_o@ manager, tm_entity_t e, tm_strhash_t tag)", asFUNCTION(add_tag), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void add_tag_optional(tm_component_manager_o@ manager, tm_entity_t e, tm_strhash_t tag)", asFUNCTION(add_tag_optional), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void remove_tag(tm_component_manager_o@ manager, tm_entity_t e, tm_strhash_t tag)", asFUNCTION(remove_tag), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("bool has_tag(tm_component_manager_o@ manager, tm_entity_t e, tm_strhash_t tag)", asFUNCTION(has_tag), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_array_t<tm_entity_t>@ find_all(tm_component_manager_o@ manager, tm_strhash_t tag)", asFUNCTION(find_all), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("tm_entity_t find_first(tm_component_manager_o@ manager, tm_strhash_t tag)", asFUNCTION(find_first), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}