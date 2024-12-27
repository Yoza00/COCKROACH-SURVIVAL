#include "Rank.h"

void Rank::DrawSprite()
{
	if (!m_spTex)return;

	// 切り取り範囲指定
	Math::Rectangle _rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_width),(long)m_height };

	// 描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_drawPos.x, m_drawPos.y, _rec.width, _rec.height, &_rec, nullptr, { 0.0f,0.5f });
}