#include "Game.h"

Game::Game()
    : GLFWApplication("Exam Application Autumn 2025", "1.0", 768, 1024), m_window(nullptr),
    m_renderer(nullptr), m_combinedSolidBlock(nullptr), m_deltaTime(0.0f), m_lastFrameTime(0.0f),
    m_solidBlockIndex(0)
{}

Game::~Game()
{
    shutdown();
}

unsigned Game::init()
{
    if (!getIsOpenGLInitialized())
        return 1;

    RenderCommands::DisbaleBackFaceCulling();

    m_window = getWindow();

    glfwSetKeyCallback(m_window, keyCallback);

    m_renderer = std::make_shared<Renderer>();

    bool textureLoadSuccess = TextureManager::GetInstance()->LoadTexture2DRGBA("Wall", std::string(TEXTURES_DIR) + std::string("wall_texture.png"), 0);

    if (!textureLoadSuccess)
    {
        std::cerr << "Failed to load texture!" << std::endl;
        return 1;
    }

    bool cubemapLoadSuccess = TextureManager::GetInstance()->LoadCubeMapRGBA("Block", std::string(TEXTURES_DIR) + std::string("cube_texture.jpg"), 1);

    if (!cubemapLoadSuccess)
    {
        std::cerr << "Failed to load cubemap texture!" << std::endl;
        return 1;
    }

    createTunnel();
    createCamera();
    createNewComplexBlock();
    createSolidBlock();

    return 0;
}

unsigned Game::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        float time = glfwGetTime();
        m_deltaTime = time - m_lastFrameTime;
        m_lastFrameTime = time;
        dayNightCycle();

        if (m_forceMovePossible && time > 0 && (int)time % 2 == 0 )
        {
            glm::vec3 moveDir = glm::vec3{ 0.0f, 0.0f, -1.0f };
            if (!checkCollision(moveDir))
            {
                m_activeComplexBlock->move(moveDir);
            }
            else
            {
                makeBlockSolid();
            }
            m_forceMovePossible = false;
        }
        else if(!m_forceMovePossible && (int)time % 2 != 0)
        {
            m_forceMovePossible = true;
        }

        m_renderer->draw(m_camera.GetViewProjectionMatrix(), m_camera.GetPosition(), m_positionLight, m_lightMix, m_tunnelData, m_TUNNNEL_TILE_NUMBER, m_activeComplexBlock->VAO, m_activeComplexBlock->getNumBlocks(), m_combinedSolidBlock->VAO, 10+m_solidBlockIndex, m_applyTextures);
        // Swap buffers and poll events
        glfwSwapBuffers(m_window);
        glfwPollEvents();

    }
    return 0;
}

void Game::shutdown()
{
    if (m_renderer)
    {
        m_renderer->shutdown();
    }
}

