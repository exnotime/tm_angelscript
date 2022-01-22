#pragma once
extern "C" {
	#include <foundation/api_types.h>
}

//TM
//TODO: Cleanup taken from assimp plugin
extern struct tm_logger_api* tm_logger_api;
extern struct tm_os_thread_api* tm_thread_api;
extern struct tm_os_api* tm_os_api;
extern struct tm_input_api* tm_input_api;
extern struct tm_error_api* tm_error_api;
extern struct tm_api_registry_api* tm_global_api_registry;
extern struct tm_temp_allocator_api* tm_temp_allocator_api;
extern struct tm_the_truth_api* tm_the_truth_api;
extern struct tm_the_truth_assets_api* tm_the_truth_assets_api;
extern struct tm_buffer_format_api* tm_buffer_format_api;
extern struct tm_profiler_api* tm_profiler_api;
extern struct tm_job_system_api* tm_job_system_api;
extern struct tm_ui_api* tm_ui_api;
extern struct tm_localizer_api* tm_localizer_api;
extern struct tm_path_api* tm_path_api;
extern struct tm_math_api* tm_math_api;
extern struct tm_allocator_api* tm_allocator_api;
extern struct tm_properties_view_api* tm_properties_view_api;
extern struct tm_entity_api* tm_entity_api;
extern struct tm_shader_repository_api* tm_shader_repository_api;
extern struct tm_render_context_api* tm_render_context_api;
extern struct tm_render_graph_api* tm_render_graph_api;
extern struct tm_runtime_data_repository_api* tm_runtime_data_repository_api;
extern struct tm_shader_api* tm_shader_api;
extern struct tm_shader_system_api* tm_shader_system_api;
extern struct tm_visibility_flags_api* tm_visibility_flags_api;
extern struct tm_renderer_api* tm_renderer_api;
extern struct tm_owner_component_api* tm_owner_component_api;
extern struct tm_image_loader_api* tm_image_loader_api;
extern struct tm_default_render_pipe_api* tm_default_render_pipe_api;
extern struct tm_dcc_asset_api* tm_dcc_asset_api;
extern struct tm_progress_report_api* tm_progress_report_api;
extern struct tm_task_system_api* tm_task_system_api;
extern struct tm_the_truth_replacer_api* tm_the_truth_replacer_api;
extern struct tm_the_truth_common_types_api* tm_the_truth_common_types_api;
extern struct tm_sprintf_api* tm_sprintf_api;
extern struct tm_config_api* tm_config_api;
extern struct tm_json_api* tm_json_api; 
extern struct tm_the_machinery_api* tm_the_machinery_api; 
extern struct tm_entity_commands_api* tm_entity_commands_api;
extern struct tm_simulation_api* tm_simulation_api;
extern struct tm_transform_component_api* tm_transform_component_api;
extern struct tm_camera_api* tm_camera_api;
extern struct tm_physx_scene_api* tm_physx_scene_api;
extern struct tm_tag_component_api* tm_tag_component_api;
extern struct tm_draw2d_api* tm_draw2d_api;


struct tm_allocator_i;
struct tm_the_truth_o;

typedef struct tm_angelscript_system {
	tm_allocator_i* allocator;
	tm_the_truth_o* tt;
	tm_tt_id_t asset_root;
} tm_angelscript_system;

tm_angelscript_system* get_system();
