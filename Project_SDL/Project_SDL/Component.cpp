#include "Component.h"

Object* Component::Get_Object() const
{
	return m_object;
}

void Component::Set_Object(Object* a_object)
{
	m_object = a_object;
}