// Data for the whole tunnel
void Game::createTunnel()
{
    // Depth * Width * Height * data for each vertex
    std::array<float, 11 * 4> vertices;
    // Depth * Width * 4 outer wall + Width * Height
    std::array<glm::mat4, (10 * 5 * 4) + (5 * 5 )> models;

    const glm::vec3 tileColor = glm::vec3{ 0.596f, 0.114f, 0.592f };

    glm::vec3 rotationVec = glm::vec3{1.0f,0.0f,0.0f};
    float rotationDeg = 270.0f;
    // Position of the grid on the third axis
    float posOnAxis = m_ENDPOS;
    unsigned int allingment = ON_X_Y_AXIS;

    unsigned int modelIndex = 0;

    // Tunnel end
    auto gridDataTunnelEnd = GeometricTools::GenerateGrid3DForInstanceDrawing(m_WIDTH, m_HEIGHT, tileColor, allingment, posOnAxis, rotationVec, rotationDeg);
    auto vboDataEnd = gridDataTunnelEnd.first;
    auto indices = gridDataTunnelEnd.second;

    auto vertexDataEnd = vboDataEnd.first;
    auto modelDataEnd = vboDataEnd.second;
    for (auto model : modelDataEnd)
    {
        models[modelIndex] = model;
        modelIndex++;
    }
    for (unsigned int i = 0; i <= 3; i++)
    {
        // Sets variables need for each wall
        switch (i)
        {
            // Right Wall
        case 0:
            posOnAxis = m_ENDRIGHT;
            allingment = ON_Y_Z_AXIS;
            rotationVec = glm::vec3{ 0.0f, 0.0f, 1.0f };
            rotationDeg = 270.0f;
            break;
            // Left Wall
        case 1:
            posOnAxis = m_ENDLEFT;
            allingment = ON_Y_Z_AXIS;
            rotationVec = glm::vec3{ 0.0f, 0.0f, 1.0f };
            rotationDeg = 90.0f;
            break;
            // Top Wall
        case 2:
            posOnAxis = m_ENDUP;
            allingment = ON_X_Z_AXIS;
            rotationVec = glm::vec3{ 1.0f, 0.0f, 0.0f };
            rotationDeg = 0.0f;
            break;
            // Bottom Wall
        case 3:
            posOnAxis = m_ENDDOWN;
            allingment = ON_X_Z_AXIS;
            rotationVec = glm::vec3{ 1.0f, 0.0f, 0.0f };
            rotationDeg = 180.0f;
            break;
        } 
 
        auto gridDataTunnelWall = GeometricTools::GenerateGrid3DForInstanceDrawing(10, 5, tileColor, allingment, posOnAxis, rotationVec, rotationDeg);
        auto vboDataTunnelWall = gridDataTunnelWall.first;
        //only the model data is needed
        auto modelDataTunnelWall = vboDataTunnelWall.second;

        for (auto model : modelDataTunnelWall)
        {
            models[modelIndex] = model;
            modelIndex++;
        }
    }

    // Create VAO and buffers using VertexBuffer and IndexBuffer classes
    std::shared_ptr<IndexBuffer> IBO = std::make_shared<IndexBuffer>(indices.data(), indices.size());

    std::shared_ptr<VertexBuffer> commonVBO = std::make_shared<VertexBuffer>(vertexDataEnd.data(), vertexDataEnd.size() * sizeof(vertexDataEnd[0]));
    commonVBO->SetLayout(BUFFERLAYOUT);

    BufferLayout instanceModelLayout = BufferLayout({ {ShaderDataType::Mat4, "model"} });
    std::shared_ptr<VertexBuffer> instanceModelVBO = std::make_shared<VertexBuffer>(models.data(), models.size() * sizeof(models[0]));
    instanceModelVBO->SetLayout(instanceModelLayout);
    
    m_tunnelData = std::make_shared<VertexArray>();
    m_tunnelData->Bind();
    m_tunnelData->SetIndexBuffer(IBO);
    m_tunnelData->AddNormalVertexBuffer(commonVBO);
    m_tunnelData->AddPerInstanceVertexBuffer(instanceModelVBO, 1);
}

void Game::createSolidBlock()
{
    m_combinedSolidBlock = std::make_shared<Block>();

    std::vector<float> vertices;

    auto blockData = GeometricTools::GetUnitCube3D();
    auto& verticesData = blockData.first;
    auto& indicesData = blockData.second;

    // Remove color data as it is applied per instance
    for (unsigned int v = 0; v <= verticesData.size() - 1; v += 11)
    {
        vertices.insert(vertices.end(), { verticesData[v], verticesData[v + 1], verticesData[v + 2],
            verticesData[v + 3], verticesData[v + 4], verticesData[v + 5],
            verticesData[v + 6], verticesData[v + 7] });
    }

    m_combinedSolidBlock->IBO = std::make_shared<IndexBuffer>(indicesData.data(), indicesData.size());

    BufferLayout commomLayout = BufferLayout({ {ShaderDataType::Float3, "position"}, {ShaderDataType::Float3, "normals"}, {ShaderDataType::Float2, "tcoords"} });
    m_combinedSolidBlock->VBO = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(vertices[0]));
    m_combinedSolidBlock->VBO->SetLayout(commomLayout);

    m_combinedSolidBlock->VAO = std::make_shared<VertexArray>();
    m_combinedSolidBlock->VAO->Bind();
    m_combinedSolidBlock->VAO->SetIndexBuffer(m_combinedSolidBlock->IBO);
    m_combinedSolidBlock->VAO->AddNormalVertexBuffer(m_combinedSolidBlock->VBO);

}

