#pragma once

#include"../UI.h"

class Menu_Screen :public UI
{
public:

	Menu_Screen()						{}
	~Menu_Screen()			override	{}

	void DrawSprite()		override;

	void SetAlpha(float alpha)
	{
		m_alpha = alpha;
	}

private:

	float						m_alpha = 0.0f;
};