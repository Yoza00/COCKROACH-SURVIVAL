#pragma once
#include "../CameraBase.h"

class Player;

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

	// プレイヤーのポインタセット
	void SetPlayer(const std::weak_ptr<Player>& spPlayer)
	{
		m_wpPlyer = spPlayer;
	}

private:

	bool	m_isCamRotUpdate = false;

	// プレイヤーのウィークポインタ
	std::weak_ptr<Player>	m_wpPlyer;

	// プレイヤーの死亡フラグをチェック
	bool CheckPlayer();
};