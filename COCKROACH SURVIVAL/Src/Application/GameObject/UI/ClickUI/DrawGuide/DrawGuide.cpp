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

	// 描画フラグチェック
	if (!m_isDraw)return;

	// 切り取り範囲指定
	Math::Rectangle _rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_width),(long)m_height };

	// 描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_drawPos.x, m_drawPos.y, _rec.width, _rec.height, &_rec, nullptr, { 0.5f,0.5f });
}

void DrawGuide::TABPushed()
{
	m_isExpired = true;
}
