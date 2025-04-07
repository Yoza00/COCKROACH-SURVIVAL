#pragma once

class ObjectData :public KdGameObject
{
public:

	ObjectData()						{}
	~ObjectData()			override	{}

	void Update()			override;
	void PostUpdate()		override;
	void DrawLit()			override;
	void Init()				override;

	// ==================================================
	// オブジェクトにデータを付与するためのセッター類
	// ==================================================
	void SetPos(const Math::Vector3 _pos)
	{
		m_pos = _pos;
	}

	void SetPath(const std::string& _fileOath)
	{
		m_filePath = _fileOath;
	}

	void SetName(const std::string& _name)
	{
		m_name = _name;
	}

	void SetRotationY(float rotationY)override
	{
		m_rotationY = rotationY;
	}

	void SetScale(float _scale = 0.25f)
	{
		m_scale = _scale;
	}

private:

	// 回転行列を作成する
	const Math::Matrix	GetRotateMatrix()const
	{
		return Math::Matrix::CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(m_rotationY),
			DirectX::XMConvertToRadians(0.0f),
			DirectX::XMConvertToRadians(0.0f)
		);
	}

	std::shared_ptr<KdModelWork>		m_spModel	= nullptr;
	Math::Vector3						m_pos		= Math::Vector3::Zero;
	std::string							m_filePath;
	std::string							m_name;

	float								m_scale		= 0.25f;
};