
extern "C" {
#include <foundation/error.h>
#include <foundation/api_registry.h>
#include <foundation/log.h>
#include <foundation/plugin_callbacks.h>
#include <foundation/asset_io.h>
#include <foundation/temp_allocator.h>
#include <foundation/os.h>
#include <foundation/api_registry.h>
#include <foundation/asset_io.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_assets.h>
#include <foundation/the_truth_types.h>
#include <foundation/json.h>
#include <foundation/math.h>
#include <foundation/job_system.h>
#include <foundation/error.h>
#include <foundation/config.h>
#include <foundation/log.h>
#include <foundation/path.h>
#include <foundation/buffer_format.h>
#include <foundation/profiler.h>
#include <foundation/localizer.h>
#include <foundation/core.h>
#include <plugins/entity/entity.h>
#include <foundation/runtime_data_repository.h>
#include <foundation/task_system.h>
#include <foundation/sprintf.h>
#include <foundation/macros.h>
#include <foundation/the_truth.h>
#include <foundation/progress_report.h>
#include <foundation/application.h>
#include <foundation/allocator.h>
#include <plugins/ui/ui.h>
#include <plugins/editor_views/asset_browser.h>
#include <plugins/editor_views/properties.h>
#include <the_machinery/the_machinery.h>
#include <plugins/simulation/simulation_entry.h>
#include <plugins/simulation/simulation.h>
#include <plugins/entity/transform_component.h>
#include <foundation/input.h>
#include <foundation/camera.h>
#include <plugins/physx/physx_scene.h>
}

struct tm_logger_api* tm_logger_api;
struct tm_os_thread_api* tm_thread_api;
struct tm_os_time_api* tm_time_api;
struct tm_error_api* tm_error_api;
struct tm_api_registry_api* tm_global_api_registry;
struct tm_temp_allocator_api* tm_temp_allocator_api;
struct tm_the_truth_api* tm_the_truth_api;
struct tm_the_truth_assets_api* tm_the_truth_assets_api;
struct tm_buffer_format_api* tm_buffer_format_api;
struct tm_profiler_api* tm_profiler_api;
struct tm_job_system_api* tm_job_system_api;
struct tm_ui_api* tm_ui_api;
struct tm_localizer_api* tm_localizer_api;
struct tm_os_api* tm_os_api;
struct tm_asset_io_api* tm_asset_io_api;
struct tm_path_api* tm_path_api;
struct tm_math_api* tm_math_api;
struct tm_properties_view_api* tm_properties_view_api;
struct tm_entity_api* tm_entity_api;
struct tm_shader_repository_api* tm_shader_repository_api;
struct tm_render_context_api* tm_render_context_api;
struct tm_render_graph_api* tm_render_graph_api;
struct tm_runtime_data_repository_api* tm_runtime_data_repository_api;
struct tm_shader_api* tm_shader_api;
struct tm_shader_system_api* tm_shader_system_api;
struct tm_visibility_flags_api* tm_visibility_flags_api;
struct tm_renderer_api* tm_renderer_api;
struct tm_owner_component_api* tm_owner_component_api;
struct tm_image_loader_api* tm_image_loader_api;
struct tm_default_render_pipe_api* tm_default_render_pipe_api;
struct tm_dcc_asset_api* tm_dcc_asset_api;
struct tm_progress_report_api* tm_progress_report_api;
struct tm_task_system_api* tm_task_system_api;
struct tm_the_truth_replacer_api* tm_the_truth_replacer_api;
struct tm_the_truth_common_types_api* tm_the_truth_common_types_api;
struct tm_sprintf_api* tm_sprintf_api;
struct tm_config_api* tm_config_api;
struct tm_json_api* tm_json_api;
struct tm_the_machinery_api* tm_the_machinery_api;
struct tm_application_api* tm_application_api;
struct tm_allocator_api* tm_allocator_api;
struct tm_input_api* tm_input_api;
struct tm_entity_commands_api* tm_entity_commands_api;
struct tm_simulation_api* tm_simulation_api;
struct tm_transform_component_api* tm_transform_component_api;
struct tm_camera_api* tm_camera_api;
struct tm_physx_scene_api* tm_physx_scene_api;

#include <angelscript.h>

static asIScriptEngine* script_engine;
static asIScriptContext* script_context; //TODO: Allow multiple

