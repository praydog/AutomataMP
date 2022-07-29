#pragma once

#include <enetpp/server.h>

struct Packet;

struct NierServerClient {
    uint32_t uid;
    uint32_t get_id() const { return uid; } //MUST return globally unique value here
};

class NierServer : public enetpp::server<NierServerClient> {
public:
    NierServer();
    ~NierServer();

    void think();

private:
    void initClient(NierServerClient& client, const char* ip);
    void onClientConnect(NierServerClient& client);
    void onClientDisconnect(uint32_t uid);
    void onClientDataReceived(NierServerClient& client, const Packet* data, size_t size);

private:
    uint32_t m_uid{ 1 };
};