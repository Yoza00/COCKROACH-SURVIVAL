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

	int			m_index				= 0;	// 対応する桁数
	const int	MAX_INDEX			= 4;	// 指数の最大値
	int			m_drawScore			= 0;	// 表示するスコア
	float		m_charaWidthSize	= 0;	// 1文字当たりの横幅

	bool		m_isUpdateRectangleFinished = false;		// 切り取り範囲の更新が終了したかどうか

	// 切り取り範囲更新
	void UpdateRectangle();

	// 切り取り開始座標(x座標)を切り替え
	void ChangeRectanglePosX(int index);
};