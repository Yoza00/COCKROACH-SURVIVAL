#pragma once

class CameraBase : public KdGameObject
{
public:
	CameraBase()						{}
	virtual ~CameraBase()	override	{}

	void Init()				override;
	void Update()			override;
	void PreDraw()			override;

	void SetTarget(const std::shared_ptr<KdGameObject>& target);

	// 作成してある当たり判定に使用するリストに格納するための関数
	void ResistHitObject(const std::shared_ptr<KdGameObject>& _object)
	{
		m_wpHitObjectList.push_back(_object);
	}

	void SetMovingPos(int num);

	// 「絶対変更しません！見るだけ！」な書き方
	const std::shared_ptr<KdCamera>& GetCamera() const
	{
		return m_spCamera;
	}

	// 「中身弄るかもね」な書き方
	std::shared_ptr<KdCamera> WorkCamera() const
	{
		return m_spCamera;
	}

	const Math::Matrix GetRotationMatrix()const
	{
		return Math::Matrix::CreateFromYawPitchRoll(
		       DirectX::XMConvertToRadians(m_DegAng.y),
		       DirectX::XMConvertToRadians(m_DegAng.x),
		       DirectX::XMConvertToRadians(m_DegAng.z));
	}

	const Math::Matrix GetRotationXMatrix()const
	{
		return Math::Matrix::CreateRotationX(
			DirectX::XMConvertToRadians(m_DegAng.x));
	}

	const Math::Matrix GetRotationYMatrix() const
	{
		return Math::Matrix::CreateRotationY(
			   DirectX::XMConvertToRadians(m_DegAng.y));
	}

	const Math::Matrix GetRotationZMatrix()const
	{
		return Math::Matrix::CreateRotationZ(
			DirectX::XMConvertToRadians(m_DegAng.z));
	}

	// カメラの回転角度取得用関数
	// 引数で呼び出す関数を指定し、呼び出した関数の結果を取得し、この関数がその値を返す
	// 呼び出す関数を決めるための関数
	// 0,6:Y軸		地面(オブジェクトの上の面を移動中) 
	// 2,3,4,5:XとY軸		壁(オブジェクトの側面を移動中)
	// 1:XとZ軸		天井(オブジェクトの下の面を移動中)
	const Math::Matrix GetRotationCameraMatrix(int num)const;

	// カメラのX軸とY軸の回転角度取得用関数
	const Math::Matrix GetRotationXYMatrix()const;

	// カメラのX軸とZ軸の回転角度取得用関数
	const Math::Matrix GetRotationXZMatrix()const;

	// 回転角度を取得
	const Math::Vector3& GetDeg()const { return m_DegAng; }

private:
	// カメラ回転用角度
	Math::Vector3				m_DegAng		= Math::Vector3::Zero;
	const float					m_mouseCorrectValue = 0.15f;		// マウスの補正値(移動量の補正値)

protected:
	void UpdateRotateByMouse();

	std::shared_ptr<KdCamera>	m_spCamera		= nullptr;
	std::weak_ptr<KdGameObject>	m_wpTarget;

	std::vector<std::weak_ptr<KdGameObject>>	m_wpHitObjectList{};			// 当たり判定をするオブジェクトを格納するリスト

	Math::Matrix				m_mLocalPos		= Math::Matrix::Identity;
	Math::Matrix				m_mRotation		= Math::Matrix::Identity;

	// カメラ回転用マウス座標の差分
	POINT						m_FixMousePos{};

	int							m_playerNowMovingPos = 0;	// プレイヤーの移動状態

	int							m_oldMovingPos	= 0;		// 直前に移動していた場所
	bool						m_isInversion	= false;	// 反転させるかどうか

	bool						m_isRotateY = false;							// Y軸方向に回転させるかどうか(天井移動時のフラグ)

	bool						m_isSetValue = false;		// 値をセットしたかどうか(固定値をセットしたかどうか判別用のフラグ)
	bool						m_isMoveCeiling = false;
};