#include "tm_as_types.h"
#include "tm_angelscript.h"
#include "tm_as_string.h"
#include "tm_as_array.h"
#include "tm_as_entity_api.h"
#include "tm_as_component_api.h"
#include "tm_as_simulation_api.h"
#include "tm_as_the_truth_api.h"
#include "tm_as_input_api.h"
#include "tm_as_camera_api.h"
#include "tm_as_script_component.h"
#include "tm_as_physics_api.h"
#include "tm_as_math_api.h"
#include "tm_as_camera_api.h"
#include "angelscript_compiler.h"

#include <iostream>
#include <filesystem>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

//TODO: Move to separate file
void MessageCallback(const asSMessageInfo* msg, void* param) {
	tm_log_type log_type = TM_LOG_TYPE_ERROR;
	const char* type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING) {
		type = "WARN";
	} else if (msg->type == asMSGTYPE_INFORMATION) {
		type = "INFO";
		log_type = TM_LOG_TYPE_INFO;
	}
	tm_logger_api->printf(log_type, "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

extern "C" {
#include <foundation/hash.inl>
#include <foundation/carray.inl>
}

struct tm_as_allocator_t {
	tm_allocator_i allocator;
	struct TM_HASH_T(void*, uint64_t) alloc_map;
	int64_t total;
};

static tm_as_allocator_t _tm_as_allocator;

void* tm_as_alloc(size_t size) {
	void* ptr = tm_alloc(&_tm_as_allocator.allocator, size);
	tm_hash_add(&_tm_as_allocator.alloc_map, ptr, size);
	_tm_as_allocator.total += size;
	return ptr;
}

void tm_as_free(void* ptr) {
	uint64_t size = tm_hash_get(&_tm_as_allocator.alloc_map, ptr);
	tm_free(&_tm_as_allocator.allocator, ptr, size);
	tm_hash_remove(&_tm_as_allocator.alloc_map, ptr);
	_tm_as_allocator.total -= size;
}

static tm_angelscript_system _tm_as_system;
tm_angelscript_system* get_system() {
	//Setup state for angelscript, cant be called during boot
	_tm_as_system.allocator = &_tm_as_allocator.allocator;
	_tm_as_system.tt = tm_the_machinery_api->get_truth(tm_application_api->application());
	_tm_as_system.asset_root = tm_the_machinery_api->get_asset_root(tm_application_api->application());
	return &_tm_as_system;
}
void setup_angelscript() {
	//Setup allocator
	const tm_allocator_i* system_allocator = tm_allocator_api->system;
	_tm_as_allocator.allocator = tm_allocator_api->create_child(system_allocator, "Angelscript allocator");
	_tm_as_allocator.alloc_map = {};
	_tm_as_allocator.alloc_map.allocator = &_tm_as_allocator.allocator;
	asSetGlobalMemoryFunctions(tm_as_alloc, tm_as_free);
	
	//Create engine
	script_engine = asCreateScriptEngine();
	as_compiler::initialize(script_engine);
	script_engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	script_engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, false);
	script_engine->SetEngineProperty(asEP_USE_CHARACTER_LITERALS, true); //to have 'c' mean a single char
	script_engine->SetEngineProperty(asEP_ALLOW_MULTILINE_STRINGS, true);
	//TODO: Create context manager
	script_context = script_engine->CreateContext();
	//Register engine interfaces
	register_tm_types(script_engine);
	tm_string::register_tm_string_interface(script_engine, &_tm_as_allocator.allocator);
	tm_math::register_math_interface(script_engine, &_tm_as_allocator.allocator);
	tm_array::register_tm_array(script_engine, &_tm_as_allocator.allocator);
	tm_the_truth::register_the_truth_interface(script_engine, &_tm_as_allocator.allocator);
	tm_entity::register_tm_entity_interface(script_engine);
	tm_component::register_component_interface(script_engine);
	tm_simulation::register_simulation_interface(script_engine, &_tm_as_allocator.allocator);
	tm_input::register_input_interface(script_engine);
	tm_camera::register_camera_interface(script_engine, &_tm_as_allocator.allocator);
	tm_script_component::register_script_component_interface(script_engine, &_tm_as_allocator.allocator);
	tm_physics::register_physics_interface(script_engine, &_tm_as_allocator.allocator);
}

void prepare_angelscript_function(asIScriptFunction* func) {
	if (script_context->Prepare(func) < 0) {
		tm_logger_api->printf(TM_LOG_TYPE_ERROR, "Failed to prepare script function");
	}
}

