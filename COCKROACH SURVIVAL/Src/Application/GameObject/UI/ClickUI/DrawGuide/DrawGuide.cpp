#include "DrawGuide.h"

void DrawGuide::Update()
{
	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		TABPushed();
	}
}

void DrawGuide::DrawSprite()
{
	if (!m_spTex)return;

	// 描画
	KdShaderManager::Instance().m_spriteShader.
		DrawTex(m_spTex, static_cast<int>(m_drawPos.x), static_cast<int>(m_drawPos.y), m_rec.width, m_rec.height, &m_rec);
}

void DrawGuide::TABPushed()
{
	m_isExpired = true;
}
