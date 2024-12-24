#pragma once

#include"../BaseScene/BaseScene.h"

class ResultScene :public BaseScene
{
public:

	ResultScene(int score):m_score(score) { Init(); }
	~ResultScene()	{}

private:

	void Event()	override;
	void Init()		override;

	bool	m_isClick = false;		// クリックされたかどうか

	int		m_score = 0;
};
