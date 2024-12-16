#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void Init()				override;
	void Update()			override;

	// カメラの回転処理を行うかどうかのフラグのセッター
	void SetIsCamRotUpdate(bool isCamRotUpdate)
	{
		m_isCamRotUpdate = isCamRotUpdate;
	}

private:

	bool	m_isCamRotUpdate = false;
};