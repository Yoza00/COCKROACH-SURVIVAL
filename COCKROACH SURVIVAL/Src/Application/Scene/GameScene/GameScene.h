#pragma once

#include"../BaseScene/BaseScene.h"

class UI;

class Player;
class TPSCamera;

class GameScene : public BaseScene
{
public :

	GameScene() { Init(); }
	~GameScene() override {}

	struct FoodPram
	{
		const float m_addHP			= 5.0f;			// 加算する体力値
		const float	m_addRestNum	= 5.0f;			// 加算する空腹度
		const float m_addScore		= 100.0f;		// 加算するスコア
	};

private:

	void Event() override;
	void Init() override;

	void RandomMakePos();

	// ========== 経路探索関連 ==========
	bool LoadMapData(const std::string& filePath);

	std::vector<std::vector<int>> m_grid;
	// ==================================

	const float	m_minX			= -60.0f;	// マップの左端
	const float	m_maxX			= 56.8f;	// マップの右端
	const float	m_minZ			= -83.2f;	// マップの手前
	const float	m_maxZ			= 77.0f;	// マップの奥
	const float	m_groundHeight	= 0.0f;		// マップの高さ

	// 食べ物関連
	int			m_foodCnt		= 0;		// ゲームシーン中の食べ物を生成するまでのフレーム
	const int	MAKE_FOOD_TIME	= 300;		// 食べ物生成後、リセット値
	int			m_nowFoodCnt	= 0;		// 現在までに生成した食べ物の数
	const int	MAX_FOODNUM		= 30;		// ゲーム中に登場する食べ物の最大数

	// ウィークポインタ関連
	std::weak_ptr<Player>			m_wpPlayer;
	std::weak_ptr<TPSCamera>		m_wpCamera;

	// 食べ物の情報の構造体変数
	FoodPram						m_food;

	// 食べ物の生成に関する変数
	Math::Vector3						m_makePos = {};	// 生成座標
	std::shared_ptr<KdRandomGenerator>	m_RandomGen;	// 乱数計算用

	bool		m_isSceneChange = false;				// シーン切替中かどうか
};
