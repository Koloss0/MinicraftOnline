#pragma once

#include "network_device.h"
#include "message.h"
#include "connection.h"
#include <engine/events/event.h>
#include <engine/events/network_event.h>
#include <engine/core/log.h>

#include <olcNetwork/olcPGEX_Network.h>

#include <stdint.h>

namespace engine
{
	template <typename T>
	using IServer = olc::net::server_interface<T>;

	template <typename T>
	class Server : private IServer<T>, public INetworkDevice
	{
	public:
		using validation_callback_fn = std::function<bool(std::shared_ptr<Connection<T>> client)>;

		Server(uint16_t port)
			: IServer<T>(port), INetworkDevice(),
			  m_running(false), m_validation_callback()
		{}

		virtual ~Server() = default;

		bool start()
		{
			m_running = IServer<T>::Start();
			return m_running;
		}

		void stop()
		{
			m_running = false;
			IServer<T>::Stop();
		}

		void send(const Message<T>& message, std::shared_ptr<Connection<T>> client)
		{
			IServer<T>::MessageClient(client, message);
		}

		void send_all(const Message<T>& message, std::shared_ptr<Connection<T>> ignore_client = nullptr)
		{
			IServer<T>::MessageAllClients(message, ignore_client);
		}

		virtual void on_update() override
		{
			IServer<T>::Update();
		}

		inline void set_validation_callback(validation_callback_fn fn) { m_validation_callback = fn; }
		inline bool is_running() { return m_running; }
	private:
		virtual bool OnClientConnect(std::shared_ptr<Connection<T>> client) override
		{
			bool is_valid = true;

			if (m_validation_callback)
			{
				is_valid = m_validation_callback(client);
			}

			if (is_valid)
			{
				ClientConnectionEvent<T> e{client};
				m_event_callback(e);
			}

			return is_valid;
		}

		// Called when a client appears to have disconnected
		virtual void OnClientDisconnect(std::shared_ptr<Connection<T>> client) override
		{
			LOG_INFO("player gon");
			ClientDisconnectionEvent<T> e{client};
			m_event_callback(e);
		}

		// Called when a message arrives
		virtual void OnMessage(std::shared_ptr<Connection<T>> client, Message<T>& msg) override
		{
			ClientMessageEvent<T> e{client, msg};
			m_event_callback(e);
		}

		// Called when a client is validated
		virtual void OnClientValidated(std::shared_ptr<Connection<T>> client) override
		{
			ClientValidationEvent<T> e{client};
			m_event_callback(e);
		}
	
	private:
		bool m_running;

		validation_callback_fn m_validation_callback;
	};
}
