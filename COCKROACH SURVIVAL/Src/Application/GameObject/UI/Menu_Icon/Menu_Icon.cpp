#include "Menu_Icon.h"

void Menu_Icon::Update()
{
	if (!m_spTex)return;

	if (m_isMenu)return;

	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		// フラグが解除されていれば処理を実行する
		// ただし、一度でも入力されればフラグは起動され、
		// 入力を終えるまで解除されない
		if (!m_isKeyPush)
		{
			m_isKeyPush = true;			// フラグ起動

			if (!m_isMenu)
			{
				m_isMenu = true;
			}
		}
	}
	else
	{
		// キー入力がされていなければフラグを解除
		m_isKeyPush = false;
	}
}

void Menu_Icon::DrawSprite()
{
	if (m_isMenu)return;

	UI::DrawSprite();
}

void Menu_Icon::Init()
{
	UI::Init();
}
