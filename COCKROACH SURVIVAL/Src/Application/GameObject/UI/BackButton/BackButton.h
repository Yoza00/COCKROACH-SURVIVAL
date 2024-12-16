#pragma once

#include"../ClickUI/ClickUI.h"

class BackButton :public ClickUI
{
public:

	BackButton() {}
	~BackButton()override{}

private:

	void ButtonClicked()override;

};