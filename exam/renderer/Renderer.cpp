#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
	:m_tunnelShader(nullptr)
{
	init();
}

void Renderer::setupShaders()
{
	m_tunnelShader = std::make_shared<Shader>(TUNNEL_VERTEX_SHADER_SRC, TUNNEL_FRAGMENT_SHADER_SRC);
	m_blockShader = std::make_shared<Shader>(BLOCK_VERTEX_SHADER_SRC, BLOCK_FRAGMENT_SHADER_SRC);
}

unsigned Renderer::init()
{
	RenderCommands::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	setupShaders();

	return 0;
}

unsigned Renderer::shutdown()
{
	m_tunnelShader->~Shader();
	m_blockShader->~Shader();
	return 0;
}

void Renderer::draw(glm::mat4 viewProjectionMat, glm::vec3 cameraPos, glm::vec3 lightPos, float lightMix, std::shared_ptr<VertexArray>  tunnelData, GLuint tunnelInstances, std::shared_ptr<VertexArray> activeBlockData, GLuint activeBlockInstances, std::shared_ptr<VertexArray> solidBlockData, GLuint solidBlockInstances, bool applyTexture)
{
	RenderCommands::Clear();
	RenderCommands::SetSolidMode();
	
	uploadCommonUniforms(viewProjectionMat, lightPos, cameraPos, lightMix, applyTexture);

	renderTunnel(tunnelData, tunnelInstances);
	
	if (solidBlockInstances > 0)
	{
		renderBlock(solidBlockData, solidBlockInstances, 0.0f);
	}
	renderBlock(activeBlockData, activeBlockInstances, 1.0f);
}

void Renderer::renderTunnel(std::shared_ptr<VertexArray> tunnelData, GLuint instances)
{
	m_tunnelShader->Bind();
	tunnelData->Bind();
	tunnelData->GetIndexBuffer()->Bind();
	// Uploading unifrom values for the border
	m_tunnelShader->UploadUniformFloat1("u_BorderWidth", 0.03f);
	m_tunnelShader->UploadUniformFloat3("u_BorderColor", glm::vec3{ 1.0f, 1.0f, 1.0f });

	RenderCommands::DrawInstance(tunnelData, GL_TRIANGLES, instances);

	tunnelData->Unbind();

	m_tunnelShader->Unbind();
}

void Renderer::renderBlock(std::shared_ptr<VertexArray> blockData, GLuint instances, float isActive)
{
	m_blockShader->Bind();
	blockData->Bind();
	blockData->GetIndexBuffer()->Bind();
	m_blockShader->UploadUniformFloat1("u_IsActive", isActive);
	RenderCommands::DrawInstance(blockData, GL_TRIANGLES, instances);
	blockData->Unbind();
	m_blockShader->Unbind();
}

void Renderer::uploadCommonUniforms(glm::mat4 viewProjectionMatrix, glm::vec3 lightPos, glm::vec3 viewPos, float lightMix, bool applyTex)
{
	m_tunnelShader->Bind();
	m_tunnelShader->UploadUniformMat4("u_ViewProjection", viewProjectionMatrix);
	m_tunnelShader->UploadUniformInt1("u_ApplyTextures", applyTex);
	m_tunnelShader->UploadUniformFloat3("u_LightColorDay", m_LIGHT_COLOR_DAY);
	m_tunnelShader->UploadUniformFloat3("u_LightColorNight", m_LIGHT_COLOR_NIGHT);
	m_tunnelShader->UploadUniformFloat3("u_LightPos", lightPos);
	m_tunnelShader->UploadUniformFloat1("u_LightMix", lightMix);
	m_tunnelShader->UploadUniformFloat1("u_AmbientStrength", m_AMBIENT_STRENGTH);
	m_tunnelShader->UploadUniformFloat1("u_SpecularStrength", m_SPECULAR_STRENGTH);
	m_tunnelShader->UploadUniformFloat3("u_ViewPosition", viewPos);
	m_tunnelShader->Unbind();

	m_blockShader->Bind();
	m_blockShader->UploadUniformMat4("u_ViewProjection", viewProjectionMatrix);
	m_blockShader->UploadUniformInt1("u_ApplyTextures", applyTex);
	m_blockShader->UploadUniformFloat3("u_LightColorDay", m_LIGHT_COLOR_DAY);
	m_blockShader->UploadUniformFloat3("u_LightColorNight", m_LIGHT_COLOR_NIGHT);
	m_blockShader->UploadUniformFloat3("u_LightPos", lightPos);
	m_blockShader->UploadUniformFloat1("u_LightMix", lightMix);
	m_blockShader->UploadUniformFloat1("u_AmbientStrength", m_AMBIENT_STRENGTH);
	m_blockShader->UploadUniformFloat1("u_SpecularStrength", m_SPECULAR_STRENGTH);
	m_blockShader->UploadUniformFloat3("u_ViewPosition", viewPos);
	m_blockShader->Unbind();
}

