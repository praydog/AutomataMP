#pragma once

#include <optional>

#include <sdk/Math.hpp>
#include <sdk/regenny/CameraGame.hpp>

class CameraGame : public regenny::CameraGame {
public:
    static CameraGame* get();

public:
    std::optional<Vector2f> worldToScreen(const Vector2f& screenSize, const Vector3f& pos) const;
};