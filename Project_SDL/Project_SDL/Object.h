#pragma once
#include <vector>

class Component;

class Object
{
private:
	Object* parent;
	std::vector<Object*> child_vec;
	std::vector<Component*> component_vec;
public:
	void Start();
	void Update();

	template<typename T>
	T Get_Component()
	{

	}
};