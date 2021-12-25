#include "tm_as_string.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include <new>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering something Angelscript, %d", __LINE__);} } while(false)

extern "C" {
#include <plugins/entity/entity.h>
#include <foundation/log.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_assets.h>
#include <foundation/allocator.h>
#include <foundation/error.h>
#include <foundation/murmurhash64a.inl>
#include <foundation/hash.inl>
#include <foundation/carray.inl>
#include <foundation/string.h>
#include <foundation/string.inl>
}


namespace tm_string {

	static tm_allocator_i* _allocator = nullptr;

	struct tm_string_factory : public asIStringFactory {
		void init(tm_allocator_i* a) {
			_string_map = {};
			_string_map.allocator = a;
			_ref_map = {};
			_ref_map.allocator = a;
			tm_hash_clear(&_string_map);
			tm_hash_clear(&_ref_map);
		}

		void destroy() {
			tm_hash_clear(&_string_map);
			tm_hash_clear(&_ref_map);
		}

		const void* GetStringConstant(const char* data, asUINT length) {
			asAcquireExclusiveLock();
			uint64_t key = tm_murmur_hash_64a(data, length, 0xBEEFC0DE);
			if (tm_hash_has(&_string_map, key)) {
				uint32_t& ref = tm_hash_get(&_ref_map, key);
				ref++;
				asReleaseExclusiveLock();
				return reinterpret_cast<const void*>(tm_hash_get(&_string_map, key));
			} else {
				tm_str_t* str = (tm_str_t*)tm_alloc(_string_map.allocator, sizeof(tm_str_t));
				//Copy string
				str->data = (char*)tm_alloc(_string_map.allocator, length + 1);
				memcpy((void*)str->data, data, length);
				((char*)(str->data))[length] = 0;
				str->size = length;
				str->null_terminated = true;

				tm_hash_add(&_ref_map, key, 1U);
				tm_hash_add(&_string_map, key, str);
				asReleaseExclusiveLock();
				return reinterpret_cast<const void*>(tm_hash_get(&_string_map, key));
			}
		}

		int ReleaseStringConstant(const void* key_ptr) {
			if (key_ptr == 0)
				return asERROR;

			int ret = asSUCCESS;
			asAcquireExclusiveLock();

			tm_str_t* str = (tm_str_t*)key_ptr;
			uint64_t key = tm_murmur_hash_64a(str->data, str->size, 0xBEEFC0DE);
			if (tm_hash_has(&_string_map, key)) {
				uint32_t& ref = tm_hash_get(&_ref_map, key);
				ref--;
				if (ref == 0) {
					tm_free(_string_map.allocator, (void*)str->data, str->size + 1);
					tm_free(_string_map.allocator, (void*)str, sizeof(tm_str_t));
					tm_hash_remove(&_string_map, key);
					tm_hash_remove(&_ref_map, key);
				}
			} else {
				ret = asERROR;
			}
			asReleaseExclusiveLock();
			return ret;
		}

		int GetRawStringData(const void* key_ptr, char* data, asUINT* length) const {
			if (key_ptr == 0)
				return asERROR;

			tm_str_t* str = (tm_str_t*)key_ptr;
			uint64_t key = tm_murmur_hash_64a(str->data, str->size, 0xBEEFC0DE);
			if (tm_hash_has(&_string_map, key)) {
				if (length)
					*length = str->size;

				if (data)
					memcpy(data, str->data, str->size);

			} else {
				return asERROR;
			}
			return asSUCCESS;
		}

		struct TM_HASH_T(uint64_t, tm_str_t*) _string_map;
		struct TM_HASH_T(uint64_t, uint32_t) _ref_map;
	};

	static tm_string_factory* _string_factory = nullptr;

	static void tm_construct_string(tm_str_t* thisPointer)
	{
		new(thisPointer) tm_str_t;
		thisPointer->data = nullptr;
		thisPointer->null_terminated = false;
		thisPointer->size = 0;
	}

	static void tm_copy_construct_string(const tm_str_t& other_str, tm_str_t* thisPointer)
	{
		char* s = (char*)tm_alloc(_allocator, other_str.size + 1);
		memcpy(s, other_str.data, other_str.size);
		s[other_str.size] = 0;
		*thisPointer = TM_LITERAL(tm_str_t) { s, other_str.size, true };
	}

