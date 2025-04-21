#pragma once

#include"../../UI.h"

class Food;
class TPSCamera;
class Player;

// 餌に表示される餌の残り体力(食べきれるまで)を表示するバー
class FoodBarHP :public UI
{
public:

	FoodBarHP()						{}
	~FoodBarHP()		override	{}

	void DrawSprite()	override;	// ここで描画する
	void Init()			override;	// ここで初期化する

	void SetFood(const std::shared_ptr<Food>& spFood)
	{
		m_wpTarget = spFood;
	}

	void SetCamera(const std::shared_ptr<TPSCamera>& spCamera)
	{
		m_wpCamera = spCamera;
	}

	void SetPlayer(const std::shared_ptr<Player>& spPlayer)
	{
		m_wpPlayer = spPlayer;
	}

private:

	std::weak_ptr<Food>			m_wpTarget;	// ターゲットとなるオブジェクトのポインタを格納するためのウィークポインタ
	std::weak_ptr<TPSCamera>	m_wpCamera;
	std::weak_ptr<Player>		m_wpPlayer;

	int		m_limit		= 0;		// 無くなるまでのリミット
	int		m_singleW	= 0;		// 1当たりに表示する横幅の大きさ
	//			↑これを使用して切り取り範囲を調節する

	const float	m_correctionValue	= 30.0f;	// 距離の補正値(描画距離を決めるためにかけ合わせる量)

	bool	m_isDrawPosCheckFinish = false;		// 表示座標を確認しきったかどうか
	//			↑更新処理で一度だけ表示座標を確認し、確認しきったらフラグを起動する

	// プレイヤーと餌の距離を計算
	// true：描画する false：描画しない
	bool CheckDistanceFromPlayerToFeed();
};