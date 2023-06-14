#include "Block.h"

Block::Block()
{
	Block::m_blockType = BlockType_Default;
	m_active = false;
}

Block::~Block()
{

}

bool Block::IsActive()
{
	return m_active;
}

void Block::SetActive(bool active)
{
	m_blockType = BlockType_Default;
	m_active = active;
}
