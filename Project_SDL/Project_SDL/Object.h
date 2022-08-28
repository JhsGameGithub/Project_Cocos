#pragma once
#include <vector>
#include <unordered_map>
#include <typeinfo>

class Component;

class Object
{
private:
	unsigned int m_num;
	Object* m_parent;
	std::vector<Object*> m_child_vec;
	std::unordered_map<unsigned int, Component*> m_component_map;
public:
	void Start();
	void Update();
	
	void Set_Num(unsigned int a_num) { m_num = a_num; }


	//컴포넌트 가져오기
	template<class T>
	T* Get_Component() const
	{
		unsigned int component_code = typeid(T).hash_code();
		return m_component_map.find(component_code) != m_component_map.end() ? dynamic_cast<T*>(m_component_map[component_code]) : nullptr;
	}

	//컴포넌트 추가하기
	void Add_Component(Component* component);
};