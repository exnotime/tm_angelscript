#include "tm_angelscript_asset.h"
#include <string.h>
#include "tm_angelscript.h"
extern "C" {
#include <foundation/api_types.h>
#include <foundation/asset_io.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_assets.h>
#include <foundation/the_truth_types.h>
#include <foundation/json.h>
#include <foundation/config.h>
#include <foundation/allocator.h>
#include <foundation/buffer.h>
#include <foundation/buffer_format.h>
#include <foundation/sprintf.h>
#include <foundation/temp_allocator.h>
#include <foundation/carray_print.inl>
#include <foundation/localizer.h>
#include <foundation/string.h>
#include <foundation/os.h>
#include <foundation/log.h>
#include <foundation/path.h>

}


#include "../as_addons/scriptbuilder/scriptbuilder.h"

static asIScriptEngine* script_engine = nullptr;

#define TM_TT_TYPE_HASH__SCRIPT_PROJECT TM_STATIC_HASH("script_project", 0x71cd8709a0f4faa1ULL)
#define TM_TT_PROP_HASH_SCRIPT_MODULE TM_STATIC_HASH("script_module", 0x144d369e3a49eba2ULL)
#define TM_TT_PROP_HASH_BYTECODE TM_STATIC_HASH("bytecode", 0xfacd48b3edfe8a37ULL)
#define TM_SCRIPT_PROJECT_MAIN_FIELD TM_STATIC_HASH("main", 0x293bb3b1982c6d5aULL)

enum {
	TM_TT_PROP__SCRIPT_MODULE__DATA
};


static bool enabled(struct tm_asset_io_o* inst)
{
	return true;
}

static bool can_import(struct tm_asset_io_o* inst, const char* extension)
{
	//Angelscript code is compiled from a single source file that get compiled into a module.
	//The project file exposes the main file and what definitions to set
	bool supported = strcmp(extension, "script_project") == 0;
	return supported;
}

static bool can_reimport(struct tm_asset_io_o* inst, struct tm_the_truth_o* tt, tm_tt_id_t asset)
{
	return false;
}



static uint64_t import_asset(struct tm_asset_io_o* inst, const char* file, const struct tm_asset_io_import* args)
{
	tm_logger_api->print(TM_LOG_TYPE_INFO, "importing new angelscript asset");
	struct tm_os_file_io_api* file_io = tm_os_api->file_io;
	tm_file_o f = file_io->open_input(file);
	if (f.valid) {
		uint64_t file_size = file_io->size(f);
		TM_INIT_TEMP_ALLOCATOR(ta);

		char* buffer = (char*)tm_temp_alloc(ta, file_size + 1);
		file_io->read(f, buffer, file_size);
		buffer[file_size] = '\0';

		tm_config_i* config = tm_config_api->create(args->allocator);
		char error_buffer[TM_JSON_ERROR_STRING_LENGTH + 1];
		//All extensions!
		if (!tm_json_api->parse(buffer, config, (tm_json_parse_ext)0xffffffff, error_buffer)) {
			tm_logger_api->printf(TM_LOG_TYPE_ERROR, "Failed to parse script_project %s", file);
			tm_logger_api->printf(TM_LOG_TYPE_ERROR, "Error %s", error_buffer);
			return 0;
		}
		tm_config_item_t root = config->root(config->inst);
		tm_config_item_t main = config->object_get(config->inst, root, TM_SCRIPT_PROJECT_MAIN_FIELD);
		const char* main_file = config->to_string(config->inst, main);
		//TODO: script project file should instead define the modules that other files should link towards
		//main file should be local to the script project

		//const char* extensions[] = { ".script_project" };
		tm_str_t file_t = tm_str(file);
		tm_str_t path_name = tm_path_api->strip_extension(file_t);
		//This just changes the size of the string so we have to create a new string from its results?
		tm_str_t main_path = tm_path_api->directory(file_t);// tm_path_api_dir(file, path_name, ta);
		//char* main_path_str =  (char*)tm_temp_alloc(ta, main_path.size);

		const char* completed_file = tm_temp_allocator_api->printf(ta, "%s/%s", main_path.data, main_file);

		CScriptBuilder builder;
		builder.StartNewModule(script_engine, "main_module");
		builder.AddSectionFromFile(completed_file);
		builder.BuildModule();
		asIScriptModule* script_module = builder.GetModule();
		(void)script_module;

		tm_config_api->destroy(config);

		//TODO: Save bytecode to the truth
		tm_the_truth_o* tt = args->tt;
		tm_tt_undo_scope_t undo_scope = { 0 };

		tm_the_truth_property_definition_t prop = {};
		prop.buffer_extension = "abc";
		prop.buffer_extension_f = nullptr;
		prop.editor = TM_THE_TRUTH__EDITOR__HIDDEN;
		prop.name = "bytecode";
		prop.type = TM_THE_TRUTH_PROPERTY_TYPE_BUFFER;
		const tm_tt_type_t script_module_type = tm_the_truth_api->create_object_type(tt, "script_module", &prop, 1);
		tm_the_truth_api->add_properties(tt, script_module_type, &prop, 1);

		const tm_tt_id_t asset_id = tm_the_truth_api->create_object_of_type(tt, script_module_type, undo_scope);
		tm_the_truth_object_o* asset_obj = tm_the_truth_api->write(tt, asset_id);

		tm_buffers_i* buffers = tm_the_truth_api->buffers(tt);
		void* data_dst = buffers->allocate(buffers->inst, 16, 0);
		uint32_t buffer_index = buffers->add(buffers->inst, data_dst, 16, 0);
		tm_the_truth_api->set_buffer(tt, asset_obj, TM_TT_PROP__SCRIPT_MODULE__DATA, buffer_index);
		tm_the_truth_api->commit(tt, asset_obj, undo_scope);

		TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
		file_io->close(f);
	}
	return 0;
}

static void importer_extensions_string(struct tm_asset_io_o* inst, char** output, struct tm_temp_allocator_i* ta, const char* separator)
{
	const char* extension = "script_project";
	tm_carray_temp_printf(output, ta, "%.*s%s", strlen(extension), extension, separator);
}


static struct tm_asset_io_i angelscript_importer;

struct tm_asset_io_i* angelscript_io_interface(asIScriptEngine* engine) {
	script_engine = engine;
	angelscript_importer.enabled = enabled;
	angelscript_importer.can_import = can_import;
	angelscript_importer.can_reimport = can_reimport;
	angelscript_importer.import_asset = import_asset;
	angelscript_importer.importer_extensions_string = importer_extensions_string;
	return &angelscript_importer;
}