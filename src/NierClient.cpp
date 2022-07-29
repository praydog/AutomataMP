#include <thread>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "Packets.hpp"
#include "mods/AutomataMPMod.hpp"
#include "NierClient.hpp"

using namespace std;

NierClient::NierClient(const std::string& host)
{
    set_trace_handler([](const std::string& s) { spdlog::info("{}", s); });
    connect(enetpp::client_connect_params().set_channel_count(1).set_server_host_name_and_port(host.c_str(), 6969).set_timeout(chrono::seconds(1)));

    while (get_connection_state() == enetpp::CONNECT_CONNECTING) {
        think();
        this_thread::yield();
    }
}

NierClient::~NierClient()
{
    disconnect();
}

void NierClient::think()
{
    consume_events(
        [this]() { onConnect(); },
        [this]() { onDisconnect(); },
        [this](auto a, auto b) { onDataReceived((const Packet*)a, b); }
    );
}

void NierClient::onConnect()
{
    spdlog::set_default_logger(spdlog::basic_logger_mt("AutomataMPClient", "automatamp_clientlog.txt", true));
    spdlog::info("Connected");
}

void NierClient::onDisconnect()
{
    spdlog::info("Disconnected");
}

void NierClient::onDataReceived(const Packet* data, size_t size)
{
    if (data->id >= ID_SHARED_START && data->id < ID_SHARED_END) {
        AutomataMPMod::get()->sharedPacketProcess(data, size);
    }
    else if (data->id >= ID_SERVER_START && data->id < ID_SERVER_END) {
        AutomataMPMod::get()->serverPacketProcess(data, size);
    }
}
