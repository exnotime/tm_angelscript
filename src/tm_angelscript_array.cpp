#include "tm_angelscript_array.h"
#include "tm_angelscript.h"
#include <angelscript.h>
#include <new>
#define AS_CHECK(x) do{if(x < 0) { tm_logger_api->printf(TM_LOG_TYPE_INFO, "Failed registering something Angelscript, %d", __LINE__);} } while(false)

extern "C" {
#include <foundation/allocator.h>
#include <foundation/error.h>
#include <foundation/log.h>
#include <foundation/carray.inl>

}

namespace tm_array {
#define tm_carray_create_with_size(p, s, n, allocator) (tm_carray_create_internal(p, n, s, allocator, __FILE__, __LINE__))
#define tm_carray_free_with_size(a, s, allocator)((*(void **)&(a)) = tm_carray_set_capacity_internal((void *)a, 0, s, allocator, __FILE__, __LINE__))

	static tm_allocator_i* _allocator = nullptr;
	
	tm_script_array_t* create(asITypeInfo* ot, asUINT length, void* defaultValue) {
		tm_script_array_t* sa = (tm_script_array_t*)tm_alloc(_allocator, sizeof(tm_script_array_t));
		memset(sa, 0x0, sizeof(tm_script_array_t));
		sa->sub_type_id = ot->GetSubTypeId();
		// Determine element size
		if (sa->sub_type_id & asTYPEID_MASK_OBJECT)
			sa->element_size = sizeof(asPWORD);
		else
			sa->element_size = ot->GetEngine()->GetSizeOfPrimitiveType(sa->sub_type_id);
		if (length == 0) {
			//tm_carray_create crashes with an empty array as input
			//Create it with 1 element instead and set the header to say 0
			sa->array = (char*)tm_carray_create_with_size(sa->array, sa->element_size, 1, _allocator);
			tm_carray_header(sa->array)->size = 0;
		} else {
			sa->array = (char*)tm_carray_create_with_size(sa->array, sa->element_size, length, _allocator);
		}
		sa->sub_type_id = ot->GetSubTypeId();
		sa->type = ot;
		ot->AddRef();
		sa->ref_count = 1;

		if (ot->GetFlags() & asOBJ_GC)
			ot->GetEngine()->NotifyGarbageCollectorOfNewObject(sa, ot);
		// Initialize the elements with the default value
		if (defaultValue) {
			for (uint32_t n = 0; n < length; n++) {
				memcpy((char*)sa->array + n * sa->element_size, defaultValue, sa->element_size);
			}
		}
		return sa;
	}

	tm_script_array_t* create(asITypeInfo* ot) {
		return create(ot, 0, nullptr);
	}

	tm_script_array_t* create(asITypeInfo* ot, asUINT length) {
		return create(ot, length, nullptr);
	}

