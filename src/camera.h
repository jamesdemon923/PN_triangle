//=================================================================================//
// Copyright (c) 2023 Haolan Xu
//=================================================================================//

#pragma once

#include <glm/glm.hpp>

#define M_PI 3.14159265359f  // pi

enum class OrbitAxis
{
  POS_X,
  POS_Y,
  POS_Z,
  NEG_X,
  NEG_Y,
  NEG_Z
};

class Camera 
{
public:

    Camera() = default;
    Camera(glm::vec3 at, float dist, glm::vec2 azel = glm::vec2(90.f, 0.f));

    void startNewRotation();

    void rotate(glm::vec2 delta);
    void zoom(float delta);
    void pan(glm::vec2 delta);
    void reset();
    
    void setAxis(OrbitAxis axis);

    glm::vec2 azel() const;

    glm::vec3 eye() const;
    glm::vec3 dir() const;
    glm::vec3 up() const;

protected:

    void update();

    OrbitAxis m_axis{OrbitAxis::NEG_Y};
  
    glm::vec2 m_azel{0.f}; //< NOTE: unit is in degrees

    float m_distance{1.f};
    float m_worldSize{1.f};
    float m_speed{0.25f};

    bool m_invertRotation{true};

    glm::vec3 m_eye;
    glm::vec3 m_at;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_forward; //< newly added

    //< use RH coordinate
    glm::mat4 m_viewMatrix{1.f};
    glm::mat4 m_projMatrix{0.f};

    float m_fovy{45.f};
    float m_aspect{1280.f / 720.f};
    float m_zNear{0.001f};
    float m_zFar{100.f};

    // Initial settings
    glm::vec3 m_initial_at;
    float m_initial_distance;
    glm::vec2 m_initial_azel;

public:

    glm::mat4 viewMatrix() const;
    glm::mat4 projMatrix() const;

};

