﻿#pragma once

class BaseScene;

// UI用
class UI;

class SceneManager
{
public:

	// シーン情報
	enum class SceneType
	{
		Title,
		Game,
		Result
	};

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// 次のシーンをセット (次のフレームから切り替わる)
	void SetNextScene(SceneType nextScene)
	{
		m_nextSceneType = nextScene;
	}
	
	// 現在のシーンのポーズフラグをセット
	// 記載方法次第で引数無しで宣言することは可能だが、分かりやすいように明記させるようにしておく
	void SetCurrentIsPause(const bool isPause);

	const SceneType GetSceneType()const { return m_currentSceneType; }

	// 現在のシーンのオブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList();

	// 現在のシーンにオブジェクトを追加
	void AddObject(const std::shared_ptr<KdGameObject>& obj);

	// UI用
	// 現在のシーンのUIリストを取得
	const std::list<std::shared_ptr<UI>>& GetUIList();

	// 現在のシーンのUIリストに追加
	void AddUI(const std::shared_ptr<UI>& UI);

	void DeleteUI();

	// 現在のシーンのポーズフラグを取得
	const bool GetIsPauseThisScene()const;

	void SetScore(int score);

private :

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行
	void Init()
	{
		// 開始シーンに切り替え
		ChangeScene(m_currentSceneType);

		// スコア初期化
		m_score = 0;

		KdEffekseerManager::GetInstance().Create(0, 0);
	}

	// シーン切り替え関数
	void ChangeScene(SceneType sceneType);

	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::Title;
	
	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = m_currentSceneType;

	int m_score = 0;

private:

	SceneManager() { Init(); }
	~SceneManager() {}

public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}
};
