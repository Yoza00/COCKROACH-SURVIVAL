#include "GameScene.h"
#include"../SceneManager.h"
#include"../../GameObject/Stage/ObjectManager/ObjectManager.h"
#include"../../main.h"

#include"../../GameObject/Stage/Floor/Floor.h"
#include"../../GameObject/Character/Player/Player.h"
#include"../../GameObject/Character/Enemy/Enemy.h"
#include"../../GameObject/Weapon/Weapon.h"
#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include"../../GameObject/Stage/ObjectData/ObjectData.h"

#include"../../GameObject/Stage/Food/Food.h"
#include"../../GameObject/UI/Bar/FoodBarHP/FoodBarHP.h"

#include"../../GameObject/UI/UI.h"
#include"../../GameObject/UI/Notice/Notice.h"
#include"../../GameObject/UI/Bar/BarHp/BarHp.h"
#include"../../GameObject/UI/Bar/BarHungry/BarHungry.h"
#include"../../GameObject/UI/SceneChange/SceneChange.h"
#include"../../GameObject/UI/Menu_Icon/Menu_Icon.h"
#include"../../GameObject/UI/Menu_Screen/Menu_Screen.h"
#include"../../GameObject/UI/ClickUI/BackButton/BackButton.h"
#include"../../GameObject/UI/ClickUI/GuideButton/GuideButton.h"
#include"../../GameObject/UI/ClickUI/DrawGuide/DrawGuide.h"
#include"../../GameObject/UI/ClickUI/TitleButton/TitleButton.h"
#include"../../GameObject/UI/Cursor/Cursor.h"
#include"../../GameObject/UI/Score/Score.h"
#include"../../GameObject/UI/EatTips/EatTips.h"

void GameScene::Event()
{
	const std::shared_ptr<Player>		_spPlayer	= m_wpPlayer.lock();

	if (_spPlayer)
	{
		if (GetAsyncKeyState(VK_SPACE) && GetAsyncKeyState(VK_RSHIFT))
		{
			_spPlayer->SetPos({ 0.0f,15.0f,0.0f });
		}

		if (_spPlayer->IsDead() == true)
		{
			if (!m_isSceneChange)
			{
				m_isSceneChange = true;

				std::shared_ptr<Notice>	_spNotice = std::make_shared<Notice>();
				_spNotice->SetFilePath("Asset/Textures/GameObject/UI/Notice/Finish.png");
				_spNotice->Init();
				_spNotice->SetDrawPos(Math::Vector2::Zero);
				_spNotice->SetIncetanceDeleteLimit();
				m_objList.push_back(_spNotice);

				std::shared_ptr<SceneChange>	_spChange = std::make_shared<SceneChange>();
				_spChange->SetFilePath("Asset/Textures/GameObject/SceneChange/BlackScreen.png");
				_spChange->Init();
				_spChange->SetDrawPos(Math::Vector2::Zero);
				m_objList.push_back(_spChange);

				int	_score = _spPlayer->GetScore();
				SceneManager::Instance().SetScore(_score);
				return;
			}
		}
	}

	// 食べ物の生成処理
	// 現在までに生成された食べ物の個数が最大数を超えていないか確認
	if (m_nowFoodCnt < MAX_FOODNUM)
	{
		m_foodCnt--;				// 食べ物の生成カウンタを更新
		
		// 生成カウンタがなくなれば生成処理を行う
		if (m_foodCnt < 0)
		{
			// 生成座標をランダムで決定
			RandomMakePos();
			
			if (_spPlayer)
			{
				// 生成
				{
					std::shared_ptr<Food>	_spFood = std::make_shared<Food>();
					_spFood->Init();
					_spFood->SetState("Asset/Models/Terrains/GameScene/Food/Cookie.gltf", m_makePos, 1.0f, m_nowFoodCnt);
					_spFood->SetPlayer(_spPlayer);
					_spFood->SetParm(m_food.m_addHP, m_food.m_addRestNum, m_food.m_addScore);		// 仮置き(HP5回復、空腹度5回復、スコア100加算)
					m_objList.push_back(_spFood);


					// ここに餌のバーのインスタンスを作成するように記載する
					// プレイヤー情報を取得し、一定の距離内にならないと表示されないようにしておく
					{
						std::shared_ptr<FoodBarHP>	_spFoodBar = std::make_shared<FoodBarHP>();
						_spFoodBar->SetFilePath("Asset/Textures/GameObject/UI/Bar/FoodBar/FoodHPBar.png");
						_spFoodBar->Init();
						_spFoodBar->SetDrawPos();
						m_objList.push_back(_spFoodBar);

						_spFoodBar->SetCamera(m_wpCamera.lock());
						_spFoodBar->SetFood(_spFood);
						_spFoodBar->SetPlayer(m_wpPlayer.lock());
					}
				}

				// 生成時間をリセット
				m_foodCnt = MAKE_FOOD_TIME;
			}
		}
	}
}

void GameScene::Init()
{
	// ステージの描画関連
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 1.0f,1.0f,1.0f,0.75f });

	m_foodCnt = MAKE_FOOD_TIME;

	std::shared_ptr<Floor>	_spFloor = std::make_shared<Floor>();
	_spFloor->Init();
	m_objList.push_back(_spFloor);

	// マップデータ
	if (!LoadMapData("Asset/Data/Json/MapData/MapData.json"))
	{
		assert(0 && "マップデータの読み込みに失敗しました。");
		return;
	}

	// ステージ内のオブジェクト
	if (ObjectManager::Instance().LoadObjectsFromJson("Asset/Data/Json/ObjectData/ObjectData.json"))
	{
		std::shared_ptr<ObjectData>	_spObject;

		const auto& _objects = ObjectManager::Instance().GetObjects();

		for (const auto& _object : _objects)
		{
			// オブジェクト生成・必要なデータをセット
			_spObject = std::make_shared<ObjectData>();
			_spObject->SetPath(_object.m_filePath);
			_spObject->SetName(_object.m_name);
			_spObject->Init();
			_spObject->SetPos({ _object.m_pos.x,_object.m_pos.y,_object.m_pos.z });
			_spObject->SetRotationY(_object.m_rot);
			_spObject->SetScale(_object.m_scale);
			m_objList.push_back(_spObject);
		}

		// ==============================================
		// エラー防止用
		// ==============================================
		ObjectManager::Instance().ListClear();		// リストの中身全削除
	}

	// プレイヤー
	Math::Vector3	_playerPos = { -60.0f,0.50f,58.33f };
	std::shared_ptr<Player>	_spPlayer = std::make_shared<Player>();
	_spPlayer->Init();
	_spPlayer->SetPos(_playerPos);
	_spPlayer->SetIsTitle(false);
	_spPlayer->SetModelHeight(0.1985f);
	m_objList.push_back(_spPlayer);

	m_wpPlayer = _spPlayer;

	// エネミー
	std::shared_ptr<Enemy>	_spEnemy = std::make_shared<Enemy>(m_grid);
	_spEnemy->Init();
	_spEnemy->SetScale(8.0f);
	m_objList.push_back(_spEnemy);

	// スプレー
	std::shared_ptr<Weapon>	_spWeapon = std::make_shared<Weapon>();
	_spWeapon->Init();
	m_objList.push_back(_spWeapon);

	// UI
	if (ObjectManager::Instance().LoadUIFromJson("Asset/Data/Json/UIData/UIData.json", "game"))
	{
		// Menu_Iconクラスのシェアードポインタをコピーしておくためのローカル変数
		// このif文の中のみ有効
		std::shared_ptr<Menu_Icon>	_spCopyMenuIcon = nullptr;

		const std::vector<Object>& _uis = ObjectManager::Instance().GetObjects();

		for (const auto& _ui : _uis)
		{
			if (_ui.m_uiType == "score")
			{
				for (int i = 0; i < 4; i++)
				{
					std::shared_ptr<Score>	_spScore = std::make_shared<Score>();
					_spScore->SetFilePath(_ui.m_filePath);
					_spScore->Init();
					_spScore->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
					_spScore->SetIndex(i);
					_spScore->SetPlayer(_spPlayer);
					m_objList.push_back(_spScore);
				}
			}
			else if (_ui.m_uiType == "Notice")
			{
				std::shared_ptr<Notice>	_spNotice = std::make_shared<Notice>();
				_spNotice->SetFilePath(_ui.m_filePath);
				_spNotice->Init();
				_spNotice->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spNotice->SetIncetanceDeleteLimit();
				m_objList.push_back(_spNotice);

				_spPlayer->SetNotice(_spNotice);
			}
			else if (_ui.m_uiType == "Tips")
			{
				std::shared_ptr<EatTips>	_spTips = std::make_shared<EatTips>();
				_spTips->SetFilePath(_ui.m_filePath);
				_spTips->Init();
				_spTips->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				m_objList.push_back(_spTips);

				_spPlayer->SetTips(_spTips);
			}
			else if (_ui.m_uiType == "bar_hungry")
			{
				std::shared_ptr<BarHungry>	_spBarHung = std::make_shared<BarHungry>();
				_spBarHung->SetFilePath(_ui.m_filePath);
				_spBarHung->Init();
				_spBarHung->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spBarHung->SetPlayer(_spPlayer);
				m_objList.push_back(_spBarHung);
			}
			else if (_ui.m_uiType == "bar_hp")
			{
				std::shared_ptr<BarHp>	_spBarHp = std::make_shared<BarHp>();
				_spBarHp->SetFilePath(_ui.m_filePath);
				_spBarHp->Init();
				_spBarHp->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spBarHp->SetPlayer(_spPlayer);
				m_objList.push_back(_spBarHp);
			}
			else if (_ui.m_uiType == "bar_frame" || _ui.m_uiType == "HP" || _ui.m_uiType == "Hungry")
			{
				std::shared_ptr<UI>	_spUI = std::make_shared<UI>();
				_spUI->SetFilePath(_ui.m_filePath);
				_spUI->Init();
				_spUI->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				m_objList.push_back(_spUI);
			}
			else if (_ui.m_uiType == "Menu_Icon")
			{
				std::shared_ptr<Menu_Icon>	_spMIcon = std::make_shared<Menu_Icon>();
				_spMIcon->SetFilePath(_ui.m_filePath);
				_spMIcon->Init();
				_spMIcon->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				m_objList.push_back(_spMIcon);

				_spCopyMenuIcon = _spMIcon;
			}
			else if (_ui.m_uiType == "Menu_BackScreen")
			{
				std::shared_ptr<Menu_Screen>	_spMSc = std::make_shared<Menu_Screen>();
				_spMSc->SetFilePath(_ui.m_filePath);
				_spMSc->Init();
				_spMSc->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spMSc->SetAlpha(0.5f);
				m_UIList.push_back(_spMSc);
			}
			else if (_ui.m_uiType == "Menu_Base")
			{
				std::shared_ptr<Menu_Screen>	_spMBase = std::make_shared<Menu_Screen>();
				_spMBase->SetFilePath(_ui.m_filePath);
				_spMBase->Init();
				_spMBase->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spMBase->SetAlpha(1.0f);
				m_UIList.push_back(_spMBase);
			}
			else if (_ui.m_uiType == "BackButton")
			{
				std::shared_ptr<BackButton>	_spBB = std::make_shared<BackButton>();
				_spBB->SetFilePath(_ui.m_filePath);
				_spBB->Init();
				_spBB->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				m_UIList.push_back(_spBB);

				_spBB->SetWPMIcon(_spCopyMenuIcon);
			}
			else if (_ui.m_uiType == "GuideButton")
			{
				std::shared_ptr<GuideButton> _spGB = std::make_shared<GuideButton>();
				_spGB->SetFilePath(_ui.m_filePath);
				_spGB->Init();
				_spGB->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				m_UIList.push_back(_spGB);
			}
			else if (_ui.m_uiType == "TitleButton")
			{
				std::shared_ptr<TitleButton>	_spTB = std::make_shared<TitleButton>();
				_spTB->SetFilePath(_ui.m_filePath);
				_spTB->Init();
				_spTB->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				m_UIList.push_back(_spTB);
			}
			else if (_ui.m_uiType == "cursor")
			{
				std::shared_ptr<Cursor>	_spCursor = std::make_shared<Cursor>();
				_spCursor->SetFilePath(_ui.m_filePath);
				_spCursor->Init();
				_spCursor->SetDrawPos();

				m_UIList.push_back(_spCursor);
			}
			else
			{
				std::shared_ptr<UI>	_spUI = std::make_shared<UI>();
				_spUI->SetFilePath(_ui.m_filePath);
				_spUI->Init();
				_spUI->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				m_UIList.push_back(_spUI);
			}
		}

		// ==============================================
		// エラー防止用
		// ==============================================
		ObjectManager::Instance().ListClear();		// リストの中身全削除
	}

	// カメラ
	std::shared_ptr<TPSCamera>	_spCamera = std::make_shared<TPSCamera>();
	//std::shared_ptr<FPSCamera>	_spCamera = std::make_shared<FPSCamera>();
	_spCamera->Init();
	m_objList.push_back(_spCamera);

	m_wpCamera = _spCamera;

	// 各オブジェクトに必要なデータを渡す
	_spCamera->SetTarget(_spPlayer);		// カメラの追尾対象のポインタをセット
	_spCamera->ResistHitObject(_spFloor);
	_spPlayer->SetCamera(_spCamera);		// プレイヤーにカメラのポインタをセット
	_spEnemy->SetPlayer(_spPlayer);
	_spWeapon->SetParent(_spEnemy);
	_spEnemy->SetWeapon(_spWeapon);

	KdEffekseerManager::GetInstance().SetCamera(_spCamera->GetCamera());

	// 乱数生成機
	m_RandomGen = std::make_shared<KdRandomGenerator>();

	// ========== サウンド関連 ==========
	//KdAudioManager::Instance().StopAllSound();			// すべての音を止める

	KdAudioManager::Instance().Play("Asset/Sounds/BGM/Game/dozikkomarch.wav", true)->SetVolume(0.4f);		// 指定の音楽をループ再生
	// ==================================
}

void GameScene::RandomMakePos()
{
	float	_posX = m_RandomGen->GetFloat(m_minX, m_maxX);
	float	_posZ = m_RandomGen->GetFloat(m_minZ, m_maxZ);

	m_makePos = { _posX,m_groundHeight,_posZ };
}

bool GameScene::LoadMapData(const std::string& filePath)
{
	std::ifstream	_file(filePath);

	// オープンチェック
	if (!_file.is_open())
	{
		return false;
	}

	nlohmann::json	_jsonData;

	// 開いたファイルのデータをjsonにコピーしてファイルを閉じる
	_file >> _jsonData;

	_file.close();

	// コピーされたjsonデータの"map"グループの中身をm_gridに格納
	// m_gridは２次元可変長配列
	m_grid = _jsonData["map"];

	return true;
}
