#pragma once

#include "engine/core/log.hpp"
#include "engine/net/player_uid.hpp"
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
			: IClient(), INetworkDevice(),
			m_puid(0)
		{
			m_puid = static_cast<PlayerUID>(std::time(nullptr));
		}

		virtual ~Client() = default;

		PlayerUID get_network_unique_id() { return m_puid; }

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

		void send(Message& message)
		{
			LOG_INFO("sending game message: [size: {0}]", static_cast<int>(message.header.size));
			message.header.id = MessageID::GAME_MESSAGE;
			IClient::Send(message);
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
							Message register_me;
							register_me.header.id = MessageID::REGISTER;
							register_me << m_puid;

							IClient::Send(register_me);

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
							if (msg.size() == sizeof(PlayerUID))
							{
								PlayerUID puid;
								msg >> puid;

								ClientConnectedToServerEvent e{puid};
								m_event_callback(e);
							}
							else
							{
								LOG_WARN("Received invalid client connection message.");
							}
							break;
						}
						case MessageID::CLIENT_DISCONNECTED:
						{
							if (msg.size() == sizeof(PlayerUID))
							{
								PlayerUID puid;
								msg >> puid;

								ClientDisconnectedEvent e{puid};
								m_event_callback(e);
							}
							else
							{
								LOG_WARN("Received invalid client disconnection message.");
							}
							break;
						}
						case MessageID::GAME_MESSAGE:
						{
							ServerMessageEvent e{msg};
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

	private:
		PlayerUID m_puid;
	};
}
