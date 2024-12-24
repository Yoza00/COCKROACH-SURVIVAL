#pragma once

#include"../ClickUI.h"

class DrawGuide;

class GuideButton :public ClickUI
{
public:

	GuideButton()					{}
	~GuideButton()		override	{}

	const std::weak_ptr<DrawGuide>& GetWPDrawGuide()const
	{
		return m_wpDrawGuide;
	}

private:

	std::weak_ptr<DrawGuide>	m_wpDrawGuide;

	void ButtonClicked()override;
};