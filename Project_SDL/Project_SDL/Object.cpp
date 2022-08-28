#include "Object.h"
#include "Transform.h"

Object::Object() : m_transform(new Transform()), m_parent(nullptr)
{
	m_transform->Set_Object(this);
}

Object::~Object()
{
	delete m_transform;
}

void Object::Start()
{
	for (auto& component : m_component_map)
		component.second->Start();
	for (Object* object : m_child_list)
		object->Start();
}

void Object::Update()
{
	for (auto& component : m_component_map)
		component.second->Update();
	for (Object* object : m_child_list)
		object->Update();
}

void Object::Add_Child(Object* a_object)
{
	a_object->Set_Parent(this);
	m_child_list.push_back(a_object);
}

void Object::Set_Parent(Object* a_object)
{
	m_parent = a_object;
}

void Object::Add_Component(Component* a_component)
{
	unsigned int component_code = typeid(a_component).hash_code();
	//컴포넌트 추가 실패
	if (m_component_map.find(component_code) != m_component_map.end())
	{
		printf("컴포넌트 추가 실패");
		return;
	}
	else
	{
		a_component->Set_Object(this);
		m_component_map.insert({ component_code,a_component });
	}
}

Transform* Object::Get_Transform()
{
	return m_transform;
}