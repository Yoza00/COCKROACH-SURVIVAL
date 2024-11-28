#pragma once

#include"../BaseScene/BaseScene.h"
#include"../../GameObject/Stage/ObjectManager/ObjectManager.h"
#include"../Json/nlohmann/json.hpp"

#include"../../ASter/ASter.h"

class Player;
//class TimeLimit;
class Menu_Icon;

class GameScene : public BaseScene
{
public :

	GameScene() { Init(); }
	~GameScene() override {}

private:

	void Event() override;
	void Init() override;

	void RandomMakePos();

	// ========== 経路探索関連 ==========
	bool LoadMapData(const std::string& filePath);

	std::vector<std::vector<int>> m_grid;
	ASter	m_Aster;
	// ==================================

	// 食べ物関連
	int m_foodCnt = 0;					// ゲームシーン中の食べ物を生成するまでのフレーム
	const int MAKE_FOOD_TIME = 300;		// 食べ物生成後、リセット値
	//const int MAKE_FOOD_TIME = 60;		// 食べ物生成後、リセット値
	int m_nowFoodCnt = 0;				// 現在までに生成した食べ物の数
	const int MAX_FOODNUM = 500;		// ゲーム中に登場する食べ物の最大数

	std::weak_ptr<Player>	m_wpPlayer;
	//std::weak_ptr<TimeLimit>m_wpLimit;
	std::weak_ptr<Menu_Icon>m_wpMIcon;

	Math::Vector3	m_makePos = {};
	std::shared_ptr<KdRandomGenerator>	m_RandomGen;

	bool		m_isSceneChange = false;// シーン切替中かどうか
};
