#pragma once

#include "event.hpp"

#include <engine/net/message.hpp>
#include <engine/net/connection.hpp>
#include <engine/net/player_uid.hpp>

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

		inline PlayerUID get_client() { return m_client; }
		
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

		inline PlayerUID get_client() { return m_client; }
		
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
		ServerMessageEvent(const Message& message)
			: m_message{message}
		{}

		inline const Message& get_message() { return m_message; }
		
		EVENT_CLASS_TYPE(ServerMessage)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ServerMessage: msg: " << static_cast<int>(m_message.header.id)
				<< ", size: " << m_message.header.size << "]";

			return ss.str();
		}

	private:
		const Message& m_message;
	};

	class ClientMessageEvent: public NetworkEvent
	{
	public:
		ClientMessageEvent(PlayerUID client, const Message& message)
			: m_client{client}, m_message{message}
		{}

		inline PlayerUID get_client() { return m_client; }
		inline const Message& get_message() { return m_message; }
		
		EVENT_CLASS_TYPE(ClientMessage)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ClientMessage: sender: " << m_client
				<< ", msg: " << static_cast<int>(m_message.header.id)
				<< ", size: " << m_message.header.size << "]";

			return ss.str();
		}

	private:
		PlayerUID m_client;
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
