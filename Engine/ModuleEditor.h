#pragma once
#include "Module.h"
#include "Globals.h"
#include <SDL.h>
#include "ImGUI/imgui.h"
#include <vector>

class Window;
class WConsole;
class WMonitor;

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
	void Log(const char* input);
	void ProcessFPS(float deltaTime);
private:
	ImGuiIO* io = nullptr;
	
	WConsole* console = nullptr;
	WMonitor* monitor = nullptr;

	std::vector<Window*> editorWindows;
};