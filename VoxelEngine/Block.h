#pragma once

enum BlockType
{
	BlockType_Default = 0,
	BlockType_Grass,
	BlockType_Dirt,
	BlockType_Water,
	BlockType_Stone,
	BlockType_Wood,
	BlockType_Sand,
	BlockType_NumTypes
};

class Block
{
public:
	static const float BLOCK_RENDER_SIZE;

	Block();
	Block(const Block& block);
	~Block();
	bool IsActive();
	void SetActive(bool active);
	BlockType GetBlockType();
	void SetBlockType(BlockType blockType);
private:
	unsigned char data;
};