	static void tm_destruct_string(tm_str_t* thisPointer)
	{
		tm_free(_allocator, (void*)thisPointer->data, thisPointer->size + 1);
	}

	tm_str_t create_string(const char* str) 
	{
		size_t length = strlen(str);
		char* s = (char*)tm_alloc(_allocator, length + 1);
		memcpy(s, str, length);
		s[length] = 0;
		return TM_LITERAL(tm_str_t) { s, (uint32_t)length, true };
	}

	void tm_string_op_assign(tm_str_t& a_str, const tm_str_t& b_str) {
		char* s = (char*)tm_alloc(_allocator, b_str.size + 1);
		memcpy(s, b_str.data, b_str.size);
		s[b_str.size] = 0;
		a_str = TM_LITERAL(tm_str_t) { s, b_str.size, true };
	}

	tm_str_t& tm_add_assign_string_to_string(const tm_str_t& other_str, tm_str_t& dest) {
		char* s = (char*)tm_alloc(_allocator, other_str.size + dest.size + 1);
		memcpy(s, dest.data, dest.size);
		memcpy(s + dest.size, other_str.data, other_str.size);
		s[dest.size + other_str.size] = 0;
		tm_free(_allocator, (void*)dest.data, dest.size);
		dest = TM_LITERAL(tm_str_t) { s, dest.size + other_str.size, true };
		
		return dest;
	}

	tm_str_t tm_add_string_to_string(const tm_str_t& a_str, tm_str_t& b_str) {
		char* s = (char*)tm_alloc(_allocator, a_str.size + b_str.size + 1);
		memcpy(s, a_str.data, a_str.size);
		memcpy(s + a_str.size, b_str.data, b_str.size);
		s[a_str.size + b_str.size] = 0;
		return TM_LITERAL(tm_str_t) { s, a_str.size + b_str.size, true };
	}

	bool tm_string_is_empty(const tm_str_t& str) {
		return str.size == 0;
	}

	char* tm_string_char_at(const uint32_t i, tm_str_t& str) {
		if (i >= str.size) {
			asIScriptContext* ctx = asGetActiveContext();
			ctx->SetException("Out of range");
			return 0;
		}
		return (char*)&str.data[i];
	}

	int tm_string_compare(const tm_str_t& a, const tm_str_t& b) {
		return strcmp(a.data, b.data);
	}

	void tm_string_insert(uint32_t pos, const tm_str_t& other, tm_str_t& str) {
		char* s = (char*)tm_alloc(_allocator, str.size + other.size + 1);
		memcpy(s, str.data, pos);
		memcpy(s + pos, other.data, other.size);
		memcpy(s + pos + other.size, &str.data[pos], str.size - pos);
		s[other.size + str.size] = 0;
		tm_free(_allocator, (void*)str.data, str.size);
		str = TM_LITERAL(tm_str_t) { s, other.size + str.size, true };
	}

	void tm_string_erase(uint32_t pos, uint32_t count, tm_str_t& str) {
		char* s = (char*)tm_alloc(_allocator, str.size - count + 1);
		memcpy(s, str.data, pos);
		memcpy(s + pos + count, &str.data[pos + count], str.size - count - pos);
		s[str.size - count] = 0;
		tm_free(_allocator, (void*)str.data, str.size);
		str = TM_LITERAL(tm_str_t) { s, str.size - count, true };
	}

	uint32_t tm_string_size(const tm_str_t& str) {
		return str.size;
	}

	void tm_string_resize(uint32_t size, tm_str_t& str) {
		char* s = (char*)tm_alloc(_allocator, size + 1);
		if (size < str.size) {
			memcpy(s, str.data, size);
		} else {
			//Copy portion of string
			memcpy(s, str.data, str.size);
			//then set the rest to spaces
			memset(s + str.size, ' ', size - str.size);
		}
		s[size] = 0;
		str = TM_LITERAL(tm_str_t) { s, size, true };
	}

	tm_str_t tm_substring(uint32_t pos, uint32_t count, tm_str_t& str) {
		char* s = (char*)tm_alloc(_allocator, count + 1);
		memcpy(s, &str.data[pos], count);
		s[count] = 0;
		return TM_LITERAL(tm_str_t) { s, count, true };
	}

