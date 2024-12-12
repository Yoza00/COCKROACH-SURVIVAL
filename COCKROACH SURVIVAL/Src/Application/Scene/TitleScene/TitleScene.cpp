#include "TitleScene.h"
#include "../SceneManager.h"

//#include"../../GameObject/Stage/Stage.h"
#include"../../GameObject/Stage/Title/House/House.h"
#include"../../GameObject/Character/Player/Player.h"

#include"../../GameObject/Stage/ObjectManager/ObjectManager.h"
#include"../../GameObject/UI/UI.h"
#include"../../GameObject/UI/StartButton/StartButton.h"
#include"../../GameObject/UI/SceneChange/SceneChange.h"

#include"../../main.h"

void TitleScene::Event()
{
	// ============================================
	// デバッグ用
	// ============================================
	/*if (GetAsyncKeyState('O'))
	{
		Application::Instance().SetShowImGui(true);
	}
	if (GetAsyncKeyState('P'))
	{
		Application::Instance().SetShowImGui(false);
	}*/
	// ============================================

	// 左クリックでシーン切替
	if (m_isStart == false)
	{
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			m_isStart = true;

			std::shared_ptr<SceneChange>	_spChange = std::make_shared<SceneChange>();
			_spChange->SetFilePath("Asset/Textures/GameObject/SceneChange/BlackScreen.png");
			_spChange->Init();
			_spChange->SetDrawPos({ 0.0f,0.0f });
			m_UIList.push_back(_spChange);
			m_objList.push_back(_spChange);
		}
	}

	// カメラの行列を作成
	Math::Vector3	_cameraPos = { 0.0f,0.0f,-15.0f };
	Math::Matrix	_transMat = Math::Matrix::CreateTranslation(_cameraPos);
	m_uniCamera->SetCameraMatrix(_transMat);
}

void TitleScene::Init()
{
	ShowCursor(false);

	// 背景オブジェクト
	// 家
	std::shared_ptr<House>	_spHouse = std::make_shared<House>();
	_spHouse->SetModel("Asset/Models/Terrains/TitleScene/House/house_model2.gltf");
	_spHouse->Init();
	m_objList.push_back(_spHouse);

	// プレイヤー
	Math::Vector3	_playerPos = { -0.3f,0.0f,-14.5f };
	//Math::Vector3	_playerPos = { -0.3f,5.0f,-14.5f };
	std::shared_ptr<Player>	_spPlayer = std::make_shared<Player>();
	_spPlayer->Init();
	_spPlayer->SetPos(_playerPos);
	_spPlayer->SetModelRot(0.0f);
	_spPlayer->SetModelHeight(0.185f);
	m_objList.push_back(_spPlayer);

	// UI
	if (ObjectManager::Instance().LoadUIFromJson("Asset/Data/Json/UIData/UIData.json"))
	{
		const std::vector<Object>& _uis = ObjectManager::Instance().GetObjects();

		for (const auto& _ui : _uis)
		{
			if (_ui.m_useScene != "title")
			{
				// 使用場面がタイトルでないものは処理を実行しない
				continue;
			}

			// 更新を行う必要があるUIはm_objListへも追加する
			// 必要がない場合は、m_uiListのみの追加でよい
			if (_ui.m_uiType == "GameTitle")
			{
				std::shared_ptr<UI>	_spUI = std::make_shared<UI>();
				_spUI->SetFilePath(_ui.m_filePath);
				_spUI->Init();
				_spUI->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				m_UIList.push_back(_spUI);
			}
			else if (_ui.m_uiType == "StartButton")
			{
				std::shared_ptr<StartButton> _spStart = std::make_shared<StartButton>();
				_spStart->SetFilePath(_ui.m_filePath);
				_spStart->Init();
				_spStart->SetDrawPos({ _ui.m_pos.x,_ui.m_pos.y });
				m_UIList.push_back(_spStart);
				m_objList.push_back(_spStart);
			}
		}

		// ==============================================
		// エラー防止用
		// 
		// ゲーム中のオブジェクトもUIもm_objectsという配列でデータを管理している。
		// そのため、配列を削除しておかないと、オブジェクトなのにスプライトで描画しようとしたりUIなのにモデル表示しようとしたりしてエラーが発生するので、
		// 防止するためにリストのデータをすべて削除する
		// ==============================================
		ObjectManager::Instance().ListClear();		// リストの中身全削除
	}

	// カメラ
	m_uniCamera = std::make_unique<KdCamera>();

	// ========== サウンド関連 ==========
	//KdAudioManager::Instance().StopAllSound();				// すべての音楽を停止

	// BGMをループ再生
	//KdAudioManager::Instance().Play("Asset/Sounds/BGM/Title/hinatabokko.wav", true)->SetVolume(0.4f);
	// ==================================
}
