#pragma once

#include"../BaseScene/BaseScene.h"

class UI;

class Player;
class Menu_Icon;
class GuideButton;
class TPSCamera;

class GameScene : public BaseScene
{
public :

	GameScene() { Init(); }
	~GameScene() override {}

	const bool GetIsMenu()const override
	{
		return m_isMenu;
	}

private:

	void Event() override;
	void Init() override;

	void RandomMakePos();

	// ========== 経路探索関連 ==========
	bool LoadMapData(const std::string& filePath);

	std::vector<std::vector<int>> m_grid;
	// ==================================

	// 食べ物関連
	int			m_foodCnt = 0;			// ゲームシーン中の食べ物を生成するまでのフレーム
	const int	MAKE_FOOD_TIME = 300;	// 食べ物生成後、リセット値
	//const int MAKE_FOOD_TIME = 60;	// 食べ物生成後、リセット値
	int			m_nowFoodCnt = 0;		// 現在までに生成した食べ物の数
	const int	MAX_FOODNUM = 500;		// ゲーム中に登場する食べ物の最大数

	// ウィークポインタ関連
	std::weak_ptr<Player>			m_wpPlayer;
	//std::weak_ptr<TimeLimit>m_wpLimit;
	std::weak_ptr<Menu_Icon>		m_wpMIcon;
	std::weak_ptr<GuideButton>		m_wpGuideButton;
	std::weak_ptr<TPSCamera>		m_wpCamera;

	// ウィークポインタの配列(メニュー画面の際に、フラグなどに変更があるUIのポインタを格納しておくための配列)
	std::vector<std::weak_ptr<UI>>	m_uiVec;

	// 食べ物の生成に関する変数
	Math::Vector3						m_makePos = {};	// 生成座標
	std::shared_ptr<KdRandomGenerator>	m_RandomGen;	// 乱数計算用

	bool		m_isSceneChange = false;// シーン切替中かどうか
	bool		m_isMenu		= false;// メニュー画面かどうか
};
