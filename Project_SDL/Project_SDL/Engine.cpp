#include <iostream>
#include <SDL.h>

#include "Engine.h"

void Engine::Run()
{
	//렌더링을 진행할 윈도우 객체 선언
	SDL_Window* window = NULL;

	//SDL 초기화
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//윈도우 생성
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
			}
		}
	}
	//윈도우 객체 파괴
	SDL_DestroyWindow(window);

	//SDL 보조 시스템 종료
	SDL_Quit();

	printf("Sucees Run");
}