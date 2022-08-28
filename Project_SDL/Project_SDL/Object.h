#pragma once
#include <list>
#include <unordered_map>
#include <typeinfo>

class Component;
class Transform;

class Object
{
private:
	Transform* m_transform;
	Object* m_parent;
	std::list<Object*> m_child_list;
	std::unordered_map<unsigned int, Component*> m_component_map;
public:
	Object();
	~Object();

	void Start();
	void Update();

	//자식 오브젝트 추가
	void Add_Child(Object* a_child);
	//부모 오브젝트 설정
	void Set_Parent(Object* a_parent);

	//컴포넌트 추가하기
	void Add_Component(Component* a_component);
	//컴포넌트 가져오기
	template<class T>
	T* Get_Component() const
	{
		unsigned int component_code = typeid(T).hash_code();
		return m_component_map.find(component_code) != m_component_map.end() ? dynamic_cast<T*>(m_component_map[component_code]) : nullptr;
	}

public:
	Transform* Get_Transform();
};