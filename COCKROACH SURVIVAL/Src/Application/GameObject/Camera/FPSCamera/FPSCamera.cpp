#include "FPSCamera.h"

void FPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 基準点(ターゲット)の目線
	//m_mLocalPos = Math::Matrix::CreateTranslation(0, 1.5f, 0.0f);
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 0.1f, 0.0f);

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
}

void FPSCamera::Update()
{
	// ターゲットの行列(有効な場合利用する)
	Math::Matrix								_targetMat	= Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject>	_spTarget	= m_wpTarget.lock();	
	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}

	if (GetAsyncKeyState(VK_LSHIFT))
	{
		if (!m_isSight)
		{
			m_isSight = true;
		}
	}
	if (GetAsyncKeyState(VK_RSHIFT))
	{
		if (m_isSight)
		{
			m_isSight = false;
		}
	}

	// カメラの回転
	if (!m_isSight)
	{
		UpdateRotateByMouse();
	}
	m_mRotation = GetRotationMatrix();
	m_mWorld	= m_mRotation * m_mLocalPos * _targetMat;

	CameraBase::Update();
}
