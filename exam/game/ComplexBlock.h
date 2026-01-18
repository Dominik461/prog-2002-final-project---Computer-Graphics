#ifndef COMPLEXBLOCKS_H_
#define COMPLEXBLOCKS_H_

#include "BlockData.h"
#include "GeometricTools.h"
#include "VertexArray.h"
#include <array>

enum BlockType
{
	L, 
	Z, 
	T
};

enum RotationAxis {
	YAW,   
	PITCH,  
	ROLL  
};

enum RotationDirection {
	CLOCKWISE,
	COUNTER_CLOCKWISE
};

struct ComplexBlock
{
	public:
		ComplexBlock(BlockType type);
		ComplexBlock() = default;
		std::array< std::shared_ptr<BlockData>, 4> getBlocks() { return m_blocks; }
		glm::vec3 getCenter() { return m_centerPos; }
		std::array<glm::vec3, 4> getCollisonPositions() { return m_collisonPos; }
		unsigned int getNumBlocks() { return m_numBlocks; }
		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<VertexBuffer> VBO2;
		std::shared_ptr<IndexBuffer> IBO;
		void move(glm::vec3 moveDir);
		std::array<glm::vec3, 3> rotate(RotationAxis axis, RotationDirection dir);
		void applyRotation(std::array<glm::vec3, 3> newPos);

	private:
		const glm::vec3 m_COLOR = glm::vec3{ 0.122f, 0.125f, 0.133f };
		std::array<std::shared_ptr<BlockData>, 4> m_blocks;

		// To still have the 1x1x1 and not need to have a messy shader
		// would also allow in the future for 2 or 3 block structures
		unsigned int m_numBlocks;
		glm::vec3 m_centerPos;
		std::array<glm::vec3, 4> m_collisonPos;

		void generateBlockStructur(BlockType type);
};

#endif // !COMPLEXBLOCKS_H_