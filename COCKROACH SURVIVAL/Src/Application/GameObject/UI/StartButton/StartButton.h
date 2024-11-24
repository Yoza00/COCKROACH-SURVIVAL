#pragma once

#include"../UI.h"

class StartButton :public UI
{
public:

	StartButton() {}
	~StartButton()override {}

	void Update()override;
	void Init()override;

private:

	float		m_angle		= 0.0f;		// sinカーブ用
	float		m_moveSpd	= 1.0f;		// 速度

};