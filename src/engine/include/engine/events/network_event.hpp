#pragma once

#include "event.hpp"

#include <X11/extensions/randr.h>
#include <engine/net/message.hpp>
#include <engine/net/connection.hpp>
#include <engine/net/player_uid.hpp>

#include <memory>
#include <sstream>

namespace Engine
{
	class NetworkEvent: public Event
	{};
	
	class ConnectedToServerEvent: public NetworkEvent
	{
	public:
		EVENT_CLASS_TYPE(ConnectedToServer)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ConnectedToServer]";

			return ss.str();
		}
	};
	
	class ClientConnectedToServerEvent: public NetworkEvent
	{
	public:
		ClientConnectedToServerEvent(PlayerUID client)
			: m_client{client}
		{}

		inline PlayerUID get_player_uid() { return m_client; }
		
		EVENT_CLASS_TYPE(ClientConnectedToServer)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ClientConnectedToServer: " << m_client << "]";

			return ss.str();
		}
	private:
		PlayerUID m_client;
	};
	
	class ClientDisconnectedEvent: public NetworkEvent
	{
	public:
		ClientDisconnectedEvent(PlayerUID client)
			: m_client{client}
		{}

		inline PlayerUID get_player_uid() { return m_client; }
		
		EVENT_CLASS_TYPE(ClientDisconnected)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ClientDisconnected: " << m_client << "]";

			return ss.str();
		}
	private:
		PlayerUID m_client;
	};

	
	class FailedToConnectEvent: public NetworkEvent
	{
	public:
		FailedToConnectEvent(const std::string& info)
			: m_info(info)
		{}

		const std::string& get_info() { return m_info; }

		EVENT_CLASS_TYPE(FailedToConnect)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[FailedToConnect: " << m_info << "]";

			return ss.str();
		}

	private:
		const std::string& m_info;
	};

	class ServerMessageEvent: public NetworkEvent
	{
	public:
		ServerMessageEvent(Message& message)
			: m_message{message}
		{}

		inline Message& get_message() { return m_message; }
		
		EVENT_CLASS_TYPE(ServerMessage)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ServerMessage: msg: " << static_cast<int>(m_message.header.id)
				<< ", size: " << m_message.header.size << "]";

			return ss.str();
		}

	private:
		Message& m_message;
	};

	class ClientMessageEvent: public NetworkEvent
	{
	public:
		ClientMessageEvent(PlayerUID sender, const Message& message)
			: m_sender{sender}, m_message{message}
		{}

		inline PlayerUID get_sender_uid() { return m_sender; }
		inline const Message& get_message() { return m_message; }
		
		EVENT_CLASS_TYPE(ClientMessage)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ClientMessage: sender: " << m_sender
				<< ", msg: " << static_cast<int>(m_message.header.id)
				<< ", size: " << m_message.header.size << "]";

			return ss.str();
		}

	private:
		PlayerUID m_sender;
		const Message& m_message;
	};
	
	class ServerDisconnectedEvent: public NetworkEvent
	{
	public:
		ServerDisconnectedEvent(const std::string& info)
			: m_info(info)
		{}

		const std::string& get_info() { return m_info; }

		EVENT_CLASS_TYPE(ServerDisconnected)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ServerDisconnected: " << m_info << "]";

			return ss.str();
		}

	private:
		const std::string& m_info;
	};
}