	tm_script_array_t* create_list(asITypeInfo* ot, void* buf) {
		tm_script_array_t* sa = (tm_script_array_t*)tm_alloc(_allocator, sizeof(tm_script_array_t));
		memset(sa, 0x0, sizeof(tm_script_array_t));
		sa->sub_type_id = ot->GetSubTypeId();
		// Determine element size
		if (sa->sub_type_id & asTYPEID_MASK_OBJECT)
			sa->element_size = sizeof(asPWORD);
		else
			sa->element_size = ot->GetEngine()->GetSizeOfPrimitiveType(sa->sub_type_id);

		asUINT length = *(asUINT*)buf;

		sa->array = (char*)tm_carray_create_with_size(sa->array, sa->element_size, length, _allocator);
		sa->sub_type_id = ot->GetSubTypeId();
		sa->type = ot;
		ot->AddRef();
		sa->ref_count = 1;

		// Copy the values of the array elements from the buffer
		if ((ot->GetSubTypeId() & asTYPEID_MASK_OBJECT) == 0)
		{
			// Copy the values of the primitive type into the internal buffer
			if (length > 0) {
				memcpy(sa->array, (((asUINT*)buf) + 1), length * sa->element_size);
			}
		} else if (ot->GetSubTypeId() & asTYPEID_OBJHANDLE) {

			// Copy the handles into the internal buffer
			if (length > 0)
				memcpy(sa->array, (((asUINT*)buf) + 1), length * sa->element_size);

			// With object handles it is safe to clear the memory in the received buffer
			// instead of increasing the ref count. It will save time both by avoiding the
			// call the increase ref, and also relieve the engine from having to release
			// its references too
			memset((((asUINT*)buf) + 1), 0, length * sa->element_size);
		} else if (ot->GetSubType()->GetFlags() & asOBJ_REF){
			// Only allocate the buffer, but not the objects
			sa->sub_type_id |= asTYPEID_OBJHANDLE;
			sa->sub_type_id &= ~asTYPEID_OBJHANDLE;

			// Copy the handles into the internal buffer
			if (length > 0)
				memcpy(sa->array, (((asUINT*)buf) + 1), length * sa->element_size);

			// For ref types we can do the same as for handles, as they are
			// implicitly stored as handles.
			memset((((asUINT*)buf) + 1), 0, length * sa->element_size);
		} else {
			// TODO: Optimize by calling the copy constructor of the object instead of
			//       constructing with the default constructor and then assigning the value
			// TODO: With C++11 ideally we should be calling the move constructor, instead
			//       of the copy constructor as the engine will just discard the objects in the
			//       buffer afterwards.

			// For value types we need to call the opAssign for each individual object
			for (asUINT n = 0; n < length; n++)
			{
				void* obj = (void*)(((char*)sa->array) + sa->element_size * n);

				asBYTE* srcObj = (asBYTE*)buf;
				srcObj += 4 + n * ot->GetSubType()->GetSize();
				ot->GetEngine()->AssignScriptObject(obj, srcObj, ot->GetSubType());
			}
		}

		if (ot->GetFlags() & asOBJ_GC)
			ot->GetEngine()->NotifyGarbageCollectorOfNewObject(sa, ot);

		return sa;
	}

	void destroy(tm_script_array_t* sa) {
		tm_carray_free_with_size(sa->array, sa->element_size, _allocator);
		tm_free(_allocator, sa, sizeof(tm_script_array_t));
	}

	// We just define a number here that we assume nobody else is using for
	// object type user data. The add-ons have reserved the numbers 1000
	// through 1999 for this purpose, so we should be fine.
	const asPWORD ARRAY_CACHE = 1000;

	struct tm_array_cache_t {
		asIScriptFunction* cmp_func;
		asIScriptFunction* equal_func;
		int32_t cmp_return_code; // To allow better error message in case of multiple matches
		int32_t equal_return_code;
		uint32_t size;
	};

	static void cleanup_typeinfo_array_cache(asITypeInfo* type) {
		tm_array_cache_t* cache = reinterpret_cast<tm_array_cache_t*>(type->GetUserData(ARRAY_CACHE));
		if (cache) {
			tm_free(_allocator, cache, cache->size);
		}
	}