	bool tm_string_equals(const  tm_str_t& a_str, const  tm_str_t& b_str) {
		return tm_strequal(a_str, b_str);
	}

	void tm_print_string(const tm_str_t& s) {
		tm_logger_api->print(tm_log_type::TM_LOG_TYPE_DEBUG, s.data);
	}

	//TODO: Lookup if this is the same as TM_STATIC_HASH
	tm_strhash_t hash_string(const tm_str_t& s) {
		return tm_murmur_hash_str(s);
	}

	//To get up and running quickly I have omitted the formatting functions (to uint/float etc) and find functions

	void register_tm_string_interface(asIScriptEngine* engine, tm_allocator_i* allocator) {
		//Is the string factory needed when we are not compiling?
		_allocator = allocator;
		_string_factory = (tm_string_factory*)tm_alloc(allocator, sizeof(tm_string_factory));
		_string_factory = new(_string_factory) tm_string_factory();
		_string_factory->init(allocator);
		

		int r = engine->RegisterObjectType("tm_str_t", sizeof(tm_str_t), asOBJ_VALUE | asGetTypeTraits<tm_str_t>());
		r = engine->RegisterStringFactory("tm_str_t", _string_factory);
		r = engine->RegisterObjectBehaviour("tm_str_t", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(tm_construct_string), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectBehaviour("tm_str_t", asBEHAVE_CONSTRUCT, "void f(const tm_str_t &in)", asFUNCTION(tm_copy_construct_string), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectBehaviour("tm_str_t", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(tm_destruct_string), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_str_t", "tm_str_t &opAssign(const tm_str_t &in)", asFUNCTION(tm_string_op_assign), asCALL_CDECL_OBJFIRST);
		r = engine->RegisterObjectMethod("tm_str_t", "tm_str_t &opAddAssign(const tm_str_t &in)", asFUNCTION(tm_add_assign_string_to_string), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_str_t", "bool opEquals(const tm_str_t &in) const", asFUNCTION(tm_string_equals), asCALL_CDECL_OBJFIRST);
		r = engine->RegisterObjectMethod("tm_str_t", "int opCmp(const tm_str_t &in) const", asFUNCTION(tm_string_compare), asCALL_CDECL_OBJFIRST);
		r = engine->RegisterObjectMethod("tm_str_t", "tm_str_t opAdd(const tm_str_t &in) const", asFUNCTION(tm_add_string_to_string), asCALL_CDECL_OBJFIRST);
		r = engine->RegisterObjectMethod("tm_str_t", "uint size() const", asFUNCTION(tm_string_size), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_str_t", "void resize(uint)", asFUNCTION(tm_string_resize), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_str_t", "bool is_empty() const", asFUNCTION(tm_string_is_empty), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_str_t", "uint8 &opIndex(uint)", asFUNCTION(tm_string_char_at), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_str_t", "const uint8 &opIndex(uint) const", asFUNCTION(tm_string_char_at), asCALL_CDECL_OBJLAST);

		r = engine->RegisterObjectMethod("tm_str_t", "tm_str_t substr(uint start = 0, int count = -1) const", asFUNCTION(tm_substring), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_str_t", "void insert(uint pos, const tm_str_t &in other)", asFUNCTION(tm_string_insert), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_str_t", "void erase(uint pos, int count = -1)", asFUNCTION(tm_string_erase), asCALL_CDECL_OBJLAST);

		r = engine->RegisterGlobalFunction("void print(const tm_str_t &in s)", asFUNCTION(tm_print_string), asCALL_CDECL);

		engine->RegisterTypedef("tm_strhash_t", "uint64");
		r = engine->RegisterGlobalFunction("tm_strhash_t hash_string(const tm_str_t &in s)", asFUNCTION(tm_print_string), asCALL_CDECL);
	}

	void destroy_string_factory(tm_allocator_i* allocator) {
		if (_string_factory) {
			_string_factory->destroy();
			_string_factory->~tm_string_factory();
			tm_free(allocator, _string_factory, sizeof(tm_string_factory));
			_string_factory = nullptr;
		}
	}
}