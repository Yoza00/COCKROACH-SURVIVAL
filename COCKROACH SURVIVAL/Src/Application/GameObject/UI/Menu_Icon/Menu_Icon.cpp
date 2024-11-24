#include "Menu_Icon.h"

void Menu_Icon::Update()
{
	if (!m_spTex)return;

	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		if (!m_isMenu)
		{
			m_isMenu = true;
		}
	}
}

void Menu_Icon::DrawSprite()
{
	if (!m_spTex)return;

	// メニューを開いている場合は表示しない
	if (m_isMenu)return;

	UI::DrawSprite();
}

void Menu_Icon::Init()
{
	UI::Init();
}
