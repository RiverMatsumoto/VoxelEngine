#include "Block.h"

Block::Block()
{
	data = 0;
}


Block::~Block()
{

}

Block::Block(const Block& block)
{
	data = block.data;
}

bool Block::IsActive()
{
	return data >> 7;
}

void Block::SetActive(bool active)
{
	// 0bx000'0000 set x to whatever value active is
	if (active)
		data |= 1 << 7;
	else
		data &= ~(1 << 7);
}

void Block::SetBlockType(BlockType blockType)
{
	data &= ((unsigned char)blockType);
}

BlockType Block::GetBlockType()
{
	return (BlockType) (data & 0b0111'1111);
}
