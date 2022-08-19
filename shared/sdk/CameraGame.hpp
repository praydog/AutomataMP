#pragma once

#include <optional>

#include <sdk/Math.hpp>
#include <sdk/regenny/CameraGame.hpp>

namespace sdk {
// cCameraGame
class CameraGame : public regenny::CameraGame {
public:
    static CameraGame* get();

public:
    std::optional<Vector2f> world_to_screen(const Vector2f& screen_size, const Vector3f& pos) const;
};
static_assert(sizeof(CameraGame) == 0x8C20, "Size of cCameraGame is not correct.");
}