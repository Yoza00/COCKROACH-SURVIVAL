#pragma once

// UI用
class UI;

class BaseScene
{
public :

	BaseScene() { Init(); }
	virtual ~BaseScene() {}

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// オブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList()
	{
		return m_objList;
	}

	// オブジェクトリストに追加
	void AddObject(const std::shared_ptr<KdGameObject>& obj)
	{
		m_objList.push_back(obj);
	}

	// UI用
	// UIリスト：取得
	const std::list<std::shared_ptr<UI>>& GetUIList()
	{
		return m_UIList;
	}

	// UIリスト：追加
	void AddUIList(const std::shared_ptr<UI>& UI)
	{
		m_UIList.push_back(UI);
	}

	// UIリスト：削除
	void DeleteUI()
	{
		m_UIList.pop_back();
	}

	const bool GetIsPause()const
	{
		return m_isPause;
	}

	void SetPauseFlg(const bool isPause)
	{
		m_isPause	= isPause;
	}

protected :

	// 継承先シーンで必要ならオーバーライドする
	virtual void Event();
	virtual void Init();

	// ユニークポインタのカメラ
	std::unique_ptr<KdCamera>					m_uniCamera = nullptr;

	// 全オブジェクトのアドレスをリストで管理
	// ここにはポーズ中に処理しない物のみ格納
	std::list<std::shared_ptr<KdGameObject>>	m_objList;

	// ポーズ時に更新されるUI用
	std::list<std::shared_ptr<UI>>	m_UIList;

	// ポーズ状態かどうか
	bool	m_isPause		= false;
};
