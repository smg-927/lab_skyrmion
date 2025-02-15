#include "context.h"
#include "image.h"

#include <imgui.h>

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init() {

    m_box = Mesh::CreateBox();
    m_plane = Mesh::CreatePlane();
    m_arrow = Mesh::CreateArrow();

    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");

    m_textureProgram = Program::Create("./shader/texture.vs", "./shader/texture.fs");

    m_postProgram = Program::Create("./shader/texture.vs", "./shader/gamma.fs");

    m_skyboxProgram = Program::Create("./shader/skybox.vs", "./shader/skybox.fs");

    m_envMapProgram = Program::Create("./shader/env_map.vs", "./shader/env_map.fs");
    
    glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
   
    TexturePtr darkGrayTexture = Texture::CreateFromImage(
    Image::CreateSingleColorImage(4, 4,
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());

    TexturePtr grayTexture = Texture::CreateFromImage(
    Image::CreateSingleColorImage(4, 4,
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());
            
    m_planeMaterial = Material::Create();
    m_planeMaterial->diffuse = Texture::CreateFromImage(
        Image::Load("./image/marble.jpg").get());
    m_planeMaterial->specular = grayTexture;
    m_planeMaterial->shininess = 128.0f;

    m_box1Material = Material::Create();
    m_box1Material->diffuse = Texture::CreateFromImage(
        Image::Load("./image/container.jpg").get());
    m_box1Material->specular = darkGrayTexture;
    m_box1Material->shininess = 16.0f;

    m_arrowMaterial = Material::Create();
    m_arrowMaterial->diffuse = grayTexture;
    m_arrowMaterial->specular = darkGrayTexture;
    m_arrowMaterial->shininess = 128.0f;

    m_windowTexture = Texture::CreateFromImage(
        Image::Load("./image/blending_transparent_window.png").get());

    auto cubeRight = Image::Load("./image/skybox/right.jpg", false);
    auto cubeLeft = Image::Load("./image/skybox/left.jpg", false);
    auto cubeTop = Image::Load("./image/skybox/top.jpg", false);
    auto cubeBottom = Image::Load("./image/skybox/bottom.jpg", false);
    auto cubeFront = Image::Load("./image/skybox/front.jpg", false);
    auto cubeBack = Image::Load("./image/skybox/back.jpg", false);
    m_cubeTexture = CubeTexture::CreateFromImages({
        cubeRight.get(),
        cubeLeft.get(),
        cubeTop.get(),
        cubeBottom.get(),
        cubeFront.get(),
        cubeBack.get(),
    });

    m_data = DataLoader::Create();
    m_data->ReadFile("Data/T_200_Index_168/T_200_Index_168_X.txt", rawdatalength, 0);
    m_data->ReadFile("Data/T_200_Index_168/T_200_Index_168_Y.txt", rawdatalength, 1);
    m_data->ReadFile("Data/T_200_Index_168/T_200_Index_168_Z.txt", rawdatalength, 2);

    m_data->ReduceAndAverage();

    data = m_data->GetData();
    
    m_cameraPos = glm::vec3(startX, startY, startZ);

    endX = data.size()-1;
    endY = data[0].size() -1;
    startZ = data[0][0].size()/2;
    endZ = (data[0][0].size()/2) + 1;

    return true;
}

void Context::Render() {
    
    if (ImGui::Begin("my first ImGui window")) {

        // 버튼 크기를 두 배로 키우기 위해 스타일 변경
        ImGuiStyle& style = ImGui::GetStyle();
        style.FramePadding = ImVec2(10, 10);  // 버튼 내부 패딩 증가
        style.ItemSpacing = ImVec2(15, 10);   // 요소 간 간격 증가

        // 전체 글꼴 크기 두 배로 설정
        ImGui::SetWindowFontScale(2.0f);

        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) {
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        }

        ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
        ImGui::Separator();

        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat3("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat3("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);

        ImGui::Separator();

        ImGui::Text("Range Control for x:");
        ImGui::DragInt("Start X", &startX, 0.3, 0, data.size());
        ImGui::DragInt("End X", &endX, 0.3, 0, data.size());

        ImGui::Text("Range Control for y:");
        ImGui::DragInt("Start Y", &startY, 0.3, 0,data[0].size());
        ImGui::DragInt("End Y", &endY, 0.3, 0, data[0].size());

        ImGui::Text("Range Control for z:");
        ImGui::DragInt("Start Z", &startZ, 0.3, 0, data[0][0].size());
        ImGui::DragInt("End Z", &endZ, 0.3, 0, data[0][0].size());

        ImGui::Text("Arrow Scale:");
        ImGui::DragFloat("Arrow Scale", &arrowscale, 0.01f, 0.1f, 2.0f);

        ImGui::Separator();

        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.01f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::Checkbox("flash light", &m_flashLightMode);
        }

        ImGui::Checkbox("animation", &m_animation);
        ImGui::Checkbox("Colormode: check = XY, Uncheck = Z", &m_colormode);

    }
    ImGui::End();


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cameraFront =
    glm::rotate(glm::mat4(1.0f),glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

	auto projection = glm::perspective(glm::radians(45.0f),
        (float)m_width / (float)m_height, 0.01f, 1000.0f);

    auto view = glm::lookAt(m_cameraPos,m_cameraPos + m_cameraFront, m_cameraUp);

    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;

    if(m_flashLightMode)
    {
        lightPos = m_cameraPos;
        lightDir = m_cameraFront;
    }
    else
    {
        auto lightModelTransform =
            glm::translate(glm::mat4(1.0),  m_light.position) *
            glm::scale(glm::mat4(1.0), glm::vec3(0.1f));

        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
        m_box -> Draw(m_simpleProgram.get());
    }
    /*
    auto skyboxModelTransform =
    glm::translate(glm::mat4(1.0), m_cameraPos) *
    glm::scale(glm::mat4(1.0), glm::vec3(50.0f));
    m_skyboxProgram->Use();
    m_cubeTexture->Bind();
    m_skyboxProgram->SetUniform("skybox", 0);
    m_skyboxProgram->SetUniform("transform", projection * view * skyboxModelTransform);
    m_box->Draw(m_skyboxProgram.get());
    */


    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", lightPos);
    m_program->SetUniform("light.direction", lightDir);
    m_program->SetUniform("light.cutoff", glm::vec2(
        cosf(glm::radians(m_light.cutoff[0])),
        cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);

    auto modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_planeMaterial->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    m_simpleProgram->Use();

    if(m_animation)
    {
        startZ = (int)(glfwGetTime() * 9) % (data[0][0].size()-2);
        endZ = startZ + 2;
    }

    for (int i = startX; i < endX; i++) {
        for (int j = startY; j < endY; j++) {
            for (int k = startZ; k < endZ; k++) {
                // Position transformation
                glm::mat4 movemat = glm::translate(glm::mat4(1.0f), glm::vec3(k, j, i));

                // Rotation transformation using the normalized vector
                const auto& vec = data[i][j][k];
                glm::mat4 rotmat = normalizeandrot(vec[0], vec[1], vec[2]);
                // Color calculation
                glm::vec4 arrowcolor;
                if(m_colormode)
                {
                    arrowcolor = vectorColorXY(vec[0], vec[1], vec[2]);
                }
                else
                {
                    arrowcolor = vectorColorZ(vec[0], vec[1], vec[2]);
                }
                glm::mat4 arrowscalemat = glm::scale(glm::mat4(1.0f), glm::vec3(arrowscale*3, arrowscale, arrowscale));
                // Combine transformations and apply to shader
                glm::mat4 modelMatrix = projection * view * movemat * rotmat * arrowscalemat;
                m_simpleProgram->SetUniform("transform", modelMatrix);
                m_simpleProgram->SetUniform("color", arrowcolor);
                // 색이 있을 때만 출력하도록 변경 
                // if(arrowcolor[0] + arrowcolor[1] + arrowcolor[2] >0.6f)
                // {
                //     m_arrow->Draw(m_simpleProgram.get());
                // }
                m_arrow->Draw(m_simpleProgram.get());
                
            }
        }
    }
}

glm::mat4 Context::normalizeandrot(float z, float y, float x)
{
	float theta = sqrt(x*x+z*z)/ sqrt(x * x + y * y + z * z);
	float phi = x / sqrt(x * x + z * z);

	phi = (acos(phi)*180/3.14);

	if (y >= 0)
		theta = acos(theta) * 180 / 3.14;
	else
		theta = -acos(theta) * 180 / 3.14;

	if (z < 0)
		phi = -phi;

	glm::mat4 rotphi = glm::rotate(glm::mat4(1.0f), glm::radians(phi), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 rottheta = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0.f, 1.f, 0.f));

	return rotphi * rottheta;
}

glm::vec4 Context::vectorColorZ(float x, float y, float z)
{
    float magnitude = sqrt(x * x + y * y + z * z);

	 float nz = z / magnitude;

	return glm::vec4(-nz,0,nz*0.5,1);
};

glm::vec4 Context::vectorColorXY(float x, float y, float z)
{
    // 벡터 크기 계산
    float magnitude = sqrt(x * x + y * y + z * z);

    // 각 성분을 정규화
    float nx = x / magnitude;
    float ny = y / magnitude;
    float nz = z / magnitude;

    // 색상 계산
    float red = std::max(0.0f, nx);     // +x 방향: 빨강
    float cyan = std::max(0.0f, -nx);  // -x 방향: 시안 (빨강의 보색)
    float green = std::max(0.0f, ny);  // +y 방향: 초록
    float magenta = std::max(0.0f, -ny); // -y 방향: 마젠타 (초록의 보색)

    // z 방향 성분이 클수록 검정색에 가까워짐
    float zFactor = nz; // nz는 [0, 1] 범위에서 검정색 효과를 줄 변수
    float intensity = 1.0f - fabs(zFactor); // z 성분에 따른 강도 감소

    // 최종 색상 혼합
    float finalRed = (red + magenta) * intensity;
    float finalGreen = (green + cyan) * intensity;
    float finalBlue = (cyan + magenta) * intensity;

    // 알파 값은 항상 1로 설정
    return glm::vec4(finalRed, finalGreen, finalBlue, 1.0f);
};


void Context::ProcessInput(GLFWwindow* window) {

    const float cameraSpeed = 1.2f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;    

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;

    // bool isKeyPressed = false;

    // if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_KEY_DOWN) {
    //     if (!isKeyPressed) {
    //         m_animation = !m_animation; // 애니메이션 상태 토글
    //         isKeyPressed = true;        // 키가 눌렸음을 표시
    //     }
    // } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
    //     isKeyPressed = false; // 키가 눌리지 않았음을 표시
    // }
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);

}

void Context::MouseMove(double x, double y) {

    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.2f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
      m_prevMousePos = glm::vec2((float)x, (float)y);
      m_cameraControl = true;
    }
    else if (action == GLFW_RELEASE) {
      m_cameraControl = false;
    }
  }
}