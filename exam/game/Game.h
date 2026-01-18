#ifndef GAME_H_
#define GAME_H_

// Definitions to handle game logic easier
// Sections 
#define SECTION_1 4.5f
#define SECTION_2 3.5f
#define SECTION_3 2.5f
#define SECTION_4 1.5f
#define SECTION_5 0.5f
#define SECTION_6 -1.5f
#define SECTION_7 -2.5f
#define SECTION_8 -3.5f
#define SECTION_9 -4.5f
#define SECTION_10 -5.5f

// Color for each section
#define SECTION_1_COLOR {0.0f, 1.0f, 0.0f}
#define SECTION_2_COLOR {0.886f, 0.322f, 0.184f}
#define SECTION_3_COLOR {0.188f, 0.890f, 0.659f}
#define SECTION_4_COLOR {0.0f, 0.659f, 0.420f}
#define SECTION_5_COLOR {0.0f, 0.0f, 1.0f}
#define SECTION_6_COLOR {1.0f, 1.0f, 0.0f}
#define SECTION_7_COLOR {0.0f, 1.0f, 1.0f}
#define SECTION_8_COLOR {0.659f, 0.118f, 0.0f}
#define SECTION_9_COLOR {0.549f, 0.573f, 0.675f}
#define SECTION_10_COLOR {0.780f, 0.141f, 0.694f}

#include "GLFWApplication.h"
#include "GeometricTools.h"
#include "Renderer.h"
#include "PerspectiveCamera.h"
#include "MatrixOperations.h"
#include "glm/glm.hpp"
#include "Block.h"
#include "ComplexBlock.h"
#include "TextureManager.h"
#include <math.h>
#include <iostream>
#include <random>

class Game : GLFWApplication
{
public:
	Game();
	~Game();

	unsigned init() override;
	unsigned run() override;
	void shutdown() override;

private:
	GLFWwindow* m_window;

	PerspectiveCamera m_camera;
	const glm::vec3 m_CAMERA_POS = glm::vec3(0.0f, 0.0f, 11.0f);

	const float m_ENDPOS = -5.0f;
	const float m_ENDRIGHT = 2.5f;
	const float m_ENDLEFT = -2.5f;
	const float m_ENDUP = 2.5f;
	const float m_ENDDOWN = -2.5f;

	const unsigned int m_DEPTH = 10;
	const unsigned int m_HEIGHT = 5;
	const unsigned int m_WIDTH = 5;

	std::shared_ptr<Renderer> m_renderer;

	const BufferLayout BUFFERLAYOUT = BufferLayout({ {ShaderDataType::Float3, "position"}, {ShaderDataType::Float3, "normals"}, {ShaderDataType::Float2, "tcoords"}, {ShaderDataType::Float3, "color"} });

	std::shared_ptr<VertexArray> m_tunnelData;
	const unsigned int m_TUNNNEL_TILE_NUMBER = m_DEPTH * m_HEIGHT * m_WIDTH;

	std::shared_ptr <Block> m_combinedSolidBlock;
	std::unique_ptr<ComplexBlock> m_activeComplexBlock;
	std::vector<Block> m_solidBlockList;
	unsigned int m_solidBlockIndex = 0;

	bool m_forceMovePossible = true;
	bool m_applyTextures = false;

	glm::vec3 m_positionLight = glm::vec3{ 0.0f, 0.0f, 14.0f };
	float m_lightMix = 0.0f;
	const float ROTATION_RADIUS = 14.0f;
	const float ROTATION_SPEED = 0.5f;
	const float CYCLE_SPEED = 0.2f;

	// Delta time for frame-independent movement
	float m_deltaTime;
	float m_lastFrameTime;

	float angle = 0.0f;

	void createTunnel();
	void createSolidBlock();
	void createNewComplexBlock();
	void createCamera();
	
	bool checkCollision(glm::vec3& intentedDir);
	bool checkCollisionBeforeRotation(std::array<glm::vec3, 3>& intentedDir);
	void makeBlockSolid();
	void updateCombinedSolidBlock();
	glm::vec3 getSolidBlockColor(float z);

	void dayNightCycle();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	GLuint instanceVbo;

	std::vector<float> createInstanceData(const std::vector<glm::vec3>& colors,
		const std::vector<glm::mat4>& modelMatrices);
};

#endif // !GAME_H_
