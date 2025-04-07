#pragma once

class Enemy;

class Weapon :public KdGameObject
{
public:

	Weapon()									{}
	~Weapon()						override	{}

	void Update()					override;
	void DrawLit()					override;
	void GenerateDepthMapFromLight()override;
	void Init()						override;

	void SetParent(const std::shared_ptr<Enemy>& parernt)
	{
		m_wpParent = parernt;
	}

	void SetIsModelDraw(bool isModelDraw)
	{
		m_isModelDraw = isModelDraw;
	}

	const Math::Matrix& GetNozzleMat()const { return m_nozzleMat; }

	std::shared_ptr<KdModelWork>	m_spModel	= nullptr;
	Math::Vector3					m_pos		= Math::Vector3::Zero;

	bool							m_isModelDraw	= false;	// モデルの描画フラグ

	Math::Matrix					m_nozzleMat = Math::Matrix::Identity;

	// 必要なウィークポインタ
	std::weak_ptr<Enemy>			m_wpParent;
};