	//Taken from the default script array addon
	// This optional callback is called when the template type is first used by the compiler.
	// It allows the application to validate if the template can be instantiated for the requested
	// subtype at compile time, instead of at runtime. The output argument dontGarbageCollect
	// allow the callback to tell the engine if the template instance type shouldn't be garbage collected,
	// i.e. no asOBJ_GC flag.
	static bool script_array_template_callback(asITypeInfo* ti, bool& dontGarbageCollect)
	{
		// Make sure the subtype can be instantiated with a default factory/constructor,
		// otherwise we won't be able to instantiate the elements.
		int typeId = ti->GetSubTypeId();
		if (typeId == asTYPEID_VOID)
			return false;
		if ((typeId & asTYPEID_MASK_OBJECT) && !(typeId & asTYPEID_OBJHANDLE))
		{
			asITypeInfo* subtype = ti->GetEngine()->GetTypeInfoById(typeId);
			asDWORD flags = subtype->GetFlags();
			if ((flags & asOBJ_VALUE) && !(flags & asOBJ_POD))
			{
				// Verify that there is a default constructor
				bool found = false;
				for (asUINT n = 0; n < subtype->GetBehaviourCount(); n++)
				{
					asEBehaviours beh;
					asIScriptFunction* func = subtype->GetBehaviourByIndex(n, &beh);
					if (beh != asBEHAVE_CONSTRUCT) continue;

					if (func->GetParamCount() == 0)
					{
						// Found the default constructor
						found = true;
						break;
					}
				}

				if (!found)
				{
					// There is no default constructor
					// TODO: Should format the message to give the name of the subtype for better understanding
					ti->GetEngine()->WriteMessage("array", 0, 0, asMSGTYPE_ERROR, "The subtype has no default constructor");
					return false;
				}
			} else if ((flags & asOBJ_REF))
			{
				bool found = false;

				// If value assignment for ref type has been disabled then the array
				// can be created if the type has a default factory function
				if (!ti->GetEngine()->GetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE))
				{
					// Verify that there is a default factory
					for (asUINT n = 0; n < subtype->GetFactoryCount(); n++)
					{
						asIScriptFunction* func = subtype->GetFactoryByIndex(n);
						if (func->GetParamCount() == 0)
						{
							// Found the default factory
							found = true;
							break;
						}
					}
				}

				if (!found)
				{
					// No default factory
					// TODO: Should format the message to give the name of the subtype for better understanding
					ti->GetEngine()->WriteMessage("array", 0, 0, asMSGTYPE_ERROR, "The subtype has no default factory");
					return false;
				}
			}

			// If the object type is not garbage collected then the array also doesn't need to be
			if (!(flags & asOBJ_GC))
				dontGarbageCollect = true;
		} else if (!(typeId & asTYPEID_OBJHANDLE))
		{
			// Arrays with primitives cannot form circular references,
			// thus there is no need to garbage collect them
			dontGarbageCollect = true;
		} else
		{

			// It is not necessary to set the array as garbage collected for all handle types.
			// If it is possible to determine that the handle cannot refer to an object type
			// that can potentially form a circular reference with the array then it is not 
			// necessary to make the array garbage collected.
			asITypeInfo* subtype = ti->GetEngine()->GetTypeInfoById(typeId);
			asDWORD flags = subtype->GetFlags();
			if (!(flags & asOBJ_GC))
			{
				if ((flags & asOBJ_SCRIPT_OBJECT))
				{
					// Even if a script class is by itself not garbage collected, it is possible
					// that classes that derive from it may be, so it is not possible to know 
					// that no circular reference can occur.
					if ((flags & asOBJ_NOINHERIT))
					{
						// A script class declared as final cannot be inherited from, thus
						// we can be certain that the object cannot be garbage collected.
						dontGarbageCollect = true;
					}
				} else
				{
					// For application registered classes we assume the application knows
					// what it is doing and don't mark the array as garbage collected unless
					// the type is also garbage collected.
					dontGarbageCollect = true;
				}
			}
		}

