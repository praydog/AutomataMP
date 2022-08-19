#pragma once

#include "regenny/BehaviorAppBase.hpp"
#include "ScriptFunctions.hpp"
#include "Behavior.hpp"

#include "Hw/cVec4.hpp"
#include "hap/VariantMap.hpp"
#include "sys/StringSystem.hpp"
#include "lib/HashedString.hpp"

namespace sdk {
class BehaviorAppBase : public sdk::Behavior {
public:
    static constexpr std::string_view class_name() {
        return "class BehaviorAppBase";
    }
    
public:
    __forceinline ::regenny::BehaviorAppBase* regenny() const {
        return (::regenny::BehaviorAppBase*)this;
    }

    __forceinline uint32_t& health() {
        return regenny()->health;
    }

public:
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, getHp, int) // base + 0x4643d8
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, getHpMax, int) // base + 0x4643cc
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, getHpRate100, float) // base + 0x491430
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, pullout, bool) // base + 0x464348
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestAnim, void, int) // base + 0x464360
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestAnimOp, void, class lib::HashedString<struct sys::StringSystem::Allocator> const &, class hap::VariantMap const &) // base + 0x464378
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestAnimSpdRate, void, float) // base + 0x4e62a0
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestAnimTable, void, class lib::HashedString<struct sys::StringSystem::Allocator> const &) // base + 0x46436c
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestMove, void, class Hw::cVec4 const &, int) // base + 0x464384
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestPath, void, int) // base + 0x464390
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestPathBlockStop, void) // base + 0x4643a8
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestPathWait, void) // base + 0x46439c
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestRoute, void, class app::Puid const &, int) // base + 0x4643b4
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestRouteOp, void, class app::Puid const &, class hap::VariantMap const &) // base + 0x464324
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestTalk, void, int) // base + 0x464330
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestTurn, void, class app::Puid const &) // base + 0x4643c0
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestWait, void) // base + 0x4e6610
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, searchOn, bool, bool) // base + 0x46433c
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, setLevel, bool, int) // base + 0x464354
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, setTalkDisable, void) // base + 0x4f5ec0
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, setTalkEnable, void) // base + 0x4f5ed0
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, talkDisable, int const) // base + 0x4fe300
    
public:
    static constexpr uint8_t s_request_route_op_index = 100;
    static constexpr uint8_t s_request_talk_index = s_request_route_op_index + 1;
    static constexpr uint8_t s_search_on_index = s_request_talk_index + 1;
    static constexpr uint8_t s_pullout_index = s_search_on_index + 1;
    static constexpr uint8_t s_set_level_index = s_pullout_index + 1;

    static constexpr uint8_t s_request_anim_index = 112;
    static constexpr uint8_t s_request_anim_table_index = s_request_anim_index + 1;
    static constexpr uint8_t s_request_anim_op_index = s_request_anim_table_index + 2;
    static constexpr uint8_t s_request_move_index = s_request_anim_op_index + 1;

    static constexpr uint8_t s_request_path_index = s_request_move_index + 1;
    static constexpr uint8_t s_request_path_wait_index = s_request_path_index + 1;
    static constexpr uint8_t s_request_path_block_stop_index = s_request_path_wait_index + 1;
    static constexpr uint8_t s_request_route_index = s_request_path_block_stop_index + 1;

private:
    uint8_t detail_behavior_app_base_data[sizeof(::regenny::BehaviorAppBase) - sizeof(sdk::Behavior)];
};

static_assert(sizeof(BehaviorAppBase) == 0xC50, "Size of BehaviorAppBase is not correct.");
}