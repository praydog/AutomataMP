#pragma once

#include "Packets.hpp"

class Entity;

class Player {
public:
    void setPlayerData(const nier_client_and_server::PlayerData& movement) {
        m_playerData = movement;
    }

    auto& getPlayerData() {
        return m_playerData;
    }

    uint32_t getHandle() {
        return m_entityHandle;
    }
    
    void setHandle(uint32_t handle) {
        m_entityHandle = handle;
    }

    float getStartTick() {
        return m_startTick;
    }

    void setStartTick(float tick) {
        m_startTick = tick;
    }

    Entity* getEntity();

private:
    uint32_t m_entityHandle{ 0 };
    float m_startTick{ 0.0f };
    nier_client_and_server::PlayerData m_playerData;
};