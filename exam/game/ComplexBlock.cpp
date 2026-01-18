#include "ComplexBlock.h"

ComplexBlock::ComplexBlock(BlockType type)
{
	m_numBlocks = 4;
	generateBlockStructur(type);
}

void ComplexBlock::generateBlockStructur(BlockType type = Z)
{
	const float START_POS_Z = 4.5f;
	auto blockData = GeometricTools::GetUnitCube3D(m_COLOR);
	auto vertices = blockData.first;
	auto indices = blockData.second;

	std::array<glm::mat4, 4> blockModels;

	unsigned int blockIndex = 0;

	// Every complex block needs a center
	std::shared_ptr<BlockData> centerBlock = std::make_shared<BlockData>();
	centerBlock->moveBlock(glm::vec3{0.0f, 0.0f, START_POS_Z});
	blockModels[blockIndex] = centerBlock->modelMatrix;
	m_blocks[blockIndex] = centerBlock;
	m_collisonPos[blockIndex] = centerBlock->position;
	m_centerPos = centerBlock->position;

	blockIndex++;

	// Positions of the blocks around origin
	std::array<glm::vec3, 3> positions;
	
	switch (type)
	{
	case L:
		positions[0] = glm::vec3{  0.0f,  1.0f, START_POS_Z};  // above from center
		positions[1] = glm::vec3{  0.0f, -1.0f, START_POS_Z}; // below from center
		positions[2] = glm::vec3{  1.0f, -1.0f, START_POS_Z}; // below to the right from center
		break;
	case Z:
		positions[0] = glm::vec3{  0.0f,  1.0f, START_POS_Z };  // above center
		positions[1] = glm::vec3{  1.0f,  0.0f, START_POS_Z };  // right center
		positions[2] = glm::vec3{  1.0f, -1.0f, START_POS_Z }; // below to the right from center
		break;
	case T:
		positions[0] = glm::vec3{  0.0f,  1.0f, START_POS_Z };  // above from center
		positions[1] = glm::vec3{  1.0f,  0.0f, START_POS_Z };  // rifth from center
		positions[2] = glm::vec3{ -1.0f,  0.0f, START_POS_Z }; // left from center
		break;
	}

		// Create and move remaing 3 blocks to position
		for (unsigned int i = 0; i < 3; i++)
		{
			std::shared_ptr<BlockData> block = std::make_shared<BlockData>();
			block->moveBlock(positions[i]);

			m_blocks[blockIndex] = block;
			blockModels[blockIndex] = block->modelMatrix;
			m_collisonPos[blockIndex] = block->position;
			blockIndex++;
		}
	IBO = std::make_shared<IndexBuffer>(indices.data(), indices.size());

	BufferLayout commomLayout = BufferLayout({ {ShaderDataType::Float3, "position"}, {ShaderDataType::Float3, "normals"}, {ShaderDataType::Float2, "tcoords"}, {ShaderDataType::Float3, "color"} });
	VBO = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(vertices[0]));
	VBO->SetLayout(commomLayout);

	BufferLayout instanceModelLayout = BufferLayout({ {ShaderDataType::Mat4, "model"} });
	VBO2 = std::make_shared<VertexBuffer>(blockModels.data(), blockModels.size() * sizeof(blockModels[0]), GL_DYNAMIC_DRAW);
	VBO2->SetLayout(instanceModelLayout);

	VAO = std::make_shared<VertexArray>();
	VAO->Bind();
	VAO->SetIndexBuffer(IBO);
	VAO->AddNormalVertexBuffer(VBO);
	VAO->AddPerInstanceVertexBuffer(VBO2, 1);
}

void ComplexBlock::move(glm::vec3 moveDir)
{	
	unsigned int offset = 0;
	std::array<glm::mat4, 4> models;

	for (unsigned int i = 0; i < m_numBlocks; i++)
	{ 
		m_blocks[i]->moveBlock(moveDir);
		models[i] = m_blocks[i]->modelMatrix;
		m_collisonPos[i] = m_blocks[i]->position;
	}

	// update center position
	m_centerPos += moveDir;

	VBO2->Bind();
	// Because all blocks are moved, the whole data can be replaced
	VBO2->BufferSubData(offset, sizeof(models[0]) * m_numBlocks, models.data());
}

std::array<glm::vec3, 3> ComplexBlock::rotate(RotationAxis axis, RotationDirection dir)
{

	std::array<glm::vec3, 3> rot;
	for (int i = 1; i < 4; i++)
	{
		glm::mat4 transform = glm::mat4(1.0f);

		glm::vec3 rotationAxis;

		// Rotate 90 degrees
		float angle = glm::radians(90.0f);
		if (dir == RotationDirection::COUNTER_CLOCKWISE) {
			angle = -angle;
		}
		switch (axis) {
		case RotationAxis::YAW:
			rotationAxis = glm::vec3(1.0, 0.0, 0.0);
			break;
		case RotationAxis::PITCH:
			rotationAxis = glm::vec3(0.0, 1.0, 0.0);
			break;
		case RotationAxis::ROLL:
			rotationAxis = glm::vec3(0.0, 0.0, 1.0);
			break;
		}
		
		transform = glm::translate(transform, m_centerPos);
		transform = glm::rotate(transform, angle, rotationAxis);
		transform = glm::translate(transform, -m_centerPos);

		glm::vec3 newPos = glm::vec3(transform * glm::vec4(m_blocks[i]->position, 1.0f));

		// Round to nearest 0.5 increment to fix floating point errors
		rot[i - 1] = glm::round(newPos * 2.0f) / 2.0f;

	}		
	return rot;
}

void ComplexBlock::applyRotation(std::array<glm::vec3, 3> newPos)
{
	for (int i = 1; i < 4; i++)
	{
		if (newPos[i-1] != m_centerPos)
		{
			m_blocks[i]->position = newPos[i - 1];
		}
	}
}
