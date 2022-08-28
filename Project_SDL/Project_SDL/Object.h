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

	//�ڽ� ������Ʈ �߰�
	void Add_Child(Object* a_child);
	//�θ� ������Ʈ ����
	void Set_Parent(Object* a_parent);

	//������Ʈ �߰��ϱ�
	void Add_Component(Component* a_component);
	//������Ʈ ��������
	template<class T>
	T* Get_Component() const
	{
		unsigned int component_code = typeid(T).hash_code();
		return m_component_map.find(component_code) != m_component_map.end() ? dynamic_cast<T*>(m_component_map[component_code]) : nullptr;
	}

public:
	Transform* Get_Transform();
};