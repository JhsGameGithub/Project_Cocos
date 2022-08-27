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
	//������ �� �Ҹ���
	explicit Engine(Renderer& a_renderer);
	~Engine();

public:
	//Getter �� Setter 
	Scene* Get_Scene() const;
	void Set_Scene(Scene* a_scene);

public:
	//�Ϲ� �Լ�
	void Run();
};