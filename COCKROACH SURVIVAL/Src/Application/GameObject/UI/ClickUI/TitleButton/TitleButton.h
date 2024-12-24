#pragma once

#include"../ClickUI.h"

class SceneChange;

class TitleButton :public ClickUI
{
public:

	TitleButton()						{}
	~TitleButton()			override	{}

	void SetWPScChange(const std::shared_ptr<SceneChange>& spScChange)
	{
		m_wpScChange = spScChange;
	}

private:

	std::weak_ptr<SceneChange>	m_wpScChange;

	void ButtonClicked()	override;

};