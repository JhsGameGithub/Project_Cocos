#pragma once
#include <vector>

class Object;

class Scene
{
private:
	std::vector<Object*> object_vec;
public:
	void Register_Object(Object* object);
	void Start();
	void Update();
};