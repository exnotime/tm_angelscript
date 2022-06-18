#include "tm_as_script_component.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering script component, %s, %d", __FILE__, __LINE__);} } while(false)

extern "C" {
#include <foundation/log.h>
#include <plugins/entity/entity.h>
#include <foundation/carray.inl>
#include <foundation/hash.inl>

}

namespace tm_script_component {
	static tm_component_i* script_components;
	static tm_allocator_i* alloc;
	static struct TM_HASH_T(uint64_t, void*) default_data_map;

	//Script components will have the same name as the class passed in to the register function
	tm_component_type_t register_script_component(tm_entity_context_o* ctx, void* obj, int type_id) {
		asIScriptContext* as_ctx = asGetActiveContext();
		asIScriptEngine* engine = as_ctx->GetEngine();
		asITypeInfo* type = engine->GetTypeInfoById(type_id);
		tm_component_i script_comp = {};
		tm_component_i* comp = (tm_component_i*)tm_carray_push(script_components, script_comp, alloc);
		memset(comp, 0x0, sizeof(tm_component_i));
		comp->bytes = type->GetSize();
		comp->name = type->GetName();
		//TODO: Deallocate
		void* default_data = tm_alloc(alloc, comp->bytes);
		memcpy(default_data, obj, comp->bytes);
		comp->default_data = default_data;
		tm_hash_add(&default_data_map, (uint64_t)type_id, default_data);

		return tm_entity_api->register_component(ctx, comp);
	}

	//These could technically be used for any component but unless we expose every engine component to AS it is better to only use it for script components 
	void add_script_component(tm_entity_context_o* ctx, tm_entity_t e, tm_component_type_t t, void* obj, int type_id) {
		asIScriptEngine* engine = asGetActiveContext()->GetEngine();
		asITypeInfo* type = engine->GetTypeInfoById(type_id);
		void* data = tm_entity_api->add_component(ctx, e, t);
		engine->AssignScriptObject(obj, data, type);
	}

	void get_script_component(tm_entity_context_o* ctx, tm_entity_t e, tm_component_type_t t, void* obj, int type_id) {
		asIScriptEngine* engine = asGetActiveContext()->GetEngine();
		asITypeInfo* type = engine->GetTypeInfoById(type_id);
		const void* data = tm_entity_api->read_component(ctx, e, t);
		engine->AssignScriptObject(obj, const_cast<void*>(data), type);
	}

	void register_script_component_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {
		alloc = allocator;
		script_components = tm_carray_create(tm_component_i, 1, allocator);
		default_data_map = {};
		default_data_map.allocator = allocator;

		AS_CHECK(engine->SetDefaultNamespace("tm_entity_ext_api"));
		AS_CHECK(engine->RegisterGlobalFunction("tm_component_type_t register_script_component(tm_entity_context_o@ ctx, ?&in comp_class)", asFUNCTION(register_script_component), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void add_script_component(tm_entity_context_o@ ctx, tm_entity_t e, tm_component_type_t type, ?&out comp)", asFUNCTION(add_script_component), asCALL_CDECL));
		AS_CHECK(engine->RegisterGlobalFunction("void get_script_component(tm_entity_context_o@ ctx, tm_entity_t e, tm_component_type_t type, ?&out comp)", asFUNCTION(get_script_component), asCALL_CDECL));
		AS_CHECK(engine->SetDefaultNamespace(""));
	}
}