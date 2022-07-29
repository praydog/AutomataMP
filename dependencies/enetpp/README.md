enetpp
---
A header only C++11 networking library that internally uses [ENet](http://enet.bespin.org/)

This is an opinionated wrapper. Instead of raw ENet functionality, client and server objects are provided. Both of which spawn a worker thread to make sure networking communication is never interupted by the main thread.

Getting Started
---
1. clone this repository
2. clone enet's repository (https://github.com/lsalzman/enet)
3. build enet as a library and link it to your project
4. add additional include directories to your project : enet/include and enetpp/include

Global state
---
Unfortunately enet requires some global state to be initialized / deinitialized. On windows this is winsock.

```c++
enetpp::global_state::get().initialize();
enetpp::global_state::get().deinitialize();
```

Using enetpp::client
---
The enetpp::client object is nice and simple.

```c++
#include "enetpp/client.h"

enetpp::client client;
client.connect(enetpp::client_connect_params()
	.set_channel_count(1)
	.set_server_host_name_and_port("localhost", 801));

while (game_is_running) {
	//send stuff
	enet_uint8 data_to_send;
	client.send_packet(0, &data_to_send, 1, ENET_PACKET_FLAG_RELIABLE);

	//consume events raised by worker thread
	auto on_connected = [&](){};
	auto on_disconnected = [&](){};
	auto on_data_received = [&](const enet_uint8* data, size_t data_size){};
	client.consume_events(on_connected, on_disconnected, on_data_received);
}

client.disconnect();
```

Using enetpp::server
---
The enetpp::server object is a bit more complicated than the client. It is templatized on a type that represents and stores information for each connected client. It's only requirement is the get_uid() member function. See sample code. This is where games should store game specific information per client. (ex. lobby state)

```c++
#include "enetpp/server.h"

struct server_client {
	unsigned int _uid;
	unsigned int get_uid() const { return _uid; } //MUST return globally unique value here
};

unsigned int next_uid = 0;
auto init_client_func = [&](server_client& client, const char* ip) {
	client._uid = next_uid;
	next_uid++;
};

enetpp::server<server_client> server;
server.start_listening(enetpp::server_listen_params<server_client>()
	.set_max_client_count(20)
	.set_channel_count(1)
	.set_listen_port(801)
	.set_initialize_client_function(init_client_func));

while (game_is_running) {
	//send stuff to specific client where uid=123
	enet_uint8 data_to_send;
	server.send_packet_to(123, 0, &data_to_send, 1, ENET_PACKET_FLAG_RELIABLE);

	//send stuff to all clients (with optional predicate filter)
	server.send_packet_to_all_if(0, &data_to_send, 1, ENET_PACKET_FLAG_RELIABLE, [](const server_client&){});

	//consume events raised by worker thread
	auto on_client_connected = [&](server_client& client) {};
	auto on_client_disconnected = [&](unsigned int client_uid) {};
	auto on_client_data_received = [&](server_client& client, const enet_uint8* data, size_t data_size) {};
	server.consume_events(on_connected, on_disconnected, on_data_received);

	//get access to all connected clients
	for (auto c : server.get_connected_clients()) {
		//do something?
	}
}

server.stop_listening();
```

Extra
---
Scan through client.h and server.h for more advanced features.
- trace handler
- more params (ex. timeout)
	
Todo
---
- expose metrics and counters (ex. average roundtrip time)	

Sample app
---
There is a sample app included that will spawn a server and client and send messages from client->server->other clients randomly. Use CMake to build. Requires the variable ENET_ROOT_PATH to be set.

License
---
MIT
