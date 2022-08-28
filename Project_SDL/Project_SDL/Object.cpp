#include "Object.h"
#include "Component.h"

void Object::Start()
{
	printf("%d\n", m_num);
	for (auto& component : m_component_map)
		component.second->Start();
	for (Object* object : m_child_vec)
		object->Start();
}

void Object::Update()
{
	for (auto& component : m_component_map)
		component.second->Update();
	for (Object* object : m_child_vec)
		object->Update();
}

void Object::Add_Component(Component* component)
{
	unsigned int component_code = typeid(component).hash_code();
	//컴포넌트 추가 실패
	if (m_component_map.find(component_code) != m_component_map.end())
		return;
	else
		m_component_map.insert({ component_code,component });
}