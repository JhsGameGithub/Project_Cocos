#include "Object.h"

void Object::Start()
{
	for (Component* component : component_vec)
		component->Start();
	for (Object* object : child_vec)
		object->Start();
}

void Object::Update()
{
	for (Component* component : component_vec)
		component->Update();
	for (Object* object : child_vec)
		object->Update();
}