#include "Engine.h"
#include "Scene.h"
#include "Object.h"
#include "Renderer.h"

int main(int argc, char* args[])
{
	Renderer renderer;
	Engine engine(renderer);
	Object object1;
	object1.Set_Num(1);
	Object object2;
	object2.Set_Num(2);

	engine.Get_Scene()->Register_Object(&object1);
	engine.Get_Scene()->Register_Object(&object2);

	engine.Run();
}