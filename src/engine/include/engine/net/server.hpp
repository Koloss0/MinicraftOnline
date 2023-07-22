#pragma once

#include "engine/net/player_uid.hpp"
#include "network_device.hpp"
#include "message.hpp"
#include "connection.hpp"
#include <engine/events/event.hpp>
#include <engine/events/network_event.hpp>
#include <engine/core/log.hpp>

#include <memory>
#include <olcNetwork/olcPGEX_Network.h>

#include <stdint.h>
#include <unordered_map>

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
			  m_running(false), m_validation_callback(),
			  m_player_uids(),
			  m_connections(),
			  m_garbage_connections()
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

		void send(Message& message, PlayerUID player_uid)
		{
			ASSERT(m_connections.find(player_uid) != m_connections.end(), \
				"Attempt to send message with invalid player UID");

			LOG_INFO("sending message to client {0}: [size: {1}]",\
				static_cast<int>(player_uid), message.header.size);

			std::shared_ptr<Connection>& client = m_connections[player_uid];
			message.header.id = MessageID::GAME_MESSAGE;
			IServer::MessageClient(client, message);
		}

		void send_all(Message& message, PlayerUID ignore_uid = 0)
		{
			message.header.id = MessageID::GAME_MESSAGE;
			if (ignore_uid)
			{
				LOG_INFO("sending message to all clients except {0}: [size: {1}]",\
					ignore_uid, message.header.size);
				std::shared_ptr<Connection>& client = m_connections[ignore_uid];
				IServer::MessageAllClients(message, client);
			}
			else
			{
				LOG_INFO("sending message to all clients: [size: {0}]", message.header.size);
				IServer::MessageAllClients(message);
			}
			
		}

		virtual void on_update() override
		{
			if (!m_garbage_connections.empty())
			{
				for (auto cid : m_garbage_connections)
				{
					PlayerUID puid = m_player_uids[cid];
					m_player_uids.erase(cid);
					m_connections.erase(puid);

					// transmit event
					ClientDisconnectedEvent e{puid};
					m_event_callback(e);

					// inform other clients of disconnected client
					Message client_disconnected;
					client_disconnected.header.id = MessageID::CLIENT_DISCONNECTED;
					client_disconnected << puid;
					IServer::MessageAllClients(client_disconnected);
				}
				m_garbage_connections.clear();
			}

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
			if (m_player_uids.find(client->GetID()) != m_player_uids.end())
			{
				m_garbage_connections.push_back(client->GetID());
			}
		}

		// Called when a message arrives
		virtual void OnMessage(std::shared_ptr<Connection> client, Message& msg) override
		{
			switch (msg.header.id)
			{
				case MessageID::REGISTER:
				{
					if (msg.size() == sizeof(PlayerUID))
					{
						PlayerUID puid;
						msg >> puid;
			
						if (m_player_uids.find(client->GetID()) != m_player_uids.end())
						{
							LOG_WARN("Player {0} attempted to register twice.", static_cast<uint32_t>(puid));
							// TODO
							/*
							Message rejection;
							rejection.header.id = MessageID::REJECT_CONNECTION;
							rejection << "You are already logged in on this server.";
							IServer::MessageClient(client, rejection);
							*/
						}
						else
						{
							m_player_uids[client->GetID()] = puid;
							m_connections[puid] = client;
							LOG_INFO("Registered new player. (PUID: {0})", static_cast<uint32_t>(puid));

							// inform other clients of new connection
							Message new_connection;
							new_connection.header.id = MessageID::CLIENT_CONNECTED;
							new_connection << puid;
							IServer::MessageAllClients(new_connection, client);
					
							// transmit event
							ClientConnectedToServerEvent e{puid};
							m_event_callback(e);
						}
					}
					else
					{
						LOG_WARN("Received invalid player registration message.");
					}
					break;
				}
				case MessageID::GAME_MESSAGE:
				{
					if (m_player_uids.find(client->GetID()) != m_player_uids.end())
					{
						PlayerUID puid = m_player_uids[client->GetID()];
						ClientMessageEvent e{puid, msg};
						m_event_callback(e);
					}
					else
					{
						LOG_WARN("Received game message from unregistered player.");
					}
					break;
				}
				default:
				{
					LOG_WARN("Recieved message of unexpected type ({0})", static_cast<int>(msg.header.id));
				}
			}
		}

		// Called when a client is validated
		virtual void OnClientValidated(std::shared_ptr<Connection> client) override
		{
			Message accept_msg;
			accept_msg.header.id = MessageID::ACCEPT_CONNECTION;
			IServer::MessageClient(client, accept_msg);
		}
	
	private:
		bool m_running;
		validation_callback_fn m_validation_callback;
		
		std::unordered_map<uint32_t, PlayerUID> m_player_uids;
		std::unordered_map<PlayerUID, std::shared_ptr<Connection>> m_connections;
		std::vector<uint32_t> m_garbage_connections;
	};
}
