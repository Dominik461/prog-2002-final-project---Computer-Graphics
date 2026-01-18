#ifndef BLOCKData_H_
#define BLOCKData_H_

#include "glm/glm.hpp"
#include "MatrixOperations.h"

struct BlockData
{
public:
	BlockData() = default;
	BlockData(glm::mat4 matrix, glm::vec3 pos)
		:modelMatrix(matrix), position(pos)
	{};
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

#endif // !BLOCKData_H_