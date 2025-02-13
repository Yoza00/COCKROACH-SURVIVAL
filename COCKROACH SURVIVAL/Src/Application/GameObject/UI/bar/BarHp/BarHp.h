#pragma once

#include"../../UI.h"

class Player;

class BarHp :public UI
{
public:

	BarHp()							{}
	~BarHp()			override	{}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	// プレイヤー情報をセット
	void SetPlayer(const std::shared_ptr<Player>& _spPlayer)
	{
		m_wpPlayer = _spPlayer;
	}

private:

	float	m_life		= 0.0f;
	float	m_singleW	= 0.0f;

	std::weak_ptr<Player>	m_wpPlayer;
};