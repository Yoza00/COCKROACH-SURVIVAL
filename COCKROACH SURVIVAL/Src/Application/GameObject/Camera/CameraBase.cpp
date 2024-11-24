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
		//return GetRotationXZMatrix();
		return GetRotationMatrix();
		break;
	case 2:			// 壁
	case 3:
	case 4:
	case 5:
		return GetRotationXYMatrix();
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

	//// 実際にカメラを回転させる処理(0.15はただの補正値)
	//m_DegAng.x += _mouseMove.y * 0.15f;
	//m_DegAng.y += _mouseMove.x * 0.15f;

	// 回転制御
	//m_DegAng.x = std::clamp(m_DegAng.x, -45.f, 45.f);
	//m_DegAng.x = std::clamp(m_DegAng.x, -90.f, 90.f);

	// カメラの回転角度制御
	// プレイヤーの現在の移動状況をもとにカメラの回転角度(移動方向等)を決定する
	switch (m_playerNowMovingPos)
	{
	case 0:
	case 6:	// 地面か空中にいる場合

		// 実際にカメラを回転させる処理(0.15はただの補正値)
		m_DegAng.x += _mouseMove.y * 0.15f;
		m_DegAng.y += _mouseMove.x * 0.15f;

		// 横回転に対する制限はないが、縦回転に対して、-90<=x<=90の範囲で回転するように補正
		m_DegAng.x = std::clamp(m_DegAng.x, -90.0f, 0.0f);	
		//m_DegAng.x = std::clamp(m_DegAng.x, -45.0f, 30.0f);
		m_DegAng.z = 0.0f;
		break;
	case 1:	// 天井にいる場合
		// 実際にカメラを回転させる処理(0.15はただの補正値)
		m_DegAng.x -= _mouseMove.y * 0.15f;
		m_DegAng.y -= _mouseMove.x * 0.15f;

		m_DegAng.x = std::clamp(m_DegAng.x, -30.0f, 60.0f);
		m_DegAng.z = 180.0f;
		break;
	case 2:
	case 3:
	case 4:
	case 5:	// 壁(オブジェクトの側面)にいる場合
		// 実際にカメラを回転させる処理(0.15はただの補正値)
		m_DegAng.x += _mouseMove.y * 0.15f;
		m_DegAng.y += _mouseMove.x * 0.15f;

		m_DegAng.x = std::clamp(m_DegAng.x, -60.0f, 60.0f);
		//m_DegAng.x = std::clamp(m_DegAng.x, -60.0f, 60.0f);
		m_DegAng.z = 0.0f;
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

	//Application::Instance().m_log.AddLog("%0.2f\n", m_DegAng.y);
}