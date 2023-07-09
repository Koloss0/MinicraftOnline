#pragma once

#include "network_device.hpp"
#include "message.hpp"
#include "connection.hpp"
#include <engine/events/event.hpp>
#include <engine/events/network_event.hpp>
#include <engine/core/log.hpp>

#include <olcNetwork/olcPGEX_Network.h>

#include <stdint.h>

namespace Engine
{
	using IServer = olc::net::server_interface<MessageID>;

	template <typename T>
	class Server : private IServer, public INetworkDevice
	{
	public:
		using validation_callback_fn = std::function<bool(std::shared_ptr<Connection> client)>;

		Server(uint16_t port)
			: IServer(port), INetworkDevice(),
			  m_running(false), m_validation_callback()
		{}

		virtual ~Server() = default;

		bool start()
		{
			m_running = IServer::Start();
			return m_running;
		}

		void stop()
		{
			m_running = false;
			IServer::Stop();
		}

		void send(const Message& message, std::shared_ptr<Connection> client)
		{
			IServer::MessageClient(client, message);
		}

		void send_all(const Message& message, std::shared_ptr<Connection> ignore_client = nullptr)
		{
			IServer::MessageAllClients(message, ignore_client);
		}

		virtual void on_update() override
		{
			IServer::Update();
		}

		inline void set_validation_callback(validation_callback_fn fn) { m_validation_callback = fn; }
		inline bool is_running() { return m_running; }
	private:
		virtual bool OnClientConnect(std::shared_ptr<Connection> client) override
		{
			bool is_valid = true;

			if (m_validation_callback)
			{
				is_valid = m_validation_callback(client);
			}

			return is_valid;
		}

		// Called when a client appears to have disconnected
		virtual void OnClientDisconnect(std::shared_ptr<Connection> client) override
		{
			ClientDisconnectedEvent e{0};
			m_event_callback(e);

			Message client_left_msg;
			client_left_msg.header.id = MessageID::CLIENT_DISCONNECTED;
			send_all(client_left_msg);
		}

		// Called when a message arrives
		virtual void OnMessage(std::shared_ptr<Connection> client, Message& msg) override
		{
			ClientMessageEvent e{0, msg};
			m_event_callback(e);
		}

		// Called when a client is validated
		virtual void OnClientValidated(std::shared_ptr<Connection> client) override
		{
			ClientConnectedToServerEvent e{0};
			m_event_callback(e);

			Message accept_msg;
			accept_msg.header.id = MessageID::ACCEPT_CONNECTION;
			send(accept_msg, client);

			Message client_joined_msg;
			client_joined_msg.header.id = MessageID::CLIENT_CONNECTED;
			send_all(client_joined_msg, client);
		}
	
	private:
		bool m_running;

		validation_callback_fn m_validation_callback;
	};
}
