#include "Scene.h"
#include "Object.h"

void Scene::Register_Object(Object* object)
{
	object_vec.push_back(object);
}

void Scene::Start()
{
	for (Object* object : object_vec)
		object->Start();
}

void Scene::Update()
{
	for (Object* object : object_vec)
		object->Update();
}