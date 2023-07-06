#pragma once

#include "message.hpp"
#include "network_device.hpp"
#include <engine/events/network_event.hpp>

#include <olcNetwork/olcPGEX_Network.h>

#include <stdint.h>

namespace engine
{
	template <typename T>
	using IClient = olc::net::client_interface<T>;

	template <typename T>
	class Client : private IClient<T>, public INetworkDevice
	{
	public:
		Client()
			: IClient<T>(), INetworkDevice()
		{}

		virtual ~Client() = default;

		bool connect_to_host(const std::string& ip, uint16_t port)
		{
			return IClient<T>::Connect(ip, port);
		}

		void disconnect()
		{
			IClient<T>::Disconnect();
		}

		bool is_connected()
		{
			return IClient<T>::IsConnected();
		}

		void send(const Message<T>& message)
		{
			IClient<T>::Send(message);
		}

		virtual void on_update()
		{
			if (is_connected())
			{
				while(!IClient<T>::Incoming().empty())
				{
					auto msg = IClient<T>::Incoming().pop_front().msg;

					engine::ServerMessageEvent<T> e{msg};
					m_event_callback(e);
				}

			}
		}
	};
}