		// The type is ok
		return true;
	}

	void add_ref(tm_script_array_t* sa) {
		sa->ref_count++;
	}

	void release(tm_script_array_t* sa) {
		sa->ref_count--;
		if (sa->ref_count == 0) {
			destroy(sa);
		}
	}

	void* index_operator(uint32_t i, tm_script_array_t* sa) {
		return (sa->array + sa->element_size * i);
	}

	tm_script_array_t* assign_operator(const tm_script_array_t* other, tm_script_array_t* sa) {
		if(sa->element_size == other->element_size){
			if(tm_carray_size(other->array) > 0){
				tm_carray_resize(sa->array, other->element_size, _allocator);
				memcpy(sa->array, other->array, tm_carray_size(other->array) * other->element_size);
			} else {
				tm_carray_shrink(sa->array, 0);
			}
		}
		return sa;
	}

	void insert_element_at(asUINT i, void* obj, tm_script_array_t* sa) {
		uint32_t current_size = (uint32_t)tm_carray_size(sa->array);
		if (i < current_size) {
			//Resize to fit the new object
			tm_carray_ensure(sa->array, current_size + 1, _allocator);
			//Move everything above i
			memcpy(sa->array + sa->element_size * i + 1, sa->array + sa->element_size * i, sa->element_size * (current_size - i));
			//Copy in the new element
			memcpy(sa->array + sa->element_size * i, obj, sa->element_size);
			tm_carray_header(sa->array)->size++;
		} else {
			asIScriptContext* ctx = asGetActiveContext();
			if (ctx) {
				ctx->SetException("Index out of bounds");
			}
			return;
		}
	}

	void insert_array_at(asUINT i, tm_script_array_t* other, tm_script_array_t* sa) {
		uint32_t current_size = (uint32_t)tm_carray_size(sa->array);
		if (i < current_size) {
			uint32_t other_size = (uint32_t)tm_carray_size(other);
			if (other_size > 0) {
				//Resize to fit the new objects
				tm_carray_ensure(sa->array, current_size + other_size, _allocator);
				//Move everything above i
				memcpy(sa->array + sa->element_size * i + other_size * sa->element_size, sa->array + sa->element_size * i, sa->element_size * (current_size - i));
				//Copy in the new element
				memcpy(sa->array + sa->element_size * i, other->array, sa->element_size * other_size);
				//write new size
				tm_carray_header(sa->array)->size += other_size;
			}
		} else {
			asIScriptContext* ctx = asGetActiveContext();
			if (ctx) {
				ctx->SetException("Index out of bounds");
			}
			return;
		}
	}

	void push(void* obj, tm_script_array_t* sa) {
		uint32_t current_size = (uint32_t)tm_carray_size(sa->array);
		tm_carray_ensure(sa->array, current_size + 1, _allocator);
		memcpy(sa->array + current_size * sa->element_size, obj, sa->element_size);
		tm_carray_header(sa->array)->size++;
	}

	void erase(uint32_t i, tm_script_array_t* sa) {
		uint32_t current_size = (uint32_t)tm_carray_size(sa->array);
		if (i < current_size) {
			//Move everything above i
			memcpy(sa->array + sa->element_size * i, sa->array + sa->element_size * i + 1, sa->element_size * (current_size - i));
			tm_carray_header(sa->array)->size--;
		} else {
			asIScriptContext* ctx = asGetActiveContext();
			if (ctx) {
				ctx->SetException("Index out of bounds");
			}
			return;
		}
	}

	void* pop(void* obj, tm_script_array_t* sa) {
		uint32_t current_size = (uint32_t)tm_carray_size(sa->array);
		void* ret = sa->array + sa->element_size * current_size - 1;
		tm_carray_shrink(sa->array, current_size - 1);
		return ret;
	}

	void erase_range(uint32_t i, uint32_t count, tm_script_array_t* sa) {
		uint32_t current_size = (uint32_t)tm_carray_size(sa->array);
		if (i + count < current_size) {
			//Move everything above i
			memcpy(sa->array + sa->element_size * i, sa->array + sa->element_size * i + count, sa->element_size * (current_size - i));
			tm_carray_header(sa->array)->size -= count;
		} else {
			asIScriptContext* ctx = asGetActiveContext();
			if (ctx) {
				ctx->SetException("Index or range out of bounds");
			}
			return;
		}
	}

	uint32_t size(tm_script_array_t* sa) {
		return (uint32_t)tm_carray_size(sa->array);
	}

	bool is_empty(tm_script_array_t* sa) {
		return (uint32_t)tm_carray_size(sa->array) == 0;
	}

	void resize(uint32_t count, tm_script_array_t* sa) {
		tm_carray_resize(sa->array, count, _allocator);
	}

	uint32_t get_ref_count(tm_script_array_t* sa) {
		return sa->ref_count;
	}

	void get_flag(tm_script_array_t* sa) {
		sa->gc = true;
	}

	void set_flag(tm_script_array_t* sa) {
		sa->gc = true;
	}

	void enum_refs(asIScriptEngine* engine, tm_script_array_t* sa) {
		if (sa->sub_type_id & asTYPEID_MASK_OBJECT)
		{
			void** d = (void**)sa->array;

			asITypeInfo* subType = engine->GetTypeInfoById(sa->sub_type_id);
			if ((subType->GetFlags() & asOBJ_REF))
			{
				// For reference types we need to notify the GC of each instance
				uint32_t size = (uint32_t)tm_carray_size(sa->array);
				for (asUINT n = 0; n < size; n++)
				{
					if (d[n])
						engine->GCEnumCallback(d[n]);
				}
			} else if ((subType->GetFlags() & asOBJ_VALUE) && (subType->GetFlags() & asOBJ_GC))
			{
				// For value types we need to forward the enum callback
				// to the object so it can decide what to do
				uint32_t size = (uint32_t)tm_carray_size(sa->array);
				for (asUINT n = 0; n < size; n++)
				{
					if (d[n])
						engine->ForwardGCEnumReferences(d[n], subType);
				}
			}
		}
	}

	void release_everything(asIScriptEngine* engine, tm_script_array_t* sa) {
		resize(0, sa);
	}

	void register_tm_array(asIScriptEngine* engine, tm_allocator_i* allocator) {
		_allocator = allocator;
		int r = 0;
		engine->SetTypeInfoUserDataCleanupCallback(cleanup_typeinfo_array_cache, ARRAY_CACHE);
		r = engine->RegisterObjectType("tm_array<class T>", 0, asOBJ_REF | asOBJ_GC | asOBJ_TEMPLATE);
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_TEMPLATE_CALLBACK, "bool f(int&in, bool&out)", asFUNCTION(script_array_template_callback), asCALL_CDECL);
		// Templates receive the object type as the first parameter. To the script writer this is hidden
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_FACTORY, "tm_array<T>@ f(int&in)", asFUNCTIONPR(create, (asITypeInfo*), tm_script_array_t*), asCALL_CDECL);
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_FACTORY, "tm_array<T>@ f(int&in, uint length) explicit", asFUNCTIONPR(create, (asITypeInfo*, asUINT), tm_script_array_t*), asCALL_CDECL);
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_FACTORY, "tm_array<T>@ f(int&in, uint length, const T &in value)", asFUNCTIONPR(create, (asITypeInfo*, asUINT, void*), tm_script_array_t*), asCALL_CDECL);
		// Register the factory that will be used for initialization lists
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_LIST_FACTORY, "tm_array<T>@ f(int&in type, int&in list) {repeat T}", asFUNCTIONPR(create_list, (asITypeInfo*, void*), tm_script_array_t*), asCALL_CDECL);
		// The memory management methods
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_ADDREF, "void f()", asFUNCTION(add_ref), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_RELEASE, "void f()", asFUNCTION(release), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "T &opIndex(uint index)", asFUNCTION(index_operator), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "const T &opIndex(uint index) const", asFUNCTION(index_operator), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "tm_array<T> &opAssign(const tm_array<T>&in)", asFUNCTION(assign_operator), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "void insert_at(uint index, const T&in value)", asFUNCTION(insert_element_at), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "void insert_at(uint index, const tm_array<T>& value)", asFUNCTION(insert_array_at), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "void push(const T&in value)", asFUNCTION(push), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "void erase(uint index)", asFUNCTION(erase), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "T& pop()", asFUNCTION(pop), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "void erase_range(uint start, uint count)", asFUNCTION(erase_range), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "uint size()", asFUNCTION(size), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "bool is_empty()", asFUNCTION(is_empty), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectMethod("tm_array<T>", "void resize(uint size)", asFUNCTION(resize), asCALL_CDECL_OBJLAST);

		// Register GC behaviours in case the array needs to be garbage collected
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_GETREFCOUNT, "int f()", asFUNCTION(get_ref_count), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_SETGCFLAG, "void f()", asFUNCTION(set_flag), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_GETGCFLAG, "bool f()", asFUNCTION(get_flag), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_ENUMREFS, "void f(int&in)", asFUNCTION(enum_refs), asCALL_CDECL_OBJLAST);
		r = engine->RegisterObjectBehaviour("tm_array<T>", asBEHAVE_RELEASEREFS, "void f(int&in)", asFUNCTION(release_everything), asCALL_CDECL_OBJLAST);

		r = engine->RegisterDefaultArrayType("tm_array<T>");
	}
}