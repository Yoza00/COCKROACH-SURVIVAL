#pragma once

class UI :public KdGameObject
{
public:

	UI()									{}
	~UI()						override	{}

	virtual void Update()		override	{}
	virtual void DrawSprite()	override;
	virtual void Init()			override;

	// ==================== セッター関連 ====================

	// ファイルパスをセットする
	virtual void SetFilePath(const std::string& _filePath)
	{
		m_filePath = _filePath;
	}

	// 描画する座標をセットする
	virtual void SetDrawPos(Math::Vector2 _pos = Math::Vector2::Zero)
	{
		m_drawPos = _pos;
	}

	// 描画フラグをセットするためのセッター
	virtual void SetIsDraw(bool isDraw)
	{
		m_isDraw = isDraw;
	}

protected:

	std::shared_ptr<KdTexture>	m_spTex;							// 画像ポインタ

	Math::Vector2				m_drawPos	= Math::Vector2::Zero;	// 表示座標
	 Math::Vector2				m_startPos	= Math::Vector2::Zero;	// 切り取り開始座標
	float						m_width		= 0.0f;					// 画像幅
	float						m_height	= 0.0f;					// 画像高さ

	std::string					m_filePath;

	bool						m_isDraw	= false;				// 描画するかどうか
};