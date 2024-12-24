#pragma once

#include"../UI.h"

class Cursor :public UI
{
public:

	Cursor()						{}
	~Cursor()			override	{}

	void Update()		override;
	void DrawSprite()	override;

private:

	// マウス座標
	POINT m_mousePos = {};

};