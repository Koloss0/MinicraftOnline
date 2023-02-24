// component.h
#pragma once

class Component
{
};

extern int s_component_counter;
template <class T>
int get_id()
{
	static int s_component_id = s_component_counter++;
	return s_component_id;
}
