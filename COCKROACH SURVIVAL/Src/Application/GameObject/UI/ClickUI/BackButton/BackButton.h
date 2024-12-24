#pragma once

#include"../ClickUI.h"

class Menu_Icon;

class BackButton :public ClickUI
{
public:

	BackButton() {}
	~BackButton()override{}

	void SetWPMIcon(const std::shared_ptr<Menu_Icon>& spMIcon)
	{
		m_wpMIcon = spMIcon;
	}

private:

	std::weak_ptr<Menu_Icon>	m_wpMIcon;

	void ButtonClicked()override;

};