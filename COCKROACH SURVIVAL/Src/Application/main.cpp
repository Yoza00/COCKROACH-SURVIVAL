#include "main.h"

#include "Scene/SceneManager.h"
#include"GameObject/Stage/ObjectData/ObjectData.h"
#include"GameObject/Stage/ObjectManager/ObjectManager.h"
#include"GameObject/UI/UI.h"

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// エントリーポイント
// アプリケーションはこの関数から進行する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_  HINSTANCE, _In_ LPSTR , _In_ int)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		CoUninitialize();

		return 0;
	}

	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// 実行
	//===================================================================
	Application::Instance().Execute();

	// COM解放
	CoUninitialize();

	return 0;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginUpdate()
{
	// 入力状況の更新
	KdInputManager::Instance().Update();

	// 空間環境の更新
	KdShaderManager::Instance().WorkAmbientController().Update();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostUpdate()
{
	// 3DSoundListnerの行列を更新
	KdAudioManager::Instance().SetListnerMatrix(KdShaderManager::Instance().GetCameraCB().mView.Invert());
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新の前処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PreUpdate()
{
	SceneManager::Instance().PreUpdate();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Update()
{
	SceneManager::Instance().Update();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新の後処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PostUpdate()
{
	SceneManager::Instance().PostUpdate();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginDraw(bool usePostProcess)
{
	KdDirect3D::Instance().ClearBackBuffer();

	KdShaderManager::Instance().WorkAmbientController().Draw();

	if (!usePostProcess) return;
	KdShaderManager::Instance().m_postProcessShader.Draw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostDraw()
{
	// ImGui処理
	ImGuiProcess();

	// BackBuffer -> 画面表示
	KdDirect3D::Instance().WorkSwapChain()->Present(0, 0);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画の前処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PreDraw()
{
	SceneManager::Instance().PreDraw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Draw()
{
	SceneManager::Instance().Draw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画の後処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PostDraw()
{
	// 画面のぼかしや被写界深度処理の実施
	KdShaderManager::Instance().m_postProcessShader.PostEffectProcess();

	// 現在のシーンのデバッグ描画
	SceneManager::Instance().DrawDebug();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 2Dスプライトの描画
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::DrawSprite()
{
	SceneManager::Instance().DrawSprite();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション初期設定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool Application::Init(int w, int h)
{
	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, "3D GameProgramming", "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	bool bFullScreen = false;
	/*if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
		bFullScreen = true;
	}*/

	//===================================================================
	// Direct3D初期化
	//===================================================================

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
#ifdef _DEBUG
	deviceDebugMode = true;
#endif

	// Direct3D初期化
	std::string errorMsg;
	if (KdDirect3D::Instance().Init(m_window.GetWndHandle(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン設定
	if (bFullScreen) {
		HRESULT hr;

		hr = KdDirect3D::Instance().SetFullscreenState(TRUE, 0);
		if (FAILED(hr))
		{
			MessageBoxA(m_window.GetWndHandle(), "フルスクリーン設定失敗", "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
			return false;
		}
	}

	//===================================================================
	// ImGui初期化
	//===================================================================
	ImGuiInit();

	//===================================================================
	// シェーダー初期化
	//===================================================================
	KdShaderManager::Instance().Init();

	//===================================================================
	// オーディオ初期化
	//===================================================================
	KdAudioManager::Instance().Init();

	return true;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション実行
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Execute()
{
	KdCSVData windowData("Asset/Data/WindowSettings.csv");
	const std::vector<std::string>& sizeData = windowData.GetLine(0);

	//===================================================================
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//===================================================================
	if (Application::Instance().Init(atoi(sizeData[0].c_str()), atoi(sizeData[1].c_str())) == false) {
		return;
	}

	//===================================================================
	// ゲームループ
	//===================================================================

	// 時間
	m_fpsController.Init();

	// ループ
	while (1)
	{
		// 処理開始時間Get
		m_fpsController.UpdateStartTime();

		// ゲーム終了指定があるときはループ終了
		if (m_endFlag)
		{
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false)
		{
			break;
		}

		if (GetAsyncKeyState(VK_ESCAPE))
		{
//			if (MessageBoxA(m_window.GetWndHandle(), "本当にゲームを終了しますか？",
//				"終了確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
			{
				End();
			}
		}

		if (IsIconic(m_window.GetWndHandle()))
		{
			continue;
		}

		//=========================================
		//
		// アプリケーション更新処理
		//
		//=========================================

		KdBeginUpdate();
		{
			PreUpdate();

			Update();

			PostUpdate();
		}
		KdPostUpdate();

		//=========================================
		//
		// アプリケーション描画処理
		//
		//=========================================

		KdBeginDraw();
		{
			PreDraw();

			Draw();

			PostDraw();

			DrawSprite();
		}
		KdPostDraw();

		//=========================================
		//
		// フレームレート制御
		//
		//=========================================

		m_fpsController.Update();

		// タイトルバーにFPS表示
		std::string	_titleBar = "COCKROACH SURVIVAL FPS = " + std::to_string(m_fpsController.m_nowfps);
		SetWindowTextA(m_window.GetWndHandle(), _titleBar.c_str());
	}

	//===================================================================
	// アプリケーション解放
	//===================================================================
	Release();
}

// アプリケーション終了
void Application::Release()
{
	KdInputManager::Instance().Release();

	KdShaderManager::Instance().Release();

	KdAudioManager::Instance().Release();

	ImGuiRelease();	// ImGui解放処理

	KdDirect3D::Instance().Release();

	// ウィンドウ削除
	m_window.Release();
}

void Application::ImGuiInit()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	// ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();
	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(m_window.GetWndHandle());
	ImGui_ImplDX11_Init(
		KdDirect3D::Instance().WorkDev(), KdDirect3D::Instance().WorkDevContext());

#include "imgui/ja_glyph_ranges.h"
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontDefault();
	// 日本語対応
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);
}

void Application::ImGuiProcess()
{
	//return;
	//if (!m_showImGui)return;

	//===========================================================
	// ImGui開始
	//===========================================================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//===========================================================
	// 以下にImGui描画処理を記述
	//===========================================================

	// デバッグウィンドウ
	if (ImGui::Begin("Debug Window"))
	{
		// FPS
		ImGui::Text("FPS : %d", m_fpsController.m_nowfps);
	}
	ImGui::End();

	renderGui();

	//char	_ObjectName[100] = "";
	//Math::Vector3	_makePos = {};

	//if (ImGui::Begin("Make Object"))
	//{
	//	// ここで読み込むモデルを決定する
	//	ImGui::InputText("objName", _ObjectName, sizeof(_ObjectName));
	//	if (ImGui::Button("make"))
	//	{
	//		// ボタンクリックで生成
	//	}

	//	// 生成座標を設定
	//	ImGui::SliderFloat("posX", &_makePos.x, -60.0f, 60.0f);
	//	ImGui::SliderFloat("posY", &_makePos.y, 0.0f, 60.0f);
	//	ImGui::SliderFloat("posZ", &_makePos.z, -120.0f, 120.0f);
	//	if (ImGui::Button("save"))
	//	{
	//		// ここでデータを保存
	//	}
	//}
	//ImGui::End();

	// ログウィンドウ
	m_log.Draw("Log Window");

	//=====================================================
	// ログ出力 ・・・ AddLog("～") で追加
	//=====================================================
	
	//m_log.AddLog("hello world\n");

	//=====================================================
	// 別ソースファイルからログを出力する場合
	//=====================================================

	// "main.h"のインクルード必須
	//Application::Instance().m_log.AddLog("TestLog\n");

	//===========================================================
	// ここより上にImGuiの描画はする事
	//===========================================================
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Application::ImGuiRelease()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Application::renderGui()
{
	static const char	_useObjectFilePath[128]	= "Asset/Data/Json/ObjectData/ObjectData.json";
	static const char	_useUIFilePath[128]		= "Asset/Data/Json/UIData/UIData.json";
	static char			_uiUseScene[10]		= "";
	static char			_objectName[128]	= "";
	static char			_objectpath[128]	= "";
	static float		_pos[3]				= { 0.0f,0.0f,0.0f };
	static float		_rot				= 0.0f;
	static float		_scale				= 1.0f;
	static char			_uiType[128] = "";

	std::shared_ptr<ObjectData>	_spObject;
	std::shared_ptr<UI>	_spUI;

	//ImGui::Begin("Map Editor");
	if (ImGui::Begin("Map Editor"))
	{
		// オブジェクト名を決定する
		ImGui::InputText("Object Name", _objectName, IM_ARRAYSIZE(_objectName));

		// オブジェクトのファイルパスを入力
		ImGui::InputText("Object Path", _objectpath, IM_ARRAYSIZE(_objectpath));

		// オブジェクトを生成
		if (ImGui::Button("Generate Object"))
		{
			_spObject = std::make_shared<ObjectData>();
			_spObject->SetPath(_objectpath);
			_spObject->SetName(_objectName);
			_spObject->Init();
			_spObject->SetPos({ _pos[0],_pos[1],_pos[2] });
			_spObject->SetRotationY(_rot);
			SceneManager::Instance().AddObject(_spObject);

			m_wpObject = std::make_shared<ObjectData>();
			m_wpObject = _spObject;
		}

		// ======================================
		// オブジェクト操作関連
		// ======================================

		// 座標
		ImGui::SliderFloat("pos_x", &_pos[0], -60.0f, 60.0f);
		ImGui::SliderFloat("pos_y", &_pos[1], 2.0f, 60.0f);
		ImGui::SliderFloat("pos_z", &_pos[2], -120.0f, 120.0f);

		// 各数値を小数点第2位以下を切り捨てる処理
		// 100倍して小数点第2位で切り捨て
		_pos[0] = std::floor(_pos[0] * 100) / 100;
		_pos[1] = std::floor(_pos[1] * 100) / 100;
		_pos[2] = std::floor(_pos[2] * 100) / 100;

		// オブジェクトの回転
		ImGui::SliderFloat("rotY", &_rot, 0.0f, 180.0f);

		// 小数第２位以下切り捨て
		_rot = std::floor(_rot * 100) / 100;

		// 拡縮
		ImGui::SliderFloat("scale", &_scale, 1.0f, 10.0f);

		// 小数第2位以下切り捨て
		_scale = std::floor(_scale * 100) / 100;

		if (m_wpObject.expired() == false)
		{
			m_wpObject.lock()->SetPos({ _pos[0],_pos[1],_pos[2] });
			m_wpObject.lock()->SetRotationY(_rot);
			m_wpObject.lock()->SetScale(_scale);
		}

		// 保存ボタン
		if (ImGui::Button("Save Object"))
		{
			nlohmann::json	_objectData;

			_objectData["name"] = _objectName;
			_objectData["filePath"] = _objectpath;
			_objectData["position"]["x"] = _pos[0];
			_objectData["position"]["y"] = _pos[1];
			_objectData["position"]["z"] = _pos[2];
			_objectData["rotate"] = _rot;
			_objectData["scale"] = _scale;

			// JSONデータに新たなオブジェクトを追加
			nlohmann::json	_jsonData = ObjectManager::Instance().LoadJsonData(_useObjectFilePath);
			_jsonData["objects"].push_back(_objectData);

			// JSONデータ保存
			ObjectManager::Instance().SaveJsonData(_jsonData, _useObjectFilePath);
		}
	}
	ImGui::End();

	if (ImGui::Begin("UI Editor"))
	{
		// UIの使用シーンを設定
		ImGui::InputText("UI UseScene", _uiUseScene, IM_ARRAYSIZE(_uiUseScene));

		// オブジェクト名を決定する
		ImGui::InputText("UI Name", _objectName, IM_ARRAYSIZE(_objectName));

		// オブジェクトのファイルパスを入力
		ImGui::InputText("UI Path", _objectpath, IM_ARRAYSIZE(_objectpath));

		// UIのタイプを入力
		// タイプ：生成するUIのタイプのことで、このタイプで判断して適切なインスタンスを生成する
		ImGui::InputText("UI Type", _uiType, IM_ARRAYSIZE(_uiType));

		// オブジェクトを生成
		if (ImGui::Button("Generate Object"))
		{
			_spUI = std::make_shared<UI>();
			_spUI->SetFilePath(_objectpath);
			_spUI->SetDrawPos();
			_spUI->Init();
			//SceneManager::Instance().AddObject(_spUI);
			SceneManager::Instance().AddUI(_spUI);

			m_wpUI = std::make_shared<UI>();
			m_wpUI = _spUI;
		}

		// ======================================
		// オブジェクト操作関連
		// ======================================

		// 座標
		ImGui::SliderFloat("pos_x", &_pos[0], -640.0f, 640.0f);
		ImGui::SliderFloat("pos_y", &_pos[1], -360.0f, 360.0f);

		if (m_wpUI.expired() == false)
		{
			m_wpUI.lock()->SetDrawPos({ _pos[0],_pos[1] });
		}

		// 保存ボタン
		if (ImGui::Button("Save Object"))
		{
			nlohmann::json	_objectData;

			_objectData["use"] = _uiUseScene;
			_objectData["type"] = _uiType;
			_objectData["name"] = _objectName;
			_objectData["filePath"] = _objectpath;
			_objectData["position"]["x"] = _pos[0];
			_objectData["position"]["y"] = _pos[1];

			// JSONデータに新たなオブジェクトを追加
			nlohmann::json	_jsonData = ObjectManager::Instance().LoadJsonData(_useUIFilePath);

			_jsonData["UI"].push_back(_objectData);

			// JSONデータ保存
			ObjectManager::Instance().SaveJsonData(_jsonData, _useUIFilePath);
		}
	}
	ImGui::End();
}
