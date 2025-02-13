#pragma once

#include"../UI.h"

class Notice :public UI
{
public:

	Notice()						{}
	~Notice()			override	{}

	void Update()		override;
	void DrawSprite()	override;

	void SetIncetanceDeleteLimit(int incetanceDeleteLimit = 120)
	{
		m_incetanceDeleteLimit = incetanceDeleteLimit;
	}

private:

	int	m_incetanceDeleteLimit = 0;		// インスタンス破棄までの時間

};