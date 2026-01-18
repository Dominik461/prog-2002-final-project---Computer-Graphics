#ifndef BLOCK_H_
#define BLOCK_H_

#include "glm/glm.hpp"
#include "MatrixOperations.h"
#include <iomanip>

struct Block
{
public:
	Block() = default;
	Block(glm::mat4 matrix, glm::vec3 pos)
		:modelMatrix(matrix), position(pos)
	{};
	Block(glm::mat4 matrix, glm::vec3 pos, std::shared_ptr<VertexArray> vao)
		:modelMatrix(matrix), position(pos), VAO(vao)
	{};
	Block(std::shared_ptr<VertexArray> vao)
		:VAO(vao)
	{
	};

	std::shared_ptr<VertexArray> VAO;
	std::shared_ptr<VertexBuffer> VBO;
	std::shared_ptr<VertexBuffer> VBO2;
	std::shared_ptr<VertexBuffer> VBO3;
	std::shared_ptr<IndexBuffer> IBO;

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f };
	void moveBlock(glm::vec3 dir)
	{
		position += dir;
		updateMatrix();
	}
private:
	void updateMatrix()
	{
		glm::vec3 scale = glm::vec3(1.0f);
		glm::vec3 rotationVec = glm::vec3(1.0f, 0.0f, 0.0f);
		float rotationDeg = 0.0f;

		modelMatrix = MatrixOperations::getTransformedMatrix(scale, rotationVec, rotationDeg, position);

	}
};

#endif // !BLOCK_H_