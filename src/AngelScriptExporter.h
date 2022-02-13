#pragma once

class asIScriptEngine;
class asIScriptModule;
namespace AngelScriptExporter {
	void ExportEngineAsJSON(const char* file, asIScriptEngine* engine);
	void ImportEngineFromJson(const char* file, asIScriptEngine* engine);
	void ExportModuleAsJSON(const char* file, asIScriptModule* module, asIScriptEngine* engine);
}


