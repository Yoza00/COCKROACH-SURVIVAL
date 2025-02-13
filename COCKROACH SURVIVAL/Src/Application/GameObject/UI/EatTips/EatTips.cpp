#include "EatTips.h"

void EatTips::DrawSprite()
{
	if (!m_isDraw)return;

	if (!m_spTex)return;

	// 描画
	KdShaderManager::Instance().m_spriteShader.
		DrawTex(m_spTex, static_cast<int>(m_drawPos.x), static_cast<int>(m_drawPos.y), m_rec.width, m_rec.height, &m_rec);
}
