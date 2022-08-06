#include <spdlog/spdlog.h>
#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include <sdk/regenny/CameraData.hpp>

#include "CameraGame.hpp"

/*
7FF710B987A0 + 0x53 ba0011_coaster_brake_start
7FF710F38177 + 0x50 LANDING
7FF710E7A420 - 0x4F state     : %s
7FF710C64BD4 - 0x4B 2065
7FF710B72806 - 0x4B Enable : %s
7FF710C1E2E9 + 0x49 ba0007_cannon_ui_loop
7FF710C64BCB - 0x42 2067
7FF710D60CBB - 0x3E lv%d
7FF710C1E2F5 + 0x3D ba0007_cannon_reload_A
7FF710BED96F + 0x3C wp3000_valcan_shot_lv2
7FF710B72781 + 0x3A TARGET
7FF710B72891 + 0x39 CurrentNode
7FF710C64BC2 - 0x39 2066
7FF710E6E570 + 0x33 COLLIDE!
7FF710B72852 + 0x2A Step : %d / %d
7FF710B727E2 - 0x27 FALSE
7FF710E6E5B8 - 0x15 DISTANCE:%f
7FF710B728DF - 0x15 GoalNode
7FF710B727D0 - 0x15 TRUE
7FF710B72781 - 0x15 TARGET
7FF710B72891 - 0x15 CurrentNode
7FF710E6E570 - 0x14 COLLIDE!
*/
CameraGame* CameraGame::get() {
    static auto instance = []() -> CameraGame* {
        spdlog::info("[CameraGame] Finding CameraGame...");

        CameraGame* result{nullptr};
        const auto game = utility::get_executable();

        // There is a cCameraGame::`vftable' that can be referenced
        // if this ever changes.
        const auto str = utility::scan_string(game, "Step : %d / %d");

        if (!str) {
            spdlog::error("[CameraGame] Failed to find CameraGame.");
            return nullptr;
        }

        const auto ref = utility::scan_reference(game, *str);

        if (!ref) {
            spdlog::error("[CameraGame] Failed to find CameraGame.");
            return nullptr;
        }

        const auto ref2 = utility::scan_disasm(*ref + 4, 100, "48 8D 0D");

        if (!ref2) {
            spdlog::error("[CameraGame] Failed to find CameraGame.");
            return nullptr;
        }

        result = (CameraGame*)utility::calculate_absolute(*ref2 + 3);

        spdlog::info("[CameraGame] CameraGame: {:x}", (uintptr_t)result);

        return result;
    }();

    return instance;
}

std::optional<Vector2f> CameraGame::worldToScreen(const Vector2f& screenSize, const Vector3f& pos) const {
    if (!this->data) {
        return std::nullopt;
    }

    const auto& transform = *(Matrix4x4f*)&this->data->view;
    const auto& right = *(Vector3f*)&transform[0];
    const auto& up = *(Vector3f*)&transform[1];
    const auto& forward = *(Vector3f*)&transform[2];
    const auto& origin = *(Vector3f*)&transform[3];
    const auto delta = pos - origin;
    const auto z = glm::dot(delta, forward * -1.0f);

    if (z <= 0.0f) {
        return std::nullopt;
    }

    // get the width height here
    const auto width = screenSize.x;
    const auto height = screenSize.y;

    const auto aspect_ratio = width / height;
    //const auto fov = 2 * glm::atan(glm::tan(this->fov / 2.0f) * (height / width));
    const auto y_scale = 1.0f / glm::tan(this->fov / 2.0f);
    const auto x_scale = y_scale / aspect_ratio;
    const auto x = glm::dot(delta, right) * (1.0f / z) * x_scale;
    const auto y = glm::dot(delta, up) * (1.0f / z) * y_scale;

    return Vector2f{ ((1.0f + x) * width * 0.5f), ((1.0f - y) * height * 0.5f) };
}
