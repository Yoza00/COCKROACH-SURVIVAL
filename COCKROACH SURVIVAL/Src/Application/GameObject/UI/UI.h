#pragma once

class UI :public KdGameObject
{
public:

	UI()									{}
	~UI()						override	{}

	virtual void DrawSprite()	override;
	virtual void Init()			override;

	// ==================== セッター関連 ====================

	// ファイルパスをセットする
	void SetFilePath(const std::string& _filePath)
	{
		m_filePath = _filePath;
	}

	// 描画する座標をセットする
	void SetDrawPos(Math::Vector2 _pos = Math::Vector2::Zero)
	{
		m_drawPos = _pos;
	}

	void SetIsMenu(bool isMenu)
	{
		m_isMenu = isMenu;
	}

	void SetIsButtonActive(bool isButtonActive)
	{
		m_isButtonActive = isButtonActive;
	}

	// ======================================================

	// ========== ゲッター関連 =============
	const bool GetIsMenu()const
	{
		return m_isMenu;
	}
	// =====================================

protected:

	std::shared_ptr<KdTexture>	m_spTex;									// 画像ポインタ

	Math::Vector2				m_drawPos			= Math::Vector2::Zero;	// 表示座標
	 Math::Vector2				m_startPos			= Math::Vector2::Zero;	// 切り取り開始座標
	float						m_width				= 0.0f;					// 画像幅
	float						m_height			= 0.0f;					// 画像高さ
	Math::Rectangle				m_rec;										// 切り取り範囲
	/*
	切り取り範囲は
	描画するだけのUI：初期化時に設定するだけ
	描画＋更新するUI：初期化時＋更新時に範囲を更新
	*/

	std::string					m_filePath;									// 使用する画像のファイルパス

	bool						m_isMenu			= false;				// メニュー画面かどうか
	bool						m_isButtonActive	= true;					// ボタンは有効かどうか
};