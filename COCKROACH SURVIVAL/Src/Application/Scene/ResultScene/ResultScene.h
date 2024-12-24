#pragma once

#include"../BaseScene/BaseScene.h"

class ResultScene :public BaseScene
{
public:

	ResultScene()	{ Init(); }
	~ResultScene()	{}

private:

	void Event()	override;
	void Init()		override;

	bool	m_isClick = false;		// クリックされたかどうか

	int		m_drawScore = 0;		// 表示するスコア
};
