#pragma once

#include"../../UI.h"

class Player;

class BarHungry :public UI
{
public:

	BarHungry()						{}
	~BarHungry()		override	{}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	// プレイヤー情報をセット
	void SetPlayer(const std::shared_ptr<Player>& _spPlayer)
	{
		m_wpPlayer = _spPlayer;
	}

private:

	// 何も動作しないとじんわり、アクションを起こすと結構減少する
	float	m_restNum = 0.0f;	// 残量

	float	m_singleW = 0.0f;	// 値が１の時に表示する幅のための変数

	std::weak_ptr<Player>	m_wpPlayer;
};