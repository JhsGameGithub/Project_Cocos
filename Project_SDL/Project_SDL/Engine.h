#pragma once

class Renderer;
class Scene;

class Engine
{
private:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;

	Scene* m_scene;

	Renderer& m_renderer;
public:
	//생성자 및 소멸자
	explicit Engine(Renderer& a_renderer);
	~Engine();

public:
	//Getter 및 Setter 
	Scene* Get_Scene() const;
	void Set_Scene(Scene* a_scene);

public:
	//일반 함수
	void Run();
};