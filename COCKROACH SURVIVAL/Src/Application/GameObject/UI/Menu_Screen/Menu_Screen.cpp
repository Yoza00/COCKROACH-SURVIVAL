#include "Menu_Screen.h"
#include"../Menu_Icon/Menu_Icon.h"

void Menu_Screen::DrawSprite()
{
	if (!m_spTex)return;

	const std::shared_ptr<Menu_Icon>	_spMIcon = m_wpMIcon.lock();
	if (_spMIcon)
	{
		if (_spMIcon->IsMenu() == false)return;
	}

	Math::Color _color = { 1.0f,1.0f,1.0f,m_alpha };

	// 切り取り範囲指定
	Math::Rectangle _rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_width),(long)m_height };

	// 描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_drawPos.x, m_drawPos.y, _rec.width, _rec.height, &_rec, &_color, { 0.5f,0.5f });
}

void Menu_Screen::Init()
{
	UI::Init();
}
