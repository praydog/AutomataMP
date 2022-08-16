#pragma once

#include "regenny/BehaviorAppBase.hpp"
#include "ScriptFunctions.hpp"
#include "Behavior.hpp"

namespace sdk {
class BehaviorAppBase : public sdk::Behavior {
public:
    __forceinline ::regenny::BehaviorAppBase* regenny() const {
        return (::regenny::BehaviorAppBase*)this;
    }

    __forceinline uint32_t& health() {
        return regenny()->health;
    }

public:
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, getHp, uint32_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, getHpMax, uint32_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, getHpRate100, uint32_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestAnim, void, uint32_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestAnimTable, void, uint32_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestAnimOp, void, uint64_t, uint64_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestMove, void, Vector4f&, int32_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestTurn, void, void*); // app::Puid const &
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestPath, void, int32_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestPathWait, void);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestPathBlockStop, void);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestRoute, void, void*, int32_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestRouteOp, void, void*, void*); // app::Puid const &,hap::VariantMap const &
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestTalk, void, int32_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestWait, void);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, requestAnimSpdRate, void, float);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, searchOn, bool, bool);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, pullout, bool);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, setLevel, bool, int32_t);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, talkDisable, int);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, setTalkEnable, void);
    OBJECT_SCRIPT_FUNCTION(BehaviorAppBase, setTalkDisable, void);
    
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