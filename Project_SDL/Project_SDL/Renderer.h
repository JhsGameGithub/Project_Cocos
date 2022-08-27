#pragma once

class Scene;

class Renderer
{
public:
	void Init();
	void Rendering(const Scene* a_scene);
};