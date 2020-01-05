#ifndef HW3_STATE_H
#define HW3_STATE_H

struct UIState {
    static constexpr auto HEIGHT_FACTOR_UNIT = 0.5f;
    static constexpr auto TEXTURE_HORIZONTAL_SHIFT_UNIT = 1.0f;
    static constexpr auto PITCH_UNIT = 0.5f;
    static constexpr auto YAW_UNIT = 0.5f;
    static constexpr auto SPEED_UNIT = 0.25f;

    double HeightFactor = 10.0f;
    double TextureHorizontalShift = 0.0f;
    glm::vec3 LightSourceCoordinate = {1.0f, 1.0f, 1.0f};
    float Pitch = 0.0f;
    float Yaw = 90.0f;
    float Speed = 0.0f;

    void IncrementHeightFactor() {
        HeightFactor += HEIGHT_FACTOR_UNIT;
    }

    void DecrementHeightFactor() {
        HeightFactor -= HEIGHT_FACTOR_UNIT;
    }

    void ShiftTextureLeft() {
        TextureHorizontalShift -= TEXTURE_HORIZONTAL_SHIFT_UNIT;
    }

    void ShiftTextureRight() {
        TextureHorizontalShift += TEXTURE_HORIZONTAL_SHIFT_UNIT;
    }

    void LightSourceUp() {
        LightSourceCoordinate.x += 5.0f;
    }

    void LightSourceDown() {
        LightSourceCoordinate.x -= 5.0f;
    }

    void LightSourceLeft() {
        LightSourceCoordinate.z -= 5.0f;
    }

    void LightSourceRight() {
        LightSourceCoordinate.z += 5.0f;
    }

    void LightSourceIncrementHeight() {
        LightSourceCoordinate.y += 5.0f;
    }

    void LightSourceDecrementHeight() {
        LightSourceCoordinate.y -= 5.0f;
    }

    void IncrementPitch() {
        Pitch += PITCH_UNIT;
        if (Pitch > 89.9f) Pitch = 89.9f;
    }

    void DecrementPitch() {
        Pitch -= PITCH_UNIT;
        if (Pitch < -89.9f) Pitch = -89.9f;
    }

    void IncrementYaw() {
        Yaw += YAW_UNIT;
        if (Yaw > 360.0f) Yaw -= 360.0f;
    }

    void DecrementYaw() {
        Yaw -= YAW_UNIT;
        if (Yaw < 0.0f) Yaw += 360.0f;
    }

    void IncrementSpeed() {
        Speed += SPEED_UNIT;
    }

    void DecrementSpeed() {
        Speed -= SPEED_UNIT;
    }

    void ResetSpeed() {
        Speed = 0.0f;
    }

    void ResetPosition() {
        ResetSpeed();
        Pitch = 0.0f;
        Yaw = 90.0f;
    }

    void Print() {
        system("clear");
        std::cout << "HeightFactor = " << HeightFactor << '\n';
        std::cout << "TextureHorizontalShift = " << TextureHorizontalShift << '\n';
        std::cout << "LightSourceCoordinate = {"
                  << LightSourceCoordinate.x << ", "
                  << LightSourceCoordinate.y << ", "
                  << LightSourceCoordinate.z << "}" << '\n';
        std::cout << "Pitch = " << Pitch << '\n';
        std::cout << "Yaw = " << Yaw << '\n';
        std::cout << "Speed = " << Speed << '\n';
    }
} TheState;

#endif //HW3_STATE_H
