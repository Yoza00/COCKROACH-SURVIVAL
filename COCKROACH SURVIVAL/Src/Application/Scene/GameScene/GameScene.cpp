#include "GameScene.h"
#include"../SceneManager.h"
#include"../../main.h"

//#include"../../GameObject/Stage/Stage.h"

#include"../../GameObject/Stage/Ceiling/Ceiling.h"
#include"../../GameObject/Stage/Floor/Floor.h"
#include"../../GameObject/Stage/Wall/Front/WallFront.h"
#include"../../GameObject/Stage/Wall/Back/WallBack.h"
#include"../../GameObject/Stage/Wall/Right/WallRight.h"
#include"../../GameObject/Stage/Wall/Left/WallLeft.h"
#include"../../GameObject/Character/Player/Player.h"
#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"
//#include"../../GameObject/Camera/FPSCamera/FPSCamera.h"
#include"../../GameObject/Stage/ObjectData/ObjectData.h"

#include"../../GameObject/Stage/Food/Food.h"

#include"../../GameObject/UI/UI.h"
#include"../../GameObject/UI/bar/BarHp/BarHp.h"
#include"../../GameObject/UI/bar/BarHungry/BarHungry.h"
//#include"../../GameObject/UI/Clock/TimeLimit/TimeLimit.h"
#include"../../GameObject/UI/SceneChange/SceneChange.h"
#include"../../GameObject/UI/Menu_Icon/Menu_Icon.h"
#include"../../GameObject/UI/Menu_Screen/Menu_Screen.h"
#include"../../GameObject/UI/ClickUI/ClickUI.h"

#include"../../GameObject/Character/Enemy/Enemy.h"

void GameScene::Event()
{
	const std::shared_ptr<Menu_Icon> _spMenu = m_wpMIcon.lock();

	bool _isFlgUpdate = false;		// フラグ更新が必要かどうか

	if (_spMenu)
	{
		if (m_isMenu != _spMenu->GetIsMenu())
		{
			m_isMenu = _spMenu->GetIsMenu();

			_isFlgUpdate = true;	// フラグ更新します！
		}
	}

	if (_isFlgUpdate)
	{
		for (auto& ui : m_uiVec)
		{
			const std::shared_ptr<UI> _spUI = ui.lock();
			if (_spUI)
			{
				_spUI->SetIsMenu(m_isMenu);
				_spUI->SetIsDraw(m_isMenu);
			}
		}

		const std::shared_ptr<TPSCamera> _spCamera = m_wpCamera.lock();
		if (_spCamera)
		{
			_spCamera->SetIsCamRotUpdate(m_isMenu);
		}
	}

	if (m_isMenu)return;

	// ============================================
	// デバッグ用
	// ============================================
	//Application::Instance().m_log.AddLog("%d\n", m_objList.size());

	/*if (GetAsyncKeyState(VK_TAB))
	{
		Application::Instance().SetShowImGui(true);
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		Application::Instance().SetShowImGui(false);
	}*/
	const std::shared_ptr<Player>		_spPlayer	= m_wpPlayer.lock();
	//const std::shared_ptr<TimeLimit>	_spLimit	= m_wpLimit.lock();

	//if (GetAsyncKeyState(VK_SPACE) && GetAsyncKeyState(VK_RSHIFT))
	//{
	//	/*if (m_wpPlayer.expired() == false)
	//	{
	//		m_wpPlayer.lock()->SetPos({0.0f,15.0f,0.0f});
	//	}*/
	//	if (_spPlayer)
	//	{
	//		_spPlayer->SetPos({ 0.0f,15.0f,0.0f });
	//	}
	//}

	//if (_spPlayer->IsDead() == true || _spLimit->GetisFinish() == true)
	//{
	//	
	//}

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

				std::shared_ptr<SceneChange>	_spChange = std::make_shared<SceneChange>();
				_spChange->SetFilePath("Asset/Textures/GameObject/SceneChange/BlackScreen.png");
				_spChange->Init();
				_spChange->SetDrawPos({ 0.0f,0.0f });
				m_UIList.push_back(_spChange);
				m_objList.push_back(_spChange);
			}
		}
	}

	/*if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene(
			SceneManager::SceneType::Result
		);
	}*/

	// ============================================

	/*if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}*/

	// 食べ物の生成処理
	//if (m_nowfoodcnt < max_foodnum)
	//{
	//	m_foodcnt--;
	//	if (m_foodcnt < 0)
	//	{
	//		m_foodcnt = make_food_time;		// 生成までのカウンタを初期化
	//		m_nowfoodcnt++;					// 生成数を更新

	//		// この先、出現させる食べ物の種類を抽選させる
	//		// 現状は1種類だけ出現させるようにする

	//		// 座標はランダムで表示する。
	//		// 未回収のアイテムは、一定時間経過後に削除される

	//		randommakepos();

	//		std::shared_ptr<food>	_spfood = std::make_shared<food>();
	//		_spfood->init();
	//		_spfood->setstate("asset/models/terrains/gamescene/food/food.gltf", m_makepos, 1.0f, m_nowfoodcnt);
	//		//_spfood->setstate("asset/models/terrains/gamescene/food/food.gltf", { (float)m_nowfoodcnt * 0.5f,3.0f,0.0f }, 1.0f, m_nowfoodcnt);
	//		m_objlist.push_back(_spfood);
	//	}
	//}

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
				std::shared_ptr<Food>	_spFood = std::make_shared<Food>();
				_spFood->Init();
				_spFood->SetState("Asset/Models/Terrains/GameScene/Food/Food.gltf", m_makePos, 1.0f, m_nowFoodCnt);
				_spFood->SetPlayer(_spPlayer);
				_spFood->SetParm(5.0f, 5.0f, 100.0f);		// 仮置き(HP5回復、空腹度5回復、スコア100加算)
				m_objList.push_back(_spFood);

				// 生成時間をリセット
				m_foodCnt = MAKE_FOOD_TIME;
			}
		}
	}

	//KdShaderManager::Instance().m_postProcessShader.SetBrightThreshold(0.5f);
}

void GameScene::Init()
{
	ShowCursor(false);

	// ウィークポインタの配列をクリーン
	m_uiVec.clear();

	// ステージの描画関連
	//KdShaderManager::Instance().WorkAmbientController().SetDirLight(Math::Vector3::Down, { 2.5f,2.5f,2.5f });
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 1.0f,1.0f,1.0f,0.75f });
	//KdShaderManager::Instance().WorkAmbientController().SetFogEnable(true, false);
	//KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ 1.0f,1.0f,1.0f }, 0.1f);

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

	/*std::shared_ptr<WallFront>	_spFront = std::make_shared<WallFront>();
	_spFront->Init();
	m_objList.push_back(_spFront);

	std::shared_ptr<WallBack>	_spback = std::make_shared<WallBack>();
	_spback->Init();
	m_objList.push_back(_spback);

	std::shared_ptr<WallRight>	_spRight = std::make_shared<WallRight>();
	_spRight->Init();
	m_objList.push_back(_spRight);

	std::shared_ptr<WallLeft>	_spLeft = std::make_shared<WallLeft>();
	_spLeft->Init();
	m_objList.push_back(_spLeft);*/

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
	Math::Vector3	_playerPos = { -63.74f,12.60f,58.33f };
	std::shared_ptr<Player>	_spPlayer = std::make_shared<Player>();
	_spPlayer->Init();
	_spPlayer->SetPos(_playerPos);
	//_spPlayer->SetModelRot(180.0f);
	_spPlayer->SetModelRotate({ 0.0f,0.0f,0.0f });
	_spPlayer->SetIsTitle(false);
	_spPlayer->SetModelHeight(0.1985f);
	m_objList.push_back(_spPlayer);

	m_wpPlayer = _spPlayer;

	// エネミー
	std::shared_ptr<Enemy>	_spEnemy = std::make_shared<Enemy>(m_grid);
	_spEnemy->Init();
	_spEnemy->SetScale(8.0f);
	m_objList.push_back(_spEnemy);

	// UI
	if (ObjectManager::Instance().LoadUIFromJson("Asset/Data/Json/UIData/UIData.json"))
	{
		const std::vector<Object>& _uis = ObjectManager::Instance().GetObjects();

		for (const auto& _ui : _uis)
		{
			if (_ui.m_useScene != "game")
			{
				// 使用場面がゲームでない場合は処理しない
				continue;
			}

			if (_ui.m_uiType == "bar_hungry")
			{
				std::shared_ptr<BarHungry>	_spBarHung = std::make_shared<BarHungry>();
				_spBarHung->SetFilePath(_ui.m_filePath);
				_spBarHung->Init();
				_spBarHung->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spBarHung->SetPlayer(_spPlayer);
				m_UIList.push_back(_spBarHung);
				m_objList.push_back(_spBarHung);
			}
			else if (_ui.m_uiType == "bar_hp")
			{
				std::shared_ptr<BarHp>	_spBarHp = std::make_shared<BarHp>();
				_spBarHp->SetFilePath(_ui.m_filePath);
				_spBarHp->Init();
				_spBarHp->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spBarHp->SetPlayer(_spPlayer);
				//m_objList.push_back(_spBarHp);
				m_UIList.push_back(_spBarHp);
				m_objList.push_back(_spBarHp);
			}
			else if (_ui.m_uiType == "Menu_Icon")
			{
				std::shared_ptr<Menu_Icon>	_spMIcon = std::make_shared<Menu_Icon>();
				_spMIcon->SetFilePath(_ui.m_filePath);
				_spMIcon->Init();
				_spMIcon->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				m_UIList.push_back(_spMIcon);
				m_objList.push_back(_spMIcon);

				m_wpMIcon = _spMIcon;
			}
			else if (_ui.m_uiType == "Menu_BackScreen")
			{
				std::shared_ptr<Menu_Screen>	_spMSc = std::make_shared<Menu_Screen>();
				_spMSc->SetFilePath(_ui.m_filePath);
				_spMSc->Init();
				_spMSc->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spMSc->SetMIcon(m_wpMIcon.lock());
				_spMSc->SetAlpha(0.5f);
				m_uiVec.push_back(_spMSc);
				m_UIList.push_back(_spMSc);
			}
			else if (_ui.m_uiType == "Menu_Base")
			{
				std::shared_ptr<Menu_Screen>	_spMBase = std::make_shared<Menu_Screen>();
				_spMBase->SetFilePath(_ui.m_filePath);
				_spMBase->Init();
				_spMBase->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spMBase->SetMIcon(m_wpMIcon.lock());
				_spMBase->SetAlpha(1.0f);
				m_uiVec.push_back(_spMBase);
				m_UIList.push_back(_spMBase);
			}
			else if (_ui.m_uiType == "BackButton" || "GuideButton" || "TitleButton")
			{
				std::shared_ptr<ClickUI>	_spUI = std::make_shared<ClickUI>();
				_spUI->SetFilePath(_ui.m_filePath);
				_spUI->Init();
				_spUI->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spUI->SetIsDraw(false);					// 通常は描画しないuiはフラグを解除して描画を行わないようにしておく
				m_UIList.push_back(_spUI);
				m_objList.push_back(_spUI);

				m_uiVec.push_back(_spUI);
			}
			else
			{
				std::shared_ptr<UI>	_spUI = std::make_shared<UI>();
				_spUI->SetFilePath(_ui.m_filePath);
				_spUI->Init();
				_spUI->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				_spUI->SetIsDraw(false);					// 通常は描画しないuiはフラグを解除して描画を行わないようにしておく
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
	//_spCamera->ResistHitObject(_spCeiling);
	_spCamera->ResistHitObject(_spFloor);
	//_spCamera->ResistHitObject(_spFront);
	//_spCamera->ResistHitObject(_spback);
	//_spCamera->ResistHitObject(_spRight);
	//_spCamera->ResistHitObject(_spLeft);
	_spPlayer->SetCamera(_spCamera);		// プレイヤーにカメラのポインタをセット
	_spEnemy->SetPlayer(_spPlayer);

	// 乱数生成機
	m_RandomGen = std::make_shared<KdRandomGenerator>();

	// ========== サウンド関連 ==========
	//KdAudioManager::Instance().StopAllSound();			// すべての音を止める

	//KdAudioManager::Instance().Play("Asset/Sounds/BGM/Game/dozikkomarch.wav", true)->SetVolume(0.4f);		// 指定の音楽をループ再生
	// ==================================
}

void GameScene::RandomMakePos()
{
	float	_minX = -60.0f;
	float	_maxX = 56.8f;
	float	_minZ = -83.2f;
	float	_maxZ = 77.0f;

	float	_posX = m_RandomGen->GetFloat(_minX, _maxX);
	float	_posZ = m_RandomGen->GetFloat(_minZ, _maxZ);

	m_makePos = { _posX,1.4f,_posZ };
}

bool GameScene::LoadMapData(const std::string& filePath)
{
	std::ifstream	_file(filePath);

	if (!_file.is_open())
	{
		return false;
	}

	nlohmann::json	_jsonData;

	_file >> _jsonData;

	_file.close();

	m_grid = _jsonData["map"];

	return true;
}
