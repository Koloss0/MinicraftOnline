#pragma once

#include "message.hpp"

#include <olcNetwork/olcPGEX_Network.h>

namespace Engine
{
	using Connection = olc::net::connection<MessageID>;
}
