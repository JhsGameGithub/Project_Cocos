#pragma once

class Object;

class Component
{
private:
	Object* m_object;
public:
	Component() : m_object(nullptr) {}
	virtual ~Component() {}

	virtual void Start() {}
	virtual void Update() {}

	Object* Get_Object() const;
	void Set_Object(Object* a_object);
};