asIScriptContext* get_script_context() {
	return script_context;
}

void run_angelscript_function(asIScriptFunction* func) {
	
	int ret = script_context->Execute();
	if (ret == asEXECUTION_SUSPENDED) {
		script_context->Execute();
	}
	if (ret == asEXECUTION_EXCEPTION) {
		int column;
		const char* filename;
		int line = script_context->GetExceptionLineNumber(&column, &filename);
		tm_logger_api->printf(TM_LOG_TYPE_ERROR, "Angelscript exception: %s : File %s:%d,%d", script_context->GetExceptionString(), filename, line, column);
	}
}

void wait_until_finished() {
	bool done = false;
	asEContextState state = script_context->GetState();
	while (!done) {
		switch (state)
		{
		case asEXECUTION_FINISHED:
			done = true;
			break;
		case asEXECUTION_SUSPENDED:
			break;
		case asEXECUTION_ABORTED:
			done = true;
			break;
		case asEXECUTION_EXCEPTION:
			done = true;
			break;
		case asEXECUTION_PREPARED:
			break;
		case asEXECUTION_UNINITIALIZED:
			done = true;
			break;
		case asEXECUTION_ACTIVE:
			break;
		case asEXECUTION_ERROR:
			done = true;
			break;
		default:
			done = true;
			break;
		}
		if (!done) {
			Sleep(1);
			state = script_context->GetState();
		}
	}
}

void shutdown_angelscript() {
	tm_string::destroy_string_factory(&_tm_as_allocator.allocator);
	script_context->Release();
	script_engine->ShutDownAndRelease();
	tm_hash_free(&_tm_as_allocator.alloc_map);
	tm_allocator_api->destroy_child(&_tm_as_allocator.allocator);
}

