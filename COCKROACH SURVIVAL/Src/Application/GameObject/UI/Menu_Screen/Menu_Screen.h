#pragma once

#include"../UI.h"

class Menu_Icon;

class Menu_Screen :public UI
{
public:

	Menu_Screen()						{}
	~Menu_Screen()			override	{}

	void DrawSprite()		override;
	void Init()				override;

	void SetAlpha(float alpha)
	{
		m_alpha = alpha;
	}

	void SetMIcon(const std::shared_ptr<Menu_Icon>& spMIcon)
	{
		m_wpMIcon = spMIcon;
	}

private:

	std::weak_ptr<Menu_Icon>	m_wpMIcon;
	float						m_alpha = 0.0f;
};