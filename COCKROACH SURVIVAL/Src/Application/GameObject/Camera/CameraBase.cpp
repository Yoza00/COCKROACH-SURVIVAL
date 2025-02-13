#include "CameraBase.h"
#include"../../main.h"

void CameraBase::Init()
{
	if (!m_spCamera)
	{
		m_spCamera = std::make_shared<KdCamera>();
	}
	// ↓画面中央座標
	m_FixMousePos.x = 640;
	m_FixMousePos.y = 360;

	m_DegAng.y = 90.0f;
}

void CameraBase::Update()
{
	if (!m_spCamera) { return; }

	m_spCamera->SetCameraMatrix(m_mWorld);
}

void CameraBase::PreDraw()
{
	if (!m_spCamera) { return; }

	m_spCamera->SetToShader();
}

void CameraBase::SetTarget(const std::shared_ptr<KdGameObject>& target)
{
	if (!target) { return; }

	m_wpTarget = target;
}

void CameraBase::SetMovingPos(int num)
{
	if (m_playerNowMovingPos == num)return;

	m_playerNowMovingPos = num;

	// 天井移動であるならばY軸回転用フラグを起動
	if (num == 1)
	{
		m_isRotateY = true;
	}

	if (m_isSetValue)
	{
		m_isSetValue = false;
	}
}

const Math::Matrix CameraBase::GetRotationCameraMatrix(int num) const
{
	switch (num)
	{
	case 0:			// 地面か飛行状態
	case 6:
		return GetRotationYMatrix();
		break;
	case 1:			// 天井
		return GetRotationMatrix();
		break;
	case 2:			// 壁
	case 3:
	case 4:
	case 5:
		return GetRotationMatrix();
		break;
	default:
		return Math::Matrix::Identity;
	}
}

const Math::Matrix CameraBase::GetRotationXYMatrix() const
{
	Math::Matrix	_rotX = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_DegAng.x));
	Math::Matrix	_rotY = GetRotationYMatrix();
	Math::Matrix	_rotMat = _rotX * _rotY;

	return _rotMat;
}

const Math::Matrix CameraBase::GetRotationXZMatrix() const
{
	Math::Matrix	_rotX = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_DegAng.x));
	Math::Matrix	_rotZ = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_DegAng.z));
	Math::Matrix	_rotMat = _rotX * _rotZ;

	return _rotMat;
}

void CameraBase::UpdateRotateByMouse()
{
	// マウスでカメラを回転させる処理
	POINT _nowPos;
	GetCursorPos(&_nowPos);

	POINT _mouseMove{};
	_mouseMove.x = _nowPos.x - m_FixMousePos.x;
	_mouseMove.y = _nowPos.y - m_FixMousePos.y;

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);

	// カメラの回転角度制御
	// プレイヤーの現在の移動状況をもとにカメラの回転角度(移動方向等)を決定する
	switch (m_playerNowMovingPos)
	{
	case 0:
	case 6:	// 地面か空中にいる場合

		m_DegAng.x += _mouseMove.y * m_mouseCorrectValue;
		m_DegAng.y += _mouseMove.x * m_mouseCorrectValue;

		m_DegAng.x = std::clamp(m_DegAng.x, -90.0f, 0.0f);
		m_DegAng.z = 0.0f;
		break;
	case 1:	// 天井にいる場合
		m_DegAng.x -= _mouseMove.y * m_mouseCorrectValue;
		m_DegAng.y -= _mouseMove.x * m_mouseCorrectValue;

		m_DegAng.x = std::clamp(m_DegAng.x, -30.0f, 60.0f);
		m_DegAng.z = 180.0f;
		break;
	case 2:
		// 一度だけ実行する処理
		if (!m_isSetValue)
		{
			m_isSetValue = true;
			m_DegAng = { -90.0f,90.0f,0.0f };
		}

		// 回転量更新
		{
			m_DegAng.x += _mouseMove.y * m_mouseCorrectValue;
			m_DegAng.y += _mouseMove.x * m_mouseCorrectValue;
		}

		// 回転量の値制御
		{
			m_DegAng.x = std::clamp(m_DegAng.x, -120.0f, -70.0f);
			m_DegAng.y = std::clamp(m_DegAng.y, 80.0f, 100.0f);
		}
		break;
	case 3:
		// 一度だけ実行する処理
		if (!m_isSetValue)
		{
			m_isSetValue = true;
			m_DegAng = { -90.0f,-90.0f,0.0f };
		}

		{
			m_DegAng.x += _mouseMove.y * m_mouseCorrectValue;
			m_DegAng.y += _mouseMove.x * m_mouseCorrectValue;
		}

		{
			m_DegAng.x = std::clamp(m_DegAng.x, -120.0f, -70.0f);
			m_DegAng.y = std::clamp(m_DegAng.y, -100.0f, -80.0f);
		}
		break;
	case 4:
		// 一度だけ実行する処理
		if (!m_isSetValue)
		{
			m_isSetValue = true;
			m_DegAng = { -90.0f,180.0f,0.0f };
		}

		{
			m_DegAng.x += _mouseMove.y * m_mouseCorrectValue;
			m_DegAng.y += _mouseMove.x * m_mouseCorrectValue;
		}

		{
			m_DegAng.x = std::clamp(m_DegAng.x, -120.0f, -70.0f);
			m_DegAng.y = std::clamp(m_DegAng.y, 170.0f, 190.0f);
		}
		break;
	case 5:	// 壁(オブジェクトの側面)にいる場合
		// 一度だけ実行する処理
		if (!m_isSetValue)
		{
			m_isSetValue = true;
			m_DegAng = { -90.0f,0.0f,0.0f };
		}

		{
			m_DegAng.x += _mouseMove.y * m_mouseCorrectValue;
			m_DegAng.y += _mouseMove.x * m_mouseCorrectValue;
		}

		{
			m_DegAng.x = std::clamp(m_DegAng.x, -120.0f, -70.0f);
			m_DegAng.y = std::clamp(m_DegAng.y, -10.0f, 10.0f);
		}
		break;
	}

	// 天井に張り付いた瞬間、カメラの向きを反対方向に回転
	if (m_isRotateY)
	{
		m_DegAng.y -= 180.0f;

		m_isRotateY = false;
	}

	// Y軸の回転制御
	// Y回転が -360.0f < y < 360.0f の範囲になるように制御
	if (m_DegAng.y > 360.0f)
	{
		m_DegAng.y -= 360.0f;
	}
	else if (m_DegAng.y < -360.0f)
	{
		m_DegAng.y += 360.0f;
	}
}