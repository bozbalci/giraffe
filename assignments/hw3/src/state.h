#ifndef HW3_STATE_H
#define HW3_STATE_H

struct UIState {
    static constexpr auto HEIGHT_FACTOR_INITIAL = 10.0f;
    static constexpr auto HEIGHT_FACTOR_UNIT = 0.5f;
    static constexpr auto TEXTURE_HORIZONTAL_SHIFT_INITIAL = 0.0f;
    static constexpr auto TEXTURE_HORIZONTAL_SHIFT_UNIT = 1.0f;
    static constexpr auto PITCH_INITIAL = 0.0f;
    static constexpr auto PITCH_UNIT = 0.5f;
    static constexpr auto PITCH_MAX = 89.9f;
    static constexpr auto PITCH_MIN = -PITCH_MAX;
    static constexpr auto YAW_INITIAL = 90.0f;
    static constexpr auto YAW_UNIT = 0.5f;
    static constexpr auto YAW_MAX = 360.0f;
    static constexpr auto YAW_MIN = 0.0f;
    static constexpr auto SPEED_INITIAL = 0.0f;
    static constexpr auto SPEED_UNIT = 0.25f;
    static constexpr auto LIGHT_UNIT = 5.0f;

    // OPENGL-RELATED DATA
    int TextureWidth;
    int TextureHeight;
    GLint CameraPositionLocation;
    GLint LightPositionLocation;
    GLint MVPMatrixLocation;
    GLint HeightFactorLocation;

    /// UI PARAMETERS
    double HeightFactor = HEIGHT_FACTOR_INITIAL;
    double TextureHorizontalShift = TEXTURE_HORIZONTAL_SHIFT_INITIAL;
    float Pitch = PITCH_INITIAL;
    float Yaw = YAW_INITIAL;
    float Speed = SPEED_INITIAL;

    /// UNIFORMS
    struct CameraType {
        glm::vec3 Position;
        glm::vec3 Gaze{0.0f, 0.0f, 0.1f};
        glm::vec3 Up{0.0f, 1.0f, 0.0f};
    } Camera;

    CameraType CAMERA_INITIAL;

    struct {
        glm::mat4 Model{1.0f};
        glm::mat4 View;
        glm::mat4 Projection = glm::perspective(
            glm::radians(45.0f),
            1.0f,
            0.1f,
            1000.0f
        );
        glm::mat4 Composite;
    } Matrices;

    glm::vec3 LightPosition;

    // METHODS
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
        LightPosition.z += LIGHT_UNIT;
    }

    void LightSourceDown() {
        LightPosition.z -= LIGHT_UNIT;
    }

    void LightSourceLeft() {
        LightPosition.x += LIGHT_UNIT;
    }

    void LightSourceRight() {
        LightPosition.x -= LIGHT_UNIT;
    }

    void LightSourceIncrementHeight() {
        LightPosition.y += LIGHT_UNIT;
    }

    void LightSourceDecrementHeight() {
        LightPosition.y -= LIGHT_UNIT;
    }

    void IncrementPitch() {
        Pitch += PITCH_UNIT;
        if (Pitch > PITCH_MAX) Pitch = PITCH_MAX;
    }

    void DecrementPitch() {
        Pitch -= PITCH_UNIT;
        if (Pitch < PITCH_MIN) Pitch = PITCH_MIN;
    }

    void IncrementYaw() {
        Yaw += YAW_UNIT;
        if (Yaw > YAW_MAX) Yaw -= YAW_MAX;
    }

    void DecrementYaw() {
        Yaw -= YAW_UNIT;
        if (Yaw < YAW_MIN) Yaw += YAW_MAX;
    }

    void IncrementSpeed() {
        Speed += SPEED_UNIT;
    }

    void DecrementSpeed() {
        Speed -= SPEED_UNIT;
    }

    void ResetSpeed() {
        Speed = SPEED_INITIAL;
    }

    void ResetPositionAndCamera() {
        ResetSpeed();
        Pitch = PITCH_INITIAL;
        Yaw = YAW_INITIAL;
        Camera = CAMERA_INITIAL;
    }

    void Initialize(const int& TextureWidth_, const int& TextureHeight_) {
        // Initialize OpenGL variables
        TextureWidth = TextureWidth_;
        TextureHeight = TextureHeight_;
        CameraPositionLocation = glGetUniformLocation(ProgramShaderId, "CameraPosition");
        LightPositionLocation = glGetUniformLocation(ProgramShaderId, "LightPosition");
        MVPMatrixLocation = glGetUniformLocation(ProgramShaderId, "MVPMatrix");
        HeightFactorLocation = glGetUniformLocation(ProgramShaderId, "HeightFactor");

        // Initialize world data
        Camera.Position = {
            (float) TextureWidth / 2.0f,
            (float) TextureWidth / 10.0f,
            (float) -TextureWidth / 4.0f
        };
        LightPosition = {
            (float) TextureWidth / 2.0f,
            100.0f,
            (float) TextureHeight / 2.0f
        };

        // Save this incarnation of the camera so that it can be restored later
        CAMERA_INITIAL = Camera;
    }

    void UpdateCamera() {
        auto YawInRadians = glm::radians(Yaw);
        auto PitchInRadians = glm::radians(Pitch);

        Camera.Gaze = glm::normalize(glm::vec3(
            std::cos(YawInRadians) * std::cos(PitchInRadians),
            std::sin(PitchInRadians),
            std::sin(YawInRadians) * std::cos(PitchInRadians)
        ));

        Camera.Position += Speed * Camera.Gaze;

        glUniform3fv(CameraPositionLocation, 1, glm::value_ptr(Camera.Position));
    }

    void UpdateMVPMatrix() {
        Matrices.View = glm::lookAt(
            Camera.Position,
            Camera.Position + Camera.Gaze,
            Camera.Up
        );

        Matrices.Composite = Matrices.Projection * Matrices.View * Matrices.Model;

        glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(Matrices.Composite));
    }

    void UpdateHeightFactor() {
        glUniform1f(HeightFactorLocation, HeightFactor);
    }

    void UpdateLightPosition() {
        glUniform3fv(LightPositionLocation, 1, glm::value_ptr(LightPosition));
    }

    void Update() {
        UpdateCamera();
        UpdateMVPMatrix();
        UpdateHeightFactor();
        UpdateLightPosition();
        // TODO Update texture horizontal offset (Q/E)
    }

    void Print() {
        system("clear");
        std::cout << "HeightFactor = " << HeightFactor << '\n';
        std::cout << "TextureHorizontalShift = " << TextureHorizontalShift << '\n';
        std::cout << "CameraPosition = {"
                  << Camera.Position.x << ", "
                  << Camera.Position.y << ", "
                  << Camera.Position.z << "}" << '\n';
        std::cout << "LightPosition = {"
                  << LightPosition.x << ", "
                  << LightPosition.y << ", "
                  << LightPosition.z << "}" << '\n';
        std::cout << "Pitch = " << Pitch << '\n';
        std::cout << "Yaw = " << Yaw << '\n';
        std::cout << "Speed = " << Speed << '\n';
    }

} TheState;

#endif //HW3_STATE_H
