#include "Engine.h"
#include "Scene.h"
#include "Object.h"
#include "Renderer.h"

int main(int argc, char* args[])
{
	Renderer renderer;
	Engine engine(renderer);
	Object object1;
	Object object2;

	engine.Get_Scene()->Register_Object(&object1);
	engine.Get_Scene()->Register_Object(&object2);

	engine.Run();
}