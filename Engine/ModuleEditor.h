#pragma once
#include "Module.h"
#include "Globals.h"
#include <SDL.h>
#include "ImGUI/imgui.h"
#include <vector>

class Window;
class WConsole;
class WMonitor;
class WConfig;
class WProperties;
class WAbout;
class Model;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void SendEvent(SDL_Event& event);

	// Windows methods
	void Log(const char* input);
	void ProcessFPS(float deltaTime);
	void UpdateWindowSizeSettings();
	void SelectedModel(Model* model);
private:
	void CreateMainMenu();
private:
	bool should_quit = false;
	ImGuiIO* io = nullptr;
	
	WConsole* console = nullptr;
	WMonitor* monitor = nullptr;
	WConfig* configuration = nullptr;
	WProperties* properties = nullptr;
	WAbout* about = nullptr;

	std::vector<Window*> editorWindows;
};