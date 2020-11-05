#pragma once
#include "Module.h"
#include "Globals.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);

private:
	void TranslateCamera(float deltaTime);
	void RotateCameraKeys(float deltaTime);

private:
	void* context;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
};
