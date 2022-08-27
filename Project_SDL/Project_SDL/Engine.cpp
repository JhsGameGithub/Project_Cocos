#include <iostream>
#include <SDL.h>

#include "Engine.h"
#include "Renderer.h"
#include "Scene.h"

Engine::Engine(Renderer& a_renderer) : m_renderer(a_renderer), m_scene(new Scene())
{

}

Engine::~Engine()
{
	delete m_scene;
}

Scene* Engine::Get_Scene() const
{
	return m_scene;
}

void Engine::Set_Scene(Scene* a_scene)
{
	m_scene = a_scene;
}

void Engine::Run()
{
	//�������� ������ ������ ��ü ����
	SDL_Window* window = NULL;

	m_renderer.Init();

	//SDL �ʱ�ȭ
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//������ ����
		window = SDL_CreateWindow("SDL INIT", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			while (true)
			{
				SDL_Event sdl_event;
				if (SDL_PollEvent(&sdl_event))
					if (sdl_event.type == SDL_QUIT)
						break;
				m_renderer.Rendering(m_scene);	
			}
		}
	}
	//������ ��ü �ı�
	SDL_DestroyWindow(window);

	//SDL ���� �ý��� ����
	SDL_Quit();

	printf("Sucees Run");
}