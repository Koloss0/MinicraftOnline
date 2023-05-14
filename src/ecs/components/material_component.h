// material_component.h
#pragma once

#include "component.h"
#include <src/renderer/material.h>

struct MaterialComponent: public Component
{
	Material* material = nullptr;
};
