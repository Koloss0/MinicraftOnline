#pragma once

#include <olcNetwork/olcPGEX_Network.h>

namespace Engine
{
	enum class MessageID: uint8_t
	{
		ACCEPT_CONNECTION, // client has been validated.
		REJECT_CONNECTION, // client failed validation.
		CLIENT_CONNECTED, // another client has joined.
		CLIENT_DISCONNECTED, // another client has left.
		SERVER_MESSAGE, // sent by the server
		CLIENT_MESSAGE, // sent by another client. (relayed by server)
		SERVER_CLOSED, // the server is now shut down
	};

	using Message = olc::net::message<MessageID>;
}
