#pragma once

class Renderer;

class Engine
{
private:
	int SCREEN_WIDTH = 800;
	int SCREEN_HEIGHT = 600;
	Renderer& m_renderer;
public:
	Engine(Renderer& a_renderer);
	void Run();
};