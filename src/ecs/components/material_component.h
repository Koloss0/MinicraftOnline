// material_component.h
#pragma once

#include "component.h"
#include "../../renderer/material.h"

struct MaterialComponent: public Component
{
	Material* material = nullptr;
};
