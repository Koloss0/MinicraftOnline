#pragma once

#include <olcNetwork/olcPGEX_Network.h>

namespace engine
{
	template <typename T>
	using Connection = olc::net::connection<T>;
}
