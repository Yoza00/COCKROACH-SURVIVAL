#include "Menu_Screen.h"

void Menu_Screen::DrawSprite()
{
	if (!m_spTex)return;

	if (!m_isMenu)return;

	Math::Color _color = { 1.0f,1.0f,1.0f,m_alpha };

	// 描画
	KdShaderManager::Instance().m_spriteShader.
		DrawTex(m_spTex, static_cast<int>(m_drawPos.x), static_cast<int>(m_drawPos.y), m_rec.width, m_rec.height, &m_rec, &_color);
}
