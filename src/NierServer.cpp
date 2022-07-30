#include <functional>

#include <imgui.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "Packets.hpp"
#include "AutomataMP.hpp"
#include "mods/AutomataMPMod.hpp"
#include "NierServer.hpp"

using namespace std;
using namespace std::placeholders;

NierServer::NierServer() {
    spdlog::set_default_logger(spdlog::basic_logger_mt("AutomataMPServer", "automatamp_serverlog.txt", true));

    start_listening(enetpp::server_listen_params<NierServerClient>().set_max_client_count(2)
        .set_channel_count(1)
        .set_listen_port(6969)
        .set_initialize_client_function([this](auto& a, auto b) { initClient(a, b); }));

    spdlog::info("Server created");
}

NierServer::~NierServer() {
    stop_listening();
}

void NierServer::on_draw_ui() {
    for (auto c : get_connected_clients()) {
        ImGui::Text("Client: %d", c->get_id());
    }
}

void NierServer::think() {
    consume_events(
        [this](auto& a) { onClientConnect(a); },
        [this](auto a) { onClientDisconnect(a); },
        [this](auto& a, auto b, auto c) { onClientDataReceived(a, (const Packet*)b, c); }
    );

    for (auto c : get_connected_clients()) {
        
    }
}

void NierServer::initClient(NierServerClient& client, const char* ip) {
    client.uid = m_uid++;
}

void NierServer::onClientConnect(NierServerClient& client) {
    spdlog::info("Client connect");
}

void NierServer::onClientDisconnect(uint32_t uid) {
    spdlog::info("Client disconnect");
}

void NierServer::onClientDataReceived(NierServerClient& client, const Packet* data, size_t size) {
    if (data->id >= ID_SHARED_START && data->id < ID_SHARED_END) {
        AutomataMPMod::get()->sharedPacketProcess(data, size);
    }
}
