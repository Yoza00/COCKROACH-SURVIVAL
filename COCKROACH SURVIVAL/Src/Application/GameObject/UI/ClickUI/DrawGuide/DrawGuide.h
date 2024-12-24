#pragma once

#include"../ClickUI.h"

class DrawGuide :public ClickUI
{
public:

	DrawGuide()						{}
	~DrawGuide()		override	{}

	void Update()		override;
	void DrawSprite()	override;

private:

	// TABキーが押された場合の処理
	void TABPushed();

};