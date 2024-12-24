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

private:

	// キーの入力制御用
	bool	m_isKeyPush		= false;	// キーが入力されているか
};