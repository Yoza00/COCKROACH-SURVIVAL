#pragma once

#include"../UI.h"

class Menu_Icon :public UI
{
public:

	Menu_Icon() {}
	~Menu_Icon()		override {}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	// フラグをセット
	void SetIsMenu(bool isMenu)
	{
		m_isMenu = isMenu;
	}

	// フラグを取得
	const bool IsMenu()const { return m_isMenu; }

private:

	bool	m_isMenu = false;		// メニューを開いているかどうか

};