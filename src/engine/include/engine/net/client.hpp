#pragma once

#include "engine/core/log.hpp"
#include "message.hpp"
#include "network_device.hpp"
#include <engine/events/network_event.hpp>

#include <olcNetwork/olcPGEX_Network.h>

#include <stdint.h>

namespace Engine
{
	using IClient = olc::net::client_interface<MessageID>;

	template <typename T>
	class Client : private IClient, public INetworkDevice
	{
	public:
		Client()
			: IClient(), INetworkDevice()
		{}

		virtual ~Client() = default;

		bool connect_to_host(const std::string& ip, uint16_t port)
		{
			return Connect(ip, port);
		}

		void disconnect()
		{
			Disconnect();
		}

		bool is_connected()
		{
			return IsConnected();
		}

		void send(const Message& message)
		{
			Send(message);
		}

		virtual void on_update()
		{
			if (is_connected())
			{
				while(!Incoming().empty())
				{
					olc::net::message<MessageID> msg = Incoming().pop_front().msg;

					switch (msg.header.id)
					{
						case MessageID::ACCEPT_CONNECTION:
						{
							ConnectedToServerEvent e;
							m_event_callback(e);
							break;
						}
						case MessageID::REJECT_CONNECTION:
						{
							FailedToConnectEvent e{"Server rejected request to join."};
							m_event_callback(e);
							break;
						}
						case MessageID::CLIENT_CONNECTED:
						{
							ClientConnectedToServerEvent e{0};
							m_event_callback(e);
							break;
						}
						case MessageID::CLIENT_DISCONNECTED:
						{
							ClientDisconnectedEvent e{0};
							m_event_callback(e);
							break;
						}
						case MessageID::SERVER_MESSAGE:
						{
							ServerMessageEvent e{msg};
							m_event_callback(e);
							break;
						}
						case MessageID::CLIENT_MESSAGE:
						{
							ClientMessageEvent e{0,msg};
							m_event_callback(e);
							break;
						}
						case MessageID::SERVER_CLOSED:
						{
							ServerDisconnectedEvent e{"Server closed."};
							m_event_callback(e);
							break;
						}
						default:
						{
							LOG_WARN("Recieved message of unexpected type ({0})", static_cast<int>(msg.header.id));
						}

					}
				}

			}
		}
	};
}
