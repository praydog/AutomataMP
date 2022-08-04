#pragma once

#include "schema/Packets_generated.h"
#include "Packets.hpp"

class Entity;

class Player {
public:
    void setGuid(uint64_t guid) {
        m_guid = guid;
    }

    uint64_t getGuid() const {
        return m_guid;
    }
    
    void setName(const std::string& name) {
        m_name = name;
    }

    const std::string& getName() const {
        return m_name;
    }

    void setPlayerData(const nier::PlayerData& movement) {
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
    std::string m_name{};
    uint64_t m_guid{};
    uint32_t m_entityHandle{ 0 };
    float m_startTick{ 0.0f };
    nier::PlayerData m_playerData;
};