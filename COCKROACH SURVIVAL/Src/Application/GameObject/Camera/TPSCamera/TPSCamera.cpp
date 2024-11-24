#include "TPSCamera.h"

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	//m_mLocalPos = Math::Matrix::CreateTranslation(0, 1.5f, -10.0f);
	//m_mLocalPos = Math::Matrix::CreateTranslation(0, 1.5f, -3.0f);
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 0.25f, -0.5f);

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
}

void TPSCamera::Update()
{
	// ターゲットの行列(有効な場合利用する)
	Math::Matrix								_targetMat	= Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject>	_spTarget	= m_wpTarget.lock();
	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}

	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
	{
		m_isSight = true;
		ShowCursor(true);
	}
	else if (GetAsyncKeyState(VK_RSHIFT) & 0x8000)
	{
		m_isSight = false;
		ShowCursor(false);
	}

	// カメラの回転
	if (!m_isSight)
	{
		UpdateRotateByMouse();
	}
	m_mRotation = GetRotationMatrix();
	m_mWorld	= m_mLocalPos * m_mRotation * _targetMat;

	KdCollider::RayInfo		_rayInfo;
	_rayInfo.m_pos = GetPos();
	_rayInfo.m_dir = Math::Vector3::Down;
	_rayInfo.m_range = 1000.0f;
	_rayInfo.m_type = KdCollider::TypeGround;

	// カメラに収めるターゲットが存在している場合だけ実行される処理
	if (_spTarget)
	{
		Math::Vector3	_targetPos = _spTarget->GetPos();		// ターゲット(今回はキャラクター)の座標
		_targetPos.y += 0.1f;
		_rayInfo.m_dir = _targetPos - GetPos();
		_rayInfo.m_range = _rayInfo.m_dir.Length();
		_rayInfo.m_dir.Normalize();
	}

	for (std::weak_ptr<KdGameObject> _wpGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject>	_spGameObj = _wpGameObj.lock();
		if (_spGameObj)
		{
			std::list < KdCollider::CollisionResult>	_retRayList;
			_spGameObj->Intersects(_rayInfo, &_retRayList);

			// ③結果を使って座標を補完する
			// レイに当たったリストから一番近いオブジェクトを検出
			float			_maxOverLap = 0.0f;
			Math::Vector3	_hitPos = {};
			bool			_hit = false;

			for (auto& _ret : _retRayList)
			{
				// レイを遮断し、オーバーした長さが
				// 一番長いものを探す
				if (_maxOverLap < _ret.m_overlapDistance)
				{
					_maxOverLap = _ret.m_overlapDistance;
					_hitPos = _ret.m_hitPos;
					_hit = true;
				}
			}

			if (_hit)
			{
				// 何かしら障害物に当たっている
				Math::Vector3	_hoseiPos = _hitPos;
				//	* 0.4fは補正値
				_hoseiPos += _rayInfo.m_dir * 0.4f;
				SetPos(_hoseiPos);
			}
		}
	}

	CameraBase::Update();
}