extern "C" {

#define TM_TT_TYPE__SCRIPT_MODULE "script_module"
#define TM_TT_TYPE_HASH__SCRIPT_MODULE TM_STATIC_HASH("script_module", 0x144d369e3a49eba2ULL)
#define TM_TT_TYPE_HASH__SCRIPT_MODULE_DEFINES TM_STATIC_HASH("defines", 0xe7420a90176aad43ULL)

#define TM_TT_TYPE_SCRIPT_SIMULATION "script_simulation"
#define TM_TT_TYPE_HASH__SCRIPT_SIMULATION TM_STATIC_HASH("script_simulation", 0xc8d39de7cadebcd6ULL)
#define	TM_TT_TYPE_HASH__SCRIPT_SIMULATION_MODULE TM_STATIC_HASH("module", 0xaf2b5e99b268c9d0ULL)

	enum SCRIPT_MODULE_PROPERTIES {
		TM_TT_PROP__SCRIPT_MODULE__MAIN,
		TM_TT_PROP__SCRIPT_MODULE__BYTECODE,
		TM_TT_PROP__SCRIPT_MODULE__DEFINES,
		TM_TT_PROP__SCRIPT_MODULE__COUNT
	};

	const char* get_project_path(tm_temp_allocator_i* temp_allocator) {
		tm_application_o* app = tm_application_api->application();
		tm_tt_id_t project_setting_tt_id;
		tm_the_truth_o* project_tt = tm_the_machinery_api->project_settings(app, &project_setting_tt_id);
		const tm_the_truth_object_o* project_obj = tm_the_truth_api->read(project_tt, project_setting_tt_id);
		tm_tt_prop_value_t project_settings_path = tm_the_truth_api->get_property_value(project_tt, project_obj, TM_TT_PROP__PROJECT_SETTINGS__PATH, temp_allocator);
		return project_settings_path.string;
	}

bool compile_module_object(tm_the_truth_o* tt, tm_tt_id_t object, tm_temp_allocator_i* temp_allocator) {
	const char* project_path = get_project_path(temp_allocator);
	const tm_the_truth_object_o* module_read_obj = tm_the_truth_api->read(tt, object);
	tm_tt_prop_value_t main_path_prop = tm_the_truth_api->get_property_value(tt, module_read_obj, TM_TT_PROP__SCRIPT_MODULE__MAIN, temp_allocator);
	void* bytecode = nullptr;
	uint64_t byte_code_size = as_compiler::compile_file_to_bytecode(script_engine, project_path, main_path_prop.string, "name", &bytecode, temp_allocator);
	if (byte_code_size) {
		tm_the_truth_object_o* module_write_obj = tm_the_truth_api->write(tt, object);
		tm_the_truth_api->set_buffer_content(tt, module_write_obj, TM_TT_PROP__SCRIPT_MODULE__BYTECODE, bytecode, byte_code_size);
		tm_the_truth_api->commit(tt, module_write_obj, TM_TT_NO_UNDO_SCOPE);
		return true;
	}
	return false;
}

enum SCRIPT_SIMULATION_PROPERTIES {
	TM_TT_PROP__SCRIPT_SIMULATION__NAME,
	TM_TT_PROP__SCRIPT_SIMULATION__MODULE,
	TM_TT_PROP__SCRIPT_SIMULATION__COUNT
};

struct tm_script_simulation_t {
	asIScriptModule* as_module;
	asIScriptFunction* start_func;
	asIScriptFunction* tick_func;
	asIScriptFunction* stop_func;
	asIScriptFunction* reload_func;
};

static const char* simulation_start_declaration = "void simulation_start(const tm_simulation_start_args_t@ args)";
static const char* simulation_tick_declaration = "void simulation_tick(const tm_simulation_frame_args_t@ args)";
static const char* simulation_stop_declaration = "void simulation_stop(tm_entity_commands_o@ commands)";

//Array of simulations
static tm_script_simulation_t* _simulations = nullptr;

static tm_simulation_state_o* simulation_start(tm_simulation_start_args_t* args){
	TM_INIT_TEMP_ALLOCATOR(ta);
	if (_simulations) {
		tm_carray_free(_simulations, &_tm_as_allocator.allocator);
	}
	tm_tt_type_t plugin_type = tm_the_truth_api->object_type_from_name_hash(args->tt, TM_TT_TYPE_HASH__SCRIPT_SIMULATION);
	tm_tt_id_t* script_simulations = tm_the_truth_api->all_objects_of_type(args->tt, plugin_type, ta);
	uint32_t sim_count = (uint32_t)tm_carray_size(script_simulations);
	bool any_success = false;
	if (sim_count > 0) {
		_simulations = tm_carray_create(tm_script_simulation_t, sim_count, &_tm_as_allocator.allocator);

		for (uint32_t i = 0; i < sim_count; ++i) {
			const tm_the_truth_object_o* plugin_obj = tm_the_truth_api->read(args->tt, script_simulations[i]);
			tm_tt_id_t module_id = tm_the_truth_api->get_reference(args->tt, plugin_obj, TM_TT_PROP__SCRIPT_SIMULATION__MODULE);
			const tm_the_truth_object_o* module_obj = tm_the_truth_api->read(args->tt, module_id);
			tm_tt_buffer_t bytecode_buffer = tm_the_truth_api->get_buffer(args->tt, module_obj, TM_TT_PROP__SCRIPT_MODULE__BYTECODE);
			const char* module_name = tm_the_truth_assets_api->object_asset_name(args->tt, module_id);
			//For now recompile all modules before getting the bytecode
			bool success = compile_module_object(args->tt, module_id, ta);
			if (success) {
				any_success = true;
				_simulations[i].as_module = as_compiler::get_module_from_bytecode(module_name, bytecode_buffer.size, bytecode_buffer.data);
				//Reflect entry points
				if (_simulations[i].as_module) {
					_simulations[i].start_func = _simulations[i].as_module->GetFunctionByDecl(simulation_start_declaration);
					_simulations[i].tick_func = _simulations[i].as_module->GetFunctionByDecl(simulation_tick_declaration);
					_simulations[i].stop_func = _simulations[i].as_module->GetFunctionByDecl(simulation_stop_declaration);
				}
			}
		}
	}
	TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
	if (any_success) {
		for (uint32_t i = 0; i < sim_count; ++i) {
			prepare_angelscript_function(_simulations[i].start_func);
			script_context->SetArgAddress(0, args);
			run_angelscript_function(_simulations[i].start_func);
		}
		tm_simulation_state_o* state = (tm_simulation_state_o*)tm_alloc(&_tm_as_allocator.allocator, sizeof(tm_simulation_state_o*));
		return state;
	}
	//Will this make sure it wont run any other callbacks?
	return nullptr;
}

void simulation_stop(tm_simulation_state_o* state, struct tm_entity_commands_o* commands) {
	wait_until_finished();
	uint32_t sim_count = (uint32_t)tm_carray_size(_simulations);
	for (uint32_t i = 0; i < sim_count; ++i) {
		tm_script_simulation_t sim = _simulations[i];
		if (sim.stop_func) {
			prepare_angelscript_function(sim.stop_func);
			get_script_context()->SetArgAddress(0, commands);
			run_angelscript_function(sim.stop_func);
		}
	}
	if (state) {
		tm_free( &_tm_as_allocator.allocator, state, sizeof(tm_simulation_state_o*));
	}
}

void simulation_tick(tm_simulation_state_o* state, tm_simulation_frame_args_t* args) {
	uint32_t sim_count = (uint32_t)tm_carray_size(_simulations);
	for (uint32_t i = 0; i < sim_count; ++i) {
		if (_simulations[i].tick_func) {
			prepare_angelscript_function(_simulations[i].tick_func);
			get_script_context()->SetArgAddress(0, args);
			run_angelscript_function(_simulations[i].tick_func);
		}
	}

	//Run garbage collection
	//TODO: Scale garbage collection to keep up with garbage build up
	script_engine->GarbageCollect(asGC_ONE_STEP);

}
void simulation_hot_reload(tm_simulation_state_o* state, struct tm_entity_commands_o* commands) {
	uint32_t sim_count = (uint32_t)tm_carray_size(_simulations);
	for (uint32_t i = 0; i < sim_count; ++i) {
		if (_simulations[i].reload_func) {
			prepare_angelscript_function(_simulations[i].reload_func);
			run_angelscript_function(_simulations[i].reload_func);
		}
	}
}

static float module_properties__custom_ui(struct tm_properties_ui_args_t* args, tm_rect_t item_rect, tm_tt_id_t object){
	TM_INIT_TEMP_ALLOCATOR(ta);
	bool picked = false;
	item_rect.y = tm_properties_view_api->ui_open_path(args, item_rect, TM_LOCALIZE_LATER("Main file"), TM_LOCALIZE_LATER("Path to main file"), object, TM_TT_PROP__SCRIPT_MODULE__MAIN, "as, *", "as", &picked);
	if (picked) {
		//localize path
		const tm_the_truth_object_o* module_read_obj = tm_the_truth_api->read(args->tt, object);
		tm_tt_prop_value_t main_path_prop = tm_the_truth_api->get_property_value(args->tt, module_read_obj, TM_TT_PROP__SCRIPT_MODULE__MAIN, ta);
		const char* project_path = get_project_path(ta);
		auto relative_path = std::filesystem::relative(std::filesystem::path(main_path_prop.string), std::filesystem::path(project_path));
		tm_the_truth_object_o* module_write_obj = tm_the_truth_api->write(args->tt, object);
		std::string p = relative_path.string();
		const char* path = p.c_str();
		tm_the_truth_api->set_string(args->tt, module_write_obj, TM_TT_PROP__SCRIPT_MODULE__MAIN, path);
		tm_the_truth_api->commit(args->tt, module_write_obj, args->last_undo_scope);

	}
	tm_ui_button_t compile_button = {};
	compile_button.text = "Compile";
	compile_button.tooltip = "Compiles the script";
	compile_button.rect = item_rect;
	if (tm_ui_api->button(args->ui, args->uistyle, &compile_button)) {
		compile_module_object(args->tt, object, ta);
	}
	item_rect.y += compile_button.rect.h + args->metrics[TM_PROPERTIES_METRIC_MARGIN];

	TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
	return item_rect.y;
}


static float simulation_properties__custom_ui(struct tm_properties_ui_args_t* args, tm_rect_t item_rect, tm_tt_id_t object) {
	TM_INIT_TEMP_ALLOCATOR(ta);

	item_rect.y = tm_properties_view_api->ui_string(args, item_rect, "Name", "Name of simulation", object, TM_TT_PROP__SCRIPT_SIMULATION__NAME);

	tm_tt_type_t module_type = tm_the_truth_api->object_type_from_name_hash(args->tt, TM_TT_TYPE_HASH__SCRIPT_MODULE);
	tm_tt_id_t* modules = tm_the_truth_api->all_objects_of_type(args->tt, module_type, ta);
	uint32_t module_count = (uint32_t)tm_carray_size(modules);
	const char** module_names = (const char**)tm_temp_alloc(ta, sizeof(char*) * module_count + 1);
	tm_tt_id_t* module_list = (tm_tt_id_t*)tm_temp_alloc(ta, sizeof(tm_tt_id_t) * module_count + 1);

	module_names[0] = "None";
	module_list[0] = { 0 };

	for (uint32_t i = 1; i < module_count + 1; ++i) {
		module_names[i] = tm_the_truth_assets_api->object_asset_name(args->tt, modules[i - 1]);
		module_list[i] = modules[i - 1];
	}
	item_rect.y = tm_properties_view_api->ui_reference_popup_picker(args, item_rect, "Module", "Selects the module", object, TM_TT_PROP__SCRIPT_SIMULATION__MODULE, module_names, module_list, module_count + 1);
	TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
	return item_rect.y;
}

static void create_asset_type(tm_the_truth_o* tt) {
	//Module type
	static tm_the_truth_property_definition_t module_properties[] = {
		{ "main", TM_THE_TRUTH_PROPERTY_TYPE_STRING },
		{ "bytecode", TM_THE_TRUTH_PROPERTY_TYPE_BUFFER }
	};
	const tm_tt_type_t module_type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__SCRIPT_MODULE, module_properties, TM_ARRAY_COUNT(module_properties));
	tm_the_truth_api->set_aspect(tt, module_type, TM_TT_ASPECT__FILE_EXTENSION, TM_TT_TYPE__SCRIPT_MODULE);
	static tm_properties_aspect_i properties_aspect;
	properties_aspect.custom_ui = module_properties__custom_ui;
	tm_the_truth_api->set_aspect(tt, module_type, TM_TT_ASPECT__PROPERTIES, &properties_aspect);

	//Simulation type
	static tm_the_truth_property_definition_t simulation_properties[TM_TT_PROP__SCRIPT_SIMULATION__COUNT];
	simulation_properties[TM_TT_PROP__SCRIPT_SIMULATION__NAME] = {};
	simulation_properties[TM_TT_PROP__SCRIPT_SIMULATION__NAME].name = "Name"; 
	simulation_properties[TM_TT_PROP__SCRIPT_SIMULATION__NAME].type = TM_THE_TRUTH_PROPERTY_TYPE_STRING;

	simulation_properties[TM_TT_PROP__SCRIPT_SIMULATION__MODULE] = {};
	simulation_properties[TM_TT_PROP__SCRIPT_SIMULATION__MODULE].name = "Module";
	simulation_properties[TM_TT_PROP__SCRIPT_SIMULATION__MODULE].type = TM_THE_TRUTH_PROPERTY_TYPE_REFERENCE;
	simulation_properties[TM_TT_PROP__SCRIPT_SIMULATION__MODULE].type_hash = TM_TT_TYPE_HASH__ANYTHING;

	const tm_tt_type_t simulation_type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE_SCRIPT_SIMULATION, simulation_properties, TM_ARRAY_COUNT(simulation_properties));
	tm_the_truth_api->set_aspect(tt, simulation_type, TM_TT_ASPECT__FILE_EXTENSION, TM_TT_TYPE_SCRIPT_SIMULATION);
	static tm_properties_aspect_i simulation_properties_aspect;
	simulation_properties_aspect.custom_ui = simulation_properties__custom_ui;
	tm_the_truth_api->set_aspect(tt, simulation_type, TM_TT_ASPECT__PROPERTIES, &simulation_properties_aspect);
}
// -- asset browser register interface
static tm_tt_id_t asset_browser_create_script_module(struct tm_asset_browser_create_asset_o* inst, tm_the_truth_o* tt, tm_tt_undo_scope_t undo_scope)
{
	const tm_tt_type_t type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__SCRIPT_MODULE);
	return tm_the_truth_api->create_object_of_type(tt, type, undo_scope);
}

static tm_asset_browser_create_asset_i asset_browser_create_script_module_inst;

static tm_tt_id_t asset_browser_create_script_simulation(struct tm_asset_browser_create_asset_o* inst, tm_the_truth_o* tt, tm_tt_undo_scope_t undo_scope)
{
	const tm_tt_type_t type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__SCRIPT_SIMULATION);
	tm_tt_id_t tt_id =  tm_the_truth_api->create_object_of_type(tt, type, undo_scope);
	return tt_id;
}

static tm_asset_browser_create_asset_i asset_browser_create_script_simulation_inst;

static tm_simulation_entry_i angelscript_simulation_entry_inst;

static bool already_loaded = false;
TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api* reg, bool load)
{
	//while (!::IsDebuggerPresent())
	//	::Sleep(100);

	if (load) {
		tm_logger_api = tm_get_api(reg, tm_logger_api);
		tm_error_api = tm_get_api(reg, tm_error_api);
		tm_temp_allocator_api = tm_get_api(reg, tm_temp_allocator_api);
		tm_buffer_format_api = tm_get_api(reg, tm_buffer_format_api);
		tm_profiler_api = tm_get_api(reg, tm_profiler_api);
		tm_job_system_api = tm_get_api(reg, tm_job_system_api);
		tm_localizer_api = tm_get_api(reg, tm_localizer_api);
		tm_thread_api = ((struct tm_os_api*)tm_get_api(reg, tm_os_api))->thread;
		tm_time_api = ((struct tm_os_api*)tm_get_api(reg, tm_os_api))->time;
		tm_os_api = tm_get_api(reg, tm_os_api);
		tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
		tm_asset_io_api = tm_get_api(reg, tm_asset_io_api);
		tm_path_api = tm_get_api(reg, tm_path_api);
		tm_math_api = tm_get_api(reg, tm_math_api);
		tm_entity_api = tm_get_api(reg, tm_entity_api);
		tm_runtime_data_repository_api = tm_get_api(reg, tm_runtime_data_repository_api);
		tm_progress_report_api = tm_get_api(reg, tm_progress_report_api);
		tm_task_system_api = tm_get_api(reg, tm_task_system_api);
		tm_the_truth_assets_api = tm_get_api(reg, tm_the_truth_assets_api);
		tm_the_truth_common_types_api = tm_get_api(reg, tm_the_truth_common_types_api);
		tm_sprintf_api = tm_get_api(reg, tm_sprintf_api);
		tm_config_api = tm_get_api(reg, tm_config_api);
		tm_json_api = tm_get_api(reg, tm_json_api);
		tm_the_machinery_api = tm_get_api(reg, tm_the_machinery_api);
		tm_properties_view_api = tm_get_api(reg, tm_properties_view_api);
		tm_application_api = tm_get_api(reg, tm_application_api);
		tm_ui_api = tm_get_api(reg, tm_ui_api);
		tm_allocator_api = tm_get_api(reg, tm_allocator_api);
		tm_input_api = tm_get_api(reg, tm_input_api);
		tm_entity_commands_api = tm_get_api(reg, tm_entity_commands_api);
		tm_simulation_api = tm_get_api(reg, tm_simulation_api);
		tm_transform_component_api = tm_get_api(reg, tm_transform_component_api);
		tm_camera_api = tm_get_api(reg, tm_camera_api);
		tm_physx_scene_api = tm_get_api(reg, tm_physx_scene_api);

		if (!already_loaded) {
			setup_angelscript();
			tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, &create_asset_type);

			asset_browser_create_script_module_inst.asset_name = TM_LOCALIZE_LATER("New Angelscript Module");
			asset_browser_create_script_module_inst.menu_name = TM_LOCALIZE_LATER("New Angelscript Module");
			asset_browser_create_script_module_inst.create = asset_browser_create_script_module;
			tm_add_or_remove_implementation(reg, load, tm_asset_browser_create_asset_i, &asset_browser_create_script_module_inst);

			asset_browser_create_script_simulation_inst.asset_name = TM_LOCALIZE_LATER("New Angelscript Simulation");
			asset_browser_create_script_simulation_inst.menu_name = TM_LOCALIZE_LATER("New Angelscript Simulation");
			asset_browser_create_script_simulation_inst.create = asset_browser_create_script_simulation;
			tm_add_or_remove_implementation(reg, load, tm_asset_browser_create_asset_i, &asset_browser_create_script_simulation_inst);

			angelscript_simulation_entry_inst = {};
			angelscript_simulation_entry_inst.display_name = "Angelscript simulation";
			angelscript_simulation_entry_inst.id = TM_STATIC_HASH("angelscript_simulation_entry", 0xe70c13e72ea3e184ULL);
			angelscript_simulation_entry_inst.start = simulation_start;
			angelscript_simulation_entry_inst.stop = simulation_stop;
			angelscript_simulation_entry_inst.tick = simulation_tick;
			angelscript_simulation_entry_inst.hot_reload = simulation_hot_reload;
			tm_add_or_remove_implementation(reg, load, tm_simulation_entry_i, &angelscript_simulation_entry_inst);
			already_loaded = true;
		}
	} else {
		shutdown_angelscript();
	}

}
}
