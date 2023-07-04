#pragma once

#include "event.h"
#include <engine/net/message.h>
#include <engine/net/connection.h>

#include <sstream>

namespace engine
{
	class NetworkEvent: public Event
	{};

	template <typename T>
	class ClientConnectionEvent: public NetworkEvent
	{
	public:
		ClientConnectionEvent(std::shared_ptr<Connection<T>> client)
			: m_client{client}
		{}

		inline std::shared_ptr<Connection<T>> get_client() { return m_client; }
		
		EVENT_CLASS_TYPE(ClientConnection)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ClientConnection]";

			return ss.str();
		}
	private:
		std::shared_ptr<Connection<T>> m_client;
	};

	template <typename T>
	class ClientDisconnectionEvent: public NetworkEvent
	{
	public:
		ClientDisconnectionEvent(std::shared_ptr<Connection<T>> client)
			: m_client{client}
		{}

		inline std::shared_ptr<Connection<T>> get_client() { return m_client; }
		
		EVENT_CLASS_TYPE(ClientDisconnection)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ClientDisconnection: " << m_client->GetID() << "]";

			return ss.str();
		}
	private:
		std::shared_ptr<Connection<T>> m_client;
	};

	template <typename T>
	class ClientMessageEvent: public NetworkEvent
	{
	public:
		ClientMessageEvent(std::shared_ptr<Connection<T>> client, const Message<T>& message)
			: m_client{client}, m_message{message}
		{}

		inline std::shared_ptr<Connection<T>> get_client() { return m_client; }
		inline const Message<T>& get_message() { return m_message; }
		
		EVENT_CLASS_TYPE(ClientMessage)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ClientMessage: sender: " << m_client->GetID()
				<< ", msg: " << static_cast<int>(m_message.header.id)
				<< ", size: " << m_message.header.size << "]";

			return ss.str();
		}

	private:
		std::shared_ptr<Connection<T>> m_client;
		const Message<T>& m_message;
	};

	template <typename T>
	class ClientValidationEvent: public NetworkEvent
	{
	public:
		ClientValidationEvent(std::shared_ptr<Connection<T>> client)
			: m_client{client}
		{}

		inline std::shared_ptr<Connection<T>> get_client() { return m_client; }
		
		EVENT_CLASS_TYPE(ClientValidation)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ClientValidation: " << m_client->GetID() << "]";

			return ss.str();
		}
	private:
		std::shared_ptr<Connection<T>> m_client;
	};
	
	template <typename T>
	class ServerMessageEvent: public NetworkEvent
	{
	public:
		ServerMessageEvent(const Message<T>& message)
			: m_message{message}
		{}

		inline const Message<T>& get_message() { return m_message; }
		
		EVENT_CLASS_TYPE(ServerMessage)
		
		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "[ServerMessage: msg: " << static_cast<int>(m_message.header.id)
				<< ", size: " << m_message.header.size << "]";

			return ss.str();
		}

	private:
		const Message<T>& m_message;
	};
}