void Game::createNewComplexBlock()
{
    BlockType type;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 2);

    int num = distrib(gen);

    type = BlockType(num);


    m_activeComplexBlock = std::make_unique<ComplexBlock>(type);
}



void Game::createCamera()
{
    float fov = 45.0f;
    float nearPlane = 1.0f;
    float farPlane = 50.0f;

    glm::vec3 ori = { 0, 0, 0 };
    glm::vec3 up = { 0, 1, 0 };

    m_camera = PerspectiveCamera({ fov, float(WINDOW_WIDTH) , float(WINDOW_HEIGHT), nearPlane, farPlane }, m_CAMERA_POS, ori, up);
}

bool Game::checkCollision(glm::vec3& intendedDir)
{
    if (intendedDir == glm::vec3{0.0f})
    {
        return false;
    }

    std::vector<glm::vec3> nextActiveBlockPosition;
    auto blocks = m_activeComplexBlock->getBlocks();

    for (auto block : blocks)
    {
        nextActiveBlockPosition.push_back(block->position + intendedDir);
    }

    for (auto pos : nextActiveBlockPosition)
    {

        if (pos.x >= m_ENDRIGHT ||
            pos.x < +m_ENDLEFT ||
            pos.y >= m_ENDUP ||
            pos.y <= m_ENDDOWN
            )

        {
            intendedDir = glm::vec3{ 0.0f };
            return false;
        }

        if (pos.z < m_ENDPOS)
        {
            return true;
        }

        if (m_solidBlockIndex != 0)
        {
            unsigned int i = 0;
            while (i < m_solidBlockIndex)
            {
                if (m_solidBlockList[i].position == pos)
                {
                    return true;
                }
                i++;
            }
        }
    }


    return false;
}

bool Game::checkCollisionBeforeRotation(std::array<glm::vec3, 3>& intentedDir)
{
    bool wall = false;
    for (auto pos : intentedDir)
    {

        if (pos.x >= m_ENDRIGHT ||
            pos.x < +m_ENDLEFT ||
            pos.y >= m_ENDUP ||
            pos.y <= m_ENDDOWN
            )
        {
            wall = true;
        }

        if (pos.z < m_ENDPOS)
        {
            return true;
        }

        if (m_solidBlockIndex != 0)
        {

            for (auto i : m_solidBlockList)
            {
                if (i.position == pos)
                {
                    return true;
                }
            }
        }
    }
    if (wall)
    {
        intentedDir[0] = glm::vec3{0.0f};
        intentedDir[1] = glm::vec3{0.0f};
        intentedDir[2] = glm::vec3{0.0f};
    }

    return false;
}

void Game::makeBlockSolid()
{   
    auto blocks = m_activeComplexBlock->getBlocks();
    
    std::vector<glm::vec3> colorData;
    std::vector<glm::mat4> modelData;

    for (int i = 0; i < m_activeComplexBlock->getNumBlocks(); i++)
    {
        m_solidBlockList.push_back(Block(blocks[i]->modelMatrix, blocks[i]->position));
        colorData.push_back(getSolidBlockColor(blocks[i]->position.z));
        modelData.push_back(blocks[i]->modelMatrix);
    }
    m_activeComplexBlock.reset();

    if (m_solidBlockIndex == 0)
    {
        auto instanceData = createInstanceData(colorData, modelData);
        // Setup instance buffer for all cubes
        m_combinedSolidBlock->VAO->SetupInstanceBuffer(instanceData, 250);
    }
    else
    {
        m_combinedSolidBlock->VAO->AddInstances(colorData, modelData);
    }
    m_solidBlockIndex += 4;
    createNewComplexBlock();
}

glm::vec3 Game::getSolidBlockColor(float z)
{
    glm::vec3 color = glm::vec3{};

    if (z == SECTION_1)
    {
        color = glm::vec3(SECTION_1_COLOR);
        return color;
    }
    if (z == SECTION_2)
    {
        color = glm::vec3(SECTION_2_COLOR);
        return color;
    }
    if (z == SECTION_3)
    {
        color = glm::vec3(SECTION_3_COLOR);
        return color;
    }
    if (z == SECTION_4)
    {
        color = glm::vec3(SECTION_4_COLOR);
        return color;
    }
    if (z == SECTION_5)
    {
        color = glm::vec3(SECTION_5_COLOR);
        return color;
    }
    if (z == SECTION_6)
    {
        color = glm::vec3(SECTION_6_COLOR);
        return color;
    }
    if (z == SECTION_7)
    {
        color = glm::vec3(SECTION_7_COLOR);
        return color;
    }
    if (z == SECTION_8)
    {
        color = glm::vec3(SECTION_8_COLOR);
        return color;
    }
    if (z == SECTION_9)
    {
        color = glm::vec3(SECTION_9_COLOR);
        return color;
    }
    if (z == SECTION_10)
    {
        color = glm::vec3(SECTION_10_COLOR);
        return color;
    }


    return color;
}

void Game::dayNightCycle()
{
    angle += ROTATION_SPEED * m_deltaTime;

    m_positionLight.x = ROTATION_RADIUS * cos(angle);
    m_positionLight.z = ROTATION_RADIUS * sin(angle);

    float time = glfwGetTime();
    m_lightMix = (sin( time* CYCLE_SPEED) + 1.0f) * 0.5f;
}

void Game::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    glm::vec3 moveDir = glm::vec3{ 0.0f };

    bool moveCalled = false;
    bool rotCalled = false;

    // Left
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        moveDir.x -= 1.0f;
        moveCalled = true;
    }
    // Right
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        moveDir.x += 1.0f;
        moveCalled = true;
    }
    // Up
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        moveDir.y += 1.0f;
        moveCalled = true;
    }
    // Down
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        moveDir.y -= 1.0f;
        moveCalled = true;
    }
    // Move block one tile back
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        moveDir.z -= 1.0f;
        moveCalled = true;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        moveDir = glm::vec3{ 0.0f, 0.0f, -1.0f };
        while (!game->checkCollision(moveDir))
        {
            game->m_activeComplexBlock->move(moveDir);
        }
        game->makeBlockSolid();
        return;
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        game->m_applyTextures = !game->m_applyTextures;
    }

    std::array<glm::vec3, 3> rotPos = { glm::vec3{0.0}, glm::vec3{0.0}, glm::vec3{0.0} };
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        rotPos = game->m_activeComplexBlock->rotate(PITCH, CLOCKWISE);
        rotCalled = true;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        rotPos = game->m_activeComplexBlock->rotate(YAW, CLOCKWISE);
        rotCalled = true;
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        rotPos = game->m_activeComplexBlock->rotate(ROLL, CLOCKWISE);
        rotCalled = true;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        rotPos = game->m_activeComplexBlock->rotate(PITCH, COUNTER_CLOCKWISE);
        rotCalled = true;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        rotPos = game->m_activeComplexBlock->rotate(YAW, COUNTER_CLOCKWISE);
        rotCalled = true;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        rotPos = game->m_activeComplexBlock->rotate(ROLL, COUNTER_CLOCKWISE);
        rotCalled = true;
    }

    if (rotCalled)
    {
        if (game->checkCollisionBeforeRotation(rotPos))
        {
            game->makeBlockSolid();
            return;
        }
        else 
        {
            if (rotPos[0] != glm::vec3{ 0.0f, 0.0f, 0.0f })
            {
                game->m_activeComplexBlock->applyRotation(rotPos);
                game->m_activeComplexBlock->move(glm::vec3{ 0.0f, 0.0f, 0.0f });
            }
        }
    }

    if (!game->checkCollision(moveDir))
    {
        game->m_activeComplexBlock->move(moveDir);
    }
    else
    {
        game->makeBlockSolid();
    }
}

// Helper function to create instance data array of floats
std::vector<float> Game::createInstanceData(const std::vector<glm::vec3>& colors,
    const std::vector<glm::mat4>& modelMatrices)
{
    std::vector<float> data;
    data.reserve(colors.size() * 19); // 3 for color + 16 for matrix

    for (size_t i = 0; i < colors.size(); i++) {
        // Add color (3 floats)
        data.push_back(colors[i].r);
        data.push_back(colors[i].g);
        data.push_back(colors[i].b);

        // Add model matrix (16 floats, column-major)
        const float* matPtr = glm::value_ptr(modelMatrices[i]);
        for (int j = 0; j < 16; j++) {
            data.push_back(matPtr[j]);
        }
    }

    return data;
}