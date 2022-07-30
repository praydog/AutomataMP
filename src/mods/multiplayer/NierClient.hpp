#pragma once

#include <enetpp/client.h>

struct Packet;

class NierClient : public enetpp::client {
public:
    NierClient(const std::string& host);
    ~NierClient();

    void think();
    bool isConnected() { return get_connection_state() == enetpp::CONNECT_CONNECTED; }

private:
    void onConnect();
    void onDisconnect();
    void onDataReceived(const Packet* data, size_t size);
};