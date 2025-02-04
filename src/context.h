#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "framebuffer.h"
#include "dataloader.h"

CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();
    void Render();    
    glm::mat4 normalizeandrot(float z, float y, float x);
    glm::vec4 vectorColorZ(float x, float y, float z);
    glm::vec4 vectorColorXY(float x, float y, float z);
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);
private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_simpleProgram;
	ProgramUPtr m_textureProgram;
    ProgramUPtr m_postProgram;
    float m_gamma {1.0f};
    
    MeshUPtr m_box;
    MeshUPtr m_plane;
    MeshUPtr m_arrow;

    DataLoaderUPtr m_data;
    // animation
    bool m_animation { false };
    bool m_colormode { false };

    // clear color
    glm::vec4 m_clearColor { glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) };

    // light parameter
    struct Light {
	    glm::vec3 position { glm::vec3(1.0f, 4.0f, 4.0f) };
        glm::vec3 direction { glm::vec3(-1.0f, -1.0f, -1.0f) };
        glm::vec2 cutoff { glm::vec2(120.0f, 5.0f) };
        float distance { 128.0f };
        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.8f, 0.8f, 0.8f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };

    Light m_light;
    bool m_flashLightMode {false};

    MaterialPtr m_planeMaterial;
    MaterialPtr m_box1Material;
    MaterialPtr m_arrowMaterial;
    TexturePtr m_windowTexture;

    bool m_cameraControl { false };
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { -20.0f };
    float m_cameraYaw { 0.0f };
	glm::vec3 m_cameraPos { glm::vec3(-24.0f, 100.0f, 89.0f) };
    glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };

    // framebuffer
    FramebufferUPtr m_framebuffer;

    // cubemap
    CubeTextureUPtr m_cubeTexture;
    ProgramUPtr m_skyboxProgram;
    ProgramUPtr m_envMapProgram;
    
    int m_width {WINDOW_WIDTH};
    int m_height {WINDOW_HEIGHT};

    std::vector<std::vector<std::vector<std::vector<float>>>> data;

    glm::vec3 rawdatalength {112, 112, 52};

    int startX = 0, endX = 1; // i 범위
    int startY = 0, endY = 1; // j 범위
    int startZ = 0, endZ = 1; // k 범위
    float arrowscale = 0.3;
};

#endif // __CONTEXT_H__