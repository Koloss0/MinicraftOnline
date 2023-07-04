#pragma once

#include <olcNetwork/olcPGEX_Network.h>

namespace engine
{
	template <typename T>
	using Message = olc::net::message<T>;
}
