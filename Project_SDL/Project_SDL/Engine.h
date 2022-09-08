#pragma once

namespace RendererHelper{
	class Renderer;
}
class Scene;

class Engine
{
private:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;

	Scene* m_scene;

	RendererHelper::Renderer* m_renderer;
public:
	//������ �� �Ҹ���
	explicit Engine();
	~Engine();

public:
	//Getter �� Setter 
	Scene* Get_Scene() const;
	void Set_Scene(Scene* a_scene);

public:
	//�Ϲ� �Լ�
	void Run();
};