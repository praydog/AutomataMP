#include <iostream>
#include <string>
#include <random>
#include <conio.h>
#include "enetpp/client.h"
#include "enetpp/server.h"

static const int CLIENT_COUNT = 10;
static const int PORT = 123;

static bool s_exit = false;
static std::mutex s_cout_mutex;

class server_client {
public:
	unsigned int _uid;

public:
	server_client()
		: _uid(0) {
	}

	unsigned int get_uid() const {
		return _uid;
	}
};

void run_server() {	
	auto trace_handler = [&](const std::string& msg) {
		std::lock_guard<std::mutex> lock(s_cout_mutex);
		std::cout << "server: " << msg << std::endl;
	};

	enetpp::server<server_client> server;
	server.set_trace_handler(trace_handler);

	unsigned int next_uid = 0;
	auto init_client_func = [&](server_client& client, const char* ip) {
		client._uid = next_uid;
		next_uid++;
	};

	server.start_listening(enetpp::server_listen_params<server_client>()
		.set_max_client_count(CLIENT_COUNT)
		.set_channel_count(1)
		.set_listen_port(PORT)
		.set_initialize_client_function(init_client_func));

	while (server.is_listening()) {

		auto on_client_connected = [&](server_client& client) { trace_handler("on_client_connected"); };
		auto on_client_disconnected = [&](unsigned int client_uid) { trace_handler("on_client_disconnected"); };
		auto on_client_data_received = [&](server_client& client, const enet_uint8* data, size_t data_size) {
			trace_handler("received packet from client : '" + std::string(reinterpret_cast<const char*>(data), data_size) + "'");
			trace_handler("forwarding packet to all other clients...");
			server.send_packet_to_all_if(0, data, data_size, ENET_PACKET_FLAG_RELIABLE, [&](const server_client& destination) {
				return destination.get_uid() != client.get_uid();
			});
		};

		server.consume_events(
			on_client_connected, 
			on_client_disconnected, 
			on_client_data_received);

		if (s_exit) {
			server.stop_listening();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void run_client(int client_index) {
	auto trace_handler = [&](const std::string& msg) {
		std::lock_guard<std::mutex> lock(s_cout_mutex);
		std::cout << "client(" << client_index << "): " << msg << std::endl;
	};
	
	enetpp::client client;
	client.set_trace_handler(trace_handler);
	client.connect(enetpp::client_connect_params()
		.set_channel_count(1)
		.set_server_host_name_and_port("localhost", PORT));

	std::mt19937 rand;
	rand.seed(static_cast<unsigned long>(client_index));
	std::uniform_int_distribution<> rand_distribution(5000, 20000);

	auto last_send_time = std::chrono::system_clock::now();
	unsigned int next_send_time_delta = rand_distribution(rand);

	while (client.is_connecting_or_connected()) {

		if (std::chrono::system_clock::now() - last_send_time > std::chrono::milliseconds(next_send_time_delta)) {
			last_send_time = std::chrono::system_clock::now();
			next_send_time_delta = rand_distribution(rand);
			trace_handler("sending packet to server");
			std::string packet = "hello from client:" + std::to_string(client_index);
			assert(sizeof(char) == sizeof(enet_uint8));
			client.send_packet(0, reinterpret_cast<const enet_uint8*>(packet.data()), packet.length(), ENET_PACKET_FLAG_RELIABLE);
		}

		auto on_connected = [&](){ trace_handler("on_connected"); };
		auto on_disconnected = [&]() { trace_handler("on_disconnected"); };
		auto on_data_received = [&](const enet_uint8* data, size_t data_size) {
			trace_handler("received packet from server : '" + std::string(reinterpret_cast<const char*>(data), data_size) + "'");
		};

		client.consume_events(
			on_connected,
			on_disconnected,
			on_data_received);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

int main(int argc, char** argv) {
	
	enetpp::global_state::get().initialize();

	auto server_thread = std::make_unique<std::thread>(&run_server);
	
	std::vector<std::unique_ptr<std::thread>> client_threads;
	for (int i = 0; i < CLIENT_COUNT; ++i) {
		client_threads.push_back(std::make_unique<std::thread>(&run_client, i));
	}

	std::cout << "press any key to exit..." << std::endl;
	_getch();
	s_exit = true;
	server_thread->join();
	for (auto& ct : client_threads) {
		ct->join();
	}

	enetpp::global_state::get().deinitialize();

	return 0;
}

