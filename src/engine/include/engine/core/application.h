#pragma once

#include "window.h"
#include "layer_stack.h"
#include <engine/net/network_device.h>
#include <engine/net/server.h>
#include <engine/net/client.h>
#include <memory>

int main(int argc, char** argv);

class Application
{
public:
	Application();
	virtual ~Application();
	
	inline static Application& get() { return *s_instance; }

	virtual void on_event(Event& e);

	void shutdown();
	
	template <typename T>
	void create_server(uint16_t port)
	{
		engine::Server<T>* server = new engine::Server<T>(port); // TODO: remove scope resolution
		m_network_device = std::unique_ptr<engine::Server<T>>(server);
		server->set_event_callback(std::bind(&Application::on_event,
					this, std::placeholders::_1));
	}

	template <typename T>
	void create_client()
	{
		engine::Client<T>* client = new engine::Client<T>(); // TODO: remove scope resolution
		m_network_device = std::unique_ptr<engine::Client<T>>(client);
		client->set_event_callback(std::bind(&Application::on_event,
					this, std::placeholders::_1));
	}

	inline engine::INetworkDevice& get_network_device() const { return *m_network_device; }

protected:
	void push_layer(Layer* layer);
	virtual void on_update(double delta);

	LayerStack m_layer_stack;
private:
	void run();

	bool m_running;

	std::unique_ptr<engine::INetworkDevice> m_network_device;
	
	static Application* s_instance;
	
	friend int ::main(int argc, char** argv);
};

Application* create_application();
