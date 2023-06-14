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
	~Block();
	bool IsActive();
	void SetActive(bool active);
private:
	bool m_active;
	BlockType m_blockType;
};

