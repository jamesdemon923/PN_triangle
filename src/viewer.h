#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "config.h"
#include "Log.h"

class MeshBin;
class Camera;

class Viewer;
//void drawUI(RenderSetting &setting, DisplayOption & displayOption);
void drawUI(Viewer &viewer);

//<
//< It serve as a container for modules such as
//< 1. window management
//< 2. keyboard & mouse event handling 
//< 3. shader program management
//< 4. lighting & camera arrangement
//< 5. rendering & updating
//< 6. interact with user
//<
class Viewer
{
private:

    int m_window_width = 1280;
    int m_window_height = 720;
    GLFWwindow *m_window{nullptr};

    std::string m_window_title{ "PN Triangle" };
    bool m_capture_colorbuffer{false};
    bool m_save_image_sequence{false};
    bool m_enable_tess_anim{false};
    int  m_sequence_count{ 0 };
    int  m_animation_mode{ 0 };
    int  m_frame_num{ 0 };

    RenderSetting &m_setting; // = GetRenderSetting();
    DisplayOption &m_option;  // = GetDisplayOption();

    friend void drawUI(Viewer &viewer);
    friend void drawOverlay(const Viewer &viewer);

private:

    GLuint programID;
    GLuint silhouProgramID;
    GLuint tessProgramID;

    // standard
    GLuint lightID;
    GLuint matrixID;
    GLuint modelMatrixID;
    GLuint viewMatrixID;
    GLuint projectionMatrixID;
    GLuint mesh_color_ID;

    // silhouette
    GLuint silhLightID;
    GLuint silhMatrixID;
    GLuint silhModelMatrixID;
    GLuint silhViewMatrixID;
    GLuint silhProjectionMatrixID;
    GLuint silh_mesh_color_ID;

    // tess
    GLuint tessLightID;
    GLuint tess_mesh_color_ID;
    GLuint tessMatrixID;
    GLuint tessModelMatrixID;
    GLuint tessViewMatrixID;
    GLuint tessProjectionMatrixID;
    GLfloat tessellationLevelID;
    //GLfloat tessellationLevelInnerID;
    //GLfloat tessellationLevelOuterID;

public:
    Viewer() = delete;
    ~Viewer()
    {
        glDeleteProgram(programID);
        glDeleteProgram(tessProgramID);
        glfwTerminate();
    }

    Viewer(std::string title, int width, int height)
        : m_window_title(title)
        , m_window_width(width)
        , m_window_height(height)
        , m_setting(GetRenderSetting())
        , m_option(GetDisplayOption())
    {
        initWindow();
        initOpenGLShaders();
    }

    void SetModel(std::string modelPath)
    {
        m_objPath = modelPath;
    }

    void Run();

    void SaveScreen(const std::string filename = "pic.tga");
    void SaveImageSequence(const std::string dir = "imageseq");

private:
    std::string m_objPath;
    int initWindow(void);
    void initOpenGLShaders(void);
    void render(MeshBin & m_meshBin, Camera &m_camera);
    void animateCamera(Camera &m_camera);
    void animateTessellation();
};
