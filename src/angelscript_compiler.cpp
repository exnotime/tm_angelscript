#include "angelscript_compiler.h"

#include <angelscript.h>
#include <as_addons/scriptbuilder/scriptbuilder.h>
#include "tm_angelscript.h"

extern "C" {
#include <foundation/string.h>
#include <foundation/api_types.h>
#include <foundation/os.h>
#include <foundation/path.h>
#include <foundation/temp_allocator.h>
#include <foundation/string.h>
#include <foundation/log.h>
}


namespace as_compiler {
	static asIScriptEngine* _engine;

	struct IncludeHandler {
		tm_str_t project_path;
		tm_str_t main_path;
		tm_temp_allocator_i* ta;
	};

	struct ByteStream : public asIBinaryStream {
		ByteStream(const void* buffer, uint64_t sz) {
			_allocator = nullptr;
			_read_pos = 0;
			_write_pos = 0;
			sz = 0;
			_buffer = (char*)buffer;
		}
		ByteStream(tm_temp_allocator_i* a) {
			_allocator = a;
			_read_pos = 0;
			_write_pos = 0;
			_buffer_size = 0;
		}
		int Read(void* ptr, asUINT size) {
			if (!ptr || !size) {
				return -1;
			}
			memcpy(ptr, &_buffer[_read_pos], size);
			_read_pos += size;
			return 0;
		}
		int Write(const void* ptr, asUINT size) {
			if (!ptr || !size) {
				return -1;
			}
			if ((_buffer_size - _write_pos) < size) {
				char* new_buffer = (char*)tm_temp_alloc(_allocator, _buffer_size + size);
				memcpy(new_buffer, _buffer, _buffer_size);
				_buffer = new_buffer;
				_buffer_size = _buffer_size + size;
			}
			memcpy(&_buffer[_write_pos], ptr, size);
			_write_pos += size;
			return 0;

		}
		uint64_t _read_pos;
		uint64_t _write_pos;
		uint64_t _buffer_size;
		char* _buffer;
		tm_temp_allocator_i* _allocator;
	};

	int include_callback(const char* include, const char* from, CScriptBuilder* builder, void* userParam) {
		IncludeHandler* handler = (IncludeHandler*)userParam;
		tm_str_t abs_path = tm_path_api->join(handler->project_path, tm_str(include), handler->ta);
		tm_file_o abs_file = tm_os_api->file_io->open_input(abs_path.data);
		if (abs_file.valid) {
			uint64_t file_size = tm_os_api->file_io->size(abs_file);
			void* buffer = tm_temp_alloc(handler->ta, file_size);
			tm_os_api->file_io->read(abs_file, buffer, file_size);
			tm_os_api->file_io->close(abs_file);
			builder->AddSectionFromMemory(abs_path.data, (char*)buffer, (uint32_t)file_size);
			return 1;
		}
		
		tm_str_t rel_path = tm_path_api->join(tm_path_api->directory(tm_str(from)), tm_str(include), handler->ta);
		tm_str_t abs_rel_path = tm_path_api->join(handler->project_path, rel_path, handler->ta);
		tm_file_o rel_file = tm_os_api->file_io->open_input(abs_rel_path.data);
		if (rel_file.valid) {
			uint64_t file_size = tm_os_api->file_io->size(rel_file);
			void* buffer = tm_temp_alloc(handler->ta, file_size);
			tm_os_api->file_io->read(rel_file, buffer, file_size);
			tm_os_api->file_io->close(rel_file);
			builder->AddSectionFromMemory(abs_rel_path.data, (char*)buffer, (uint32_t)file_size);
			return 1;
		}
		return 0;
	}

	void initialize(asIScriptEngine* engine) {
		_engine = engine;
		engine->SetEngineProperty(asEP_USE_CHARACTER_LITERALS, true); //to have '' mean a single char
		//engine->SetEngineProperty(asEP_INIT_GLOBAL_VARS_AFTER_BUILD, true); //dont allocate memory for global variables, something you would have if you set up a compiler environment without any real engine functions bound
	}

	uint64_t compile_file_to_bytecode(asIScriptEngine* engine, const char* project_root, const char* main_file, const char* module_name, void** out_bytecode, tm_temp_allocator_i* ta) {
		CScriptBuilder builder;
		IncludeHandler handler;
		handler.main_path = tm_str(main_file);
		handler.project_path = tm_str(project_root);
		handler.ta = ta;
		tm_str_t start_path = tm_path_api->join(handler.project_path, handler.main_path, ta);
		tm_file_o fh = tm_os_api->file_io->open_input(start_path.data);
		if (!fh.valid) {
			return 0;
		}
		uint64_t file_size = tm_os_api->file_io->size(fh);
		char* file_buffer = (char*)tm_temp_alloc(ta, file_size);
		tm_os_api->file_io->read(fh, file_buffer, file_size);
		tm_os_api->file_io->close(fh);
		builder.SetIncludeCallback(include_callback, &handler);
		builder.StartNewModule(engine, module_name);
		builder.AddSectionFromMemory(handler.main_path.data, file_buffer, (uint32_t)file_size);
		int ret = builder.BuildModule();
		if (ret >= 0) {
			//TODO: Verboose logging file
			tm_logger_api->print(tm_log_type::TM_LOG_TYPE_INFO, "Compiled successfully!");
			asIScriptModule* mod = builder.GetModule();
			ByteStream bs(ta);
			mod->SaveByteCode(&bs);
			*out_bytecode = bs._buffer;
			return bs._buffer_size;
		}
		return 0;
	}

	asIScriptModule* get_module_from_bytecode(const char* name, uint64_t size, const void* bc) {
		ByteStream bs(bc, size);
		asIScriptModule* mod = _engine->GetModule(name, asEGMFlags::asGM_ALWAYS_CREATE);
		bool debug;
		mod->LoadByteCode(&bs, &debug);
		//TODO: Add logging
		return mod;
	}

	void compile_string_to_bytecode(const char* filename) {

	}
}