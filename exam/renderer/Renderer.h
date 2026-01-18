#ifndef RENDERER_H_
#define RENDERER_H_

#include "ShaderDefenitionsTunnel.h"
#include "ShaderDefenitionsBlocks.h"
#include "PerspectiveCamera.h"
#include "RenderCommands.h"
#include "Shader.h"
#include "Block.h"
#include "glm/glm.hpp"
#include <iostream>
#include <iomanip>

class Renderer
{
private:
	const glm::vec3 m_LIGHT_COLOR_DAY = glm::vec3{ 0.839f, 0.753f, 0.753f };
	const glm::vec3 m_LIGHT_COLOR_NIGHT = glm::vec3{ 0.123f, 0.04f, 0.136f };
	const float m_AMBIENT_STRENGTH = 0.75;
	const float m_SPECULAR_STRENGTH = 1.0;

	std::shared_ptr<Shader> m_tunnelShader;
	std::shared_ptr<Shader> m_blockShader;

	void setupShaders();

	void renderTunnel(std::shared_ptr<VertexArray> tunnelData, GLuint instances);
	void renderBlock(std::shared_ptr<VertexArray> blockData, GLuint instances, float isActive);
	void uploadCommonUniforms(glm::mat4 viewProjectionMatrix, glm::vec3 lightPos, glm::vec3 viewPos, float lightMix, bool applyTex);

public:  
	Renderer();

	unsigned init();
	void draw(glm::mat4 viewProjectionMat, glm::vec3 cameraPos, glm::vec3 lightPos, float lightMix, std::shared_ptr<VertexArray> tunnelData, GLuint tunnelInstances, std::shared_ptr<VertexArray> activeBlockData, GLuint activeBlockinstances, std::shared_ptr<VertexArray> solidBlockData, GLuint solidBlockInstances, bool applyTexture);
	unsigned shutdown();
};


#endif // !RENDERER_H_

