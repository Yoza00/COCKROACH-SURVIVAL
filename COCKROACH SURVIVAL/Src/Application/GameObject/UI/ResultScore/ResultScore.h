#pragma once

#include"../UI.h"

class ResultScore :public UI
{
public:

	ResultScore()							{}
	~ResultScore()				override	{}

	void Update()				override;
	void DrawSprite()			override;
	void Init()					override;

	// 桁数の指数をセット
	void SetIndex(int index)
	{
		m_index = index;
	}

	// 表示しないといけないスコアをセット
	void SetDrawScore(int drawScore)
	{
		m_drawScore = drawScore;
	}

private:

	int		m_index				= 0;	// 対応する桁数
	int		m_drawScore			= 0;	// 表示するスコア
	float	m_charaWidthSize	= 0;	// 1文字当たりの横幅
};