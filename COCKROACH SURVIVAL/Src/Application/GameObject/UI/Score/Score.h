#pragma once

#include"../UI.h"

class Player;

class Score :public UI
{
public:

	Score()							{}
	~Score()			override	{}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	// 指数をセットする
	void SetIndex(int index);

	// プレイヤーのウィークポイントをセット
	void SetPlayer(const std::shared_ptr<Player>& spPlayer)
	{
		m_wpPlayer = spPlayer;
	}

private:

	// 桁数を判定するための変数
	// 10~3-m_indexの位
	int			m_index				= 0;
	const int	MAX_INDEX			= 3;

	int			m_oldScore			= 0;		// 前回更新された時のスコア
	int			m_DrawingScore		= 0;		// 表示しているスコアの値
	const int	TEN_BASE			= 10;		// 10 To The nth Power(10のn乗)
	const int	Maximum_Value		= 9999;		// 表示できる最大値
	float		m_charaWidthSize	= 0.0f;		// 1文字あたりの横幅の大きさ

	bool		m_isMaxValue		= false;	// 表示限界かどうか

	std::weak_ptr<Player>	m_wpPlayer;

	void UpdateRectangle(int newScore);

	// 各位別切り取り開始座標のX座標を更新
	void UpdateStartPosX(int index);
};