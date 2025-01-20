#include "mesh.h"

MeshUPtr Mesh::Create(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    uint32_t primitiveType) {
    auto mesh = MeshUPtr(new Mesh());
    mesh->Init(vertices, indices, primitiveType);
    return std::move(mesh);
}

void Mesh::Init(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    uint32_t primitiveType) {
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(Vertex), vertices.size());
    m_indexBuffer = Buffer::CreateWithData(
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32_t), indices.size());
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false,
        sizeof(Vertex), 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false,
        sizeof(Vertex), offsetof(Vertex, normal));
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false,
        sizeof(Vertex), offsetof(Vertex, texCoord));
}

void Mesh::Draw(const Program* program) const {
    m_vertexLayout->Bind();
    if(m_material) {
        m_material->SetToProgram(program);
    }
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(),
        GL_UNSIGNED_INT, 0);
}

MeshUPtr Mesh::CreateBox() {
    std::vector<Vertex> vertices = {
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },

        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },

        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
    };

    std::vector<uint32_t> indices = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    return Create(vertices, indices, GL_TRIANGLES);
}

MeshUPtr Mesh::CreatePlane() {
  std::vector<Vertex> vertices = {
    Vertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
    Vertex { glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
    Vertex { glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
    Vertex { glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
  };

  std::vector<uint32_t> indices = {
    0,  1,  2,  2,  3,  0,
  };

  return Create(vertices, indices, GL_TRIANGLES);
}


MeshUPtr Mesh::CreateArrow() {
    std::vector<Vertex> vertices = {
    // 위쪽 뿔 부분 0~35
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, 1.0f, 0.0f)},Vertex{glm::vec3(0.0f, 0.87f, 0.50f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, 0.87f, 0.50f)},Vertex{glm::vec3(0.0f, 0.50f, 0.87f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, 0.50f, 0.87f)},Vertex{glm::vec3(0.0f, 0.00f, 1.0f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, 0.00f, 1.0f)},Vertex{glm::vec3(0.0f, -0.50f, 0.87f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, -0.50f, 0.87f)},Vertex{glm::vec3(0.0f, -0.87f, 0.50f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, -0.87f, 0.50f)},Vertex{glm::vec3(0.0f, -1.0f, 0.0f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, -1.0f, 0.0f)},Vertex{glm::vec3(0.0f, -0.87f, -0.50f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, -0.87f, -0.50f)},Vertex{glm::vec3(0.0f, -0.50f, -0.87f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, -0.50f, -0.87f)},Vertex{glm::vec3(0.0f, 0.0f, -1.0f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, 0.0f, -1.0f)},Vertex{glm::vec3(0.0f, 0.5f, -0.87f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, 0.5f, -0.87f)},Vertex{glm::vec3(0.0f, 0.87f, -0.5f)},
    Vertex{glm::vec3(1.5f, 0.0f, 0.0f)},Vertex{glm::vec3(0.0f, 0.87f, -0.5f)},Vertex{glm::vec3(0.0f, 1.0f, 0.0f)},

    //기둥 위부분 36~48
    Vertex{glm::vec3(0.0f, 0.0f, 0.0f)},
    Vertex{glm::vec3(0.0f, 1.0f, 0.0f)},Vertex{glm::vec3(0.0f, 0.87f, 0.50f)},Vertex{glm::vec3(0.0f, 0.5f, 0.87f)},
    Vertex{glm::vec3(0.0f, 0.0f, 1.0f)},Vertex{glm::vec3(0.0f, -0.50f, 0.87f)},Vertex{glm::vec3(0.0f, -0.87f, 0.50f)},
    Vertex{glm::vec3(0.0f, -1.0f, 0.0f)},Vertex{glm::vec3(0.0f, -0.87f, -0.50f)},Vertex{glm::vec3(0.0f, -0.50f, -0.87f)},
    Vertex{glm::vec3(0.0f, 0.0f, -1.0f)}, Vertex{glm::vec3(0.0f, 0.50f, -0.87f)},Vertex{glm::vec3(0.0f, 0.87f, -0.50f)},

    //기둥 아랫 부분 49~61
    Vertex{glm::vec3(-3.0f, 0.0f, 0.0f)},
    Vertex{glm::vec3(-3.0f, 0.5f, 0.0f)},Vertex{glm::vec3(-3.0f, 0.43f, 0.25f)},Vertex{glm::vec3(-3.0f, 0.5f, 0.43f)},
    Vertex{glm::vec3(-3.0f, 0.0f, 0.5f)},Vertex{glm::vec3(-3.0f, -0.25f, 0.43f)},Vertex{glm::vec3(-3.0f, -0.43f, 0.25f)},
    Vertex{glm::vec3(-3.0f, -0.5f, 0.0f)},Vertex{glm::vec3(-3.0f, -0.43f, -0.25f)},Vertex{glm::vec3(-3.0f, -0.25f, -0.43f)},
    Vertex{glm::vec3(-3.0f, 0.0f, -0.5f)}, Vertex{glm::vec3(-3.0f, 0.25f, -0.43f)},Vertex{glm::vec3(-3.0f, 0.43f, -0.25f)},


    // 기둥 부분 EBO 사용 62~109
    Vertex{glm::vec3(0.0f, 0.50f, 0.0f)},Vertex{glm::vec3(-3.0f, 0.50f, 0.0f)},
    Vertex{glm::vec3(-3.0f, 0.43f, 0.25f)},Vertex{glm::vec3(0.0f, 0.43f, 0.25f)},

    Vertex{glm::vec3(0.0f, 0.43f, 0.25f)},Vertex{glm::vec3(-3.0f, 0.43f, 0.25f)},
    Vertex{glm::vec3(-3.0f, 0.5f, 0.43f)},Vertex{glm::vec3(0.0f, 0.5f, 0.43f)},

    Vertex{glm::vec3(0.0f, 0.5f, 0.43f)},Vertex{glm::vec3(-3.0f, 0.5f, 0.43f)},
    Vertex{glm::vec3(-3.0f, 0.0f, 0.50f)},Vertex{glm::vec3(0.0f, 0.0f, 0.50f)},

    Vertex{glm::vec3(0.0f, 0.0f, 0.50f)},Vertex{glm::vec3(-3.0f, 0.0f, 0.50f)},
    Vertex{glm::vec3(-3.0f, -0.25f, 0.43f)},Vertex{glm::vec3(0.0f, -0.25f, 0.43f)},

    Vertex{glm::vec3(0.0f, -0.25f, 0.43f)},Vertex{glm::vec3(-3.0f, -0.25f, 0.43f)},
    Vertex{glm::vec3(-3.0f, -0.43f, 0.25f)},Vertex{glm::vec3(0.0f, -0.43f, 0.25f)},

    Vertex{glm::vec3(0.0f, -0.43f, 0.25f)},Vertex{glm::vec3(-3.0f, -0.43f, 0.25f)},
    Vertex{glm::vec3(-3.0f, -0.50f, 0.0f)},Vertex{glm::vec3(0.0f, -0.50f, 0.0f)},

    Vertex{glm::vec3(0.0f, -0.50f, 0.0f)},Vertex{glm::vec3(-3.0f, -0.50f, 0.0f)},
    Vertex{glm::vec3(-3.0f, -0.43f, -0.25f)},Vertex{glm::vec3(0.0f, -0.43f, -0.25f)},

    Vertex{glm::vec3(0.0f, -0.43f, -0.25f)},Vertex{glm::vec3(-3.0f, -0.43f, -0.25f)},
    Vertex{glm::vec3(-3.0f, -0.25f, -0.43f)},Vertex{glm::vec3(0.0f, -0.25f, -0.43f)},

    Vertex{glm::vec3(0.0f, -0.25f, -0.43f)},Vertex{glm::vec3(-3.0f, -0.25f, -0.43f)},
    Vertex{glm::vec3(-3.0f, 0.0f, -0.50f)},Vertex{glm::vec3(0.0f, 0.0f, -0.50f)},

    Vertex{glm::vec3(0.0f, 0.0f, -0.50f)},Vertex{glm::vec3(-3.0f, 0.0f, -0.50f)},
    Vertex{glm::vec3(-3.0f, 0.25f, -0.43f)},Vertex{glm::vec3(0.0f, 0.25f, -0.43f)},

    Vertex{glm::vec3(0.0f, 0.25f, -0.43f)},Vertex{glm::vec3(-3.0f, 0.25f, -0.43f)},
    Vertex{glm::vec3(-3.0f, 0.43f, -0.25f)},Vertex{glm::vec3(0.0f, 0.43f, -0.25f)},

    Vertex{glm::vec3(0.0f, 0.43f, -0.25f)},Vertex{glm::vec3(-3.0f, 0.43f, -0.25f)},
    Vertex{glm::vec3(-3.0f, 0.50f, 0.0f)},Vertex{glm::vec3(0.0f, 0.50f, 0.0f)},
    };

    std::vector<uint32_t> indices = {
      0,1,2,3,4,5,6,7,8,9,10,11,
      12,13,14,15,16,17,18,19,20,21,22,23,
      24,25,26,27,28,29,30,31,32,33,34,35,

      36,37,38, 36,38,39, 36,39,40,
      36,40,41, 36,41,42, 36,42,43, 
      36,43,44, 36,44,45, 36,45,46, 
      36,46,47, 36,47,48, 36,48,37, 

      49,50,51, 49,51,52, 49,52,53,
      49,53,54, 49,54,55, 49,55,56,
      49,56,57, 49,57,58, 49,58,59,
      49,59,60, 49,60,61, 49,61,50,

      62,64,63, 62,65,64,
      66,68,67, 66,69,68,
      70,72,71, 70,73,62,
      74,76,75, 74,77,66,
      78,80,79, 78,81,80,
      82,84,83, 82,85,84,
      86,88,87, 86,89,88,
      90,92,91, 90,93,92,
      94,96,95, 94,97,96,
      98,100,99, 98,101,100,
      102,104,103, 102,105,104,
      106,108,107, 102,109,108,
    };

    return Create(vertices, indices, GL_TRIANGLES);
}


void Material::SetToProgram(const Program* program) const {
  int textureCount = 0;
  if (diffuse) {
    glActiveTexture(GL_TEXTURE0 + textureCount);
    program->SetUniform("material.diffuse", textureCount);
    diffuse->Bind();
    textureCount++;
  }

  if (specular) {
    glActiveTexture(GL_TEXTURE0 + textureCount);
    program->SetUniform("material.specular", textureCount);
    specular->Bind();
    textureCount++;
  }

  glActiveTexture(GL_TEXTURE0);
  program->SetUniform("material.shininess", shininess);
}