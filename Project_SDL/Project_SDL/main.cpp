#include "Engine.h"
#include "Renderer.h"

int main(int argc, char* args[])
{
	Renderer renderer;
	Engine engine(renderer);

	engine.Run();
}