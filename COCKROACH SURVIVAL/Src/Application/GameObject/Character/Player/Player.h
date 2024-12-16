#pragma once

class TPSCamera;
//class FPSCamera;

//class TimeLimit;

class Player :public KdGameObject
{
public:

	// 歩行場所
	enum Position
	{
		Ground,			// 地面(通常平らな部分)
		Ceiling,		// 天井
		//Wall,			// 壁
		Wall_Right,		// 側面右
		Wall_Left,		// 側面左
		Wall_Forward,	// 側面奥
		Wall_Backward,	// 側面手前
		Flying			// 飛行(非着地状態)
	};

	// アクションタイプ
	enum ActionType
	{
		None,				// 特に使用しない
		Idle,				// 待機中
		Run,				// 移動中
		Jump,				// 跳躍
		Fly,				// 滑空中
		Max					// 要素の数を確認する用
	};

	Player();
	~Player()						override	{}

	void Update()					override;
	void PostUpdate()				override;
	void GenerateDepthMapFromLight()override;
	void DrawLit()					override;
	void Init()						override;

	// セッター関連
	// カメラのポインタをセットする
	void SetCamera(const std::shared_ptr<TPSCamera>& _spCamera)
	{
		m_wpCamera = _spCamera;
	}
	/*void SetCamera(const std::shared_ptr<FPSCamera>& _spCamera)
	{
		m_wpCamera = _spCamera;
	}*/

	/*void SetLimit(const std::shared_ptr<TimeLimit>& _spLimit)
	{
		m_wpLimit = _spLimit;
	}*/

	// プレイヤー座標をセットする
	void SetPos(const Math::Vector3	_pos)
	{
		m_pos = _pos;
	}

	// プレイヤーの回転角度を渡す
	void SetModelRotate(const Math::Vector3& modelRotate)
	{
		m_modelRotate = modelRotate;
	}

	void SetModelHeight(const float	_height)
	{
		m_adJustHeight = _height;
	}

	// タイトルフラグをセットするためのセッター
	void SetIsTitle(bool isTitle)
	{
		m_isTitle = isTitle;
	}

	// 食事が完了した結果をセットする
	void SetAteFood(float life, float restNum, int score);

	// 満腹度を取得
	const float GetRestNum() const
	{
		return m_restNum;
	}

	// HP残量を取得
	const float GetHP()const
	{
		return m_life;
	}

	Math::Vector3 GetPos()const override { return m_pos; }

	// 生存フラグを返す
	const bool IsDead()const { return m_isDead; }

private:

	// アニメーションを変更する必要があるときに実行される関数
	void AnimationUpdate();

	// モデル
	std::shared_ptr<KdModelWork>	m_spModel		= nullptr;

	std::shared_ptr<KdAnimator>		m_spAnimator	= nullptr;

	// ==============================
	// プレイヤー 関連
	// ==============================

	// 座標
	//Math::Vector3					m_pos			= Math::Vector3::Zero;
	Math::Vector3					m_pos = { 0.0f,10.0f,0.0f };

	float							m_adJustHeight	= 0.0f;				// 座標補正用
	// ↑モデルの原点がモデルのおなかの位置になっているので、それを補正するための変数

	float							m_enableStepHeight = 0.005f;			// 自然に飛び越えられる高さの許容範囲

	//const float						m_moveSpeed		= 0.05f;
	//const float						m_moveSpeed		= 0.1f;				// 普通の速度
	//const float						m_moveSpeed		= 0.35f;
	//const float						m_moveSpeed		= 0.005f;		// テスト用
	const float						m_moveSpeed		= 0.15f;

	const float						m_scale			= 0.15f;			// プレイヤーのサイズ

	//bool							m_isMove		= false;			// 移動状態かどうか

	//Math::Matrix					m_rotMat = Math::Matrix::Identity;	// 回転行列

	// ================== 重力関連 ========================================
	float							m_gravity		= 0.0f;					// 重力の強さ
	Math::Vector3					m_gravityDir	= Math::Vector3::Down;	// 重力の方向(3D空間上での重力のかかる方向を設定)
	const float						m_gravityPow	= 0.005f;				// 重力の強さを増やす際に使用する変数(毎フレームこれをm_gravityに加算することで、重力を強くする)
	// ====================================================================

	UINT							m_actionType;						// アクションタイプ

	Math::Matrix					m_modelRotMat	= Math::Matrix::Identity;
	// ↑モデルの向き補正用行列

	// ごり押し用
	Math::Vector3					m_moveDir	= Math::Vector3::Zero;
	bool							m_isMove	= true;					// 移動しても良いかどうか

	bool							m_isEat		= false;		// 食べられるかどうか

	// キーフラグ(１回押し制限)
	bool							m_isPush	= false;
	
	// プレイヤーの体力
	float							m_life		= 100.0f;
	bool							m_isDead	= false;		// 死亡フラグ

	// カメラのウィークポインタ
	std::weak_ptr<TPSCamera>		m_wpCamera;
	//std::weak_ptr<FPSCamera>		m_wpCamera;

	float							m_restNum	= 100;		// 満腹度(満腹:100 空腹:0)
	bool							m_isHungry	= false;	// 空腹かどうか(満腹度が0以下になればtrueになり、そうでない場合はfalse)

	Math::Vector3					m_direction = Math::Vector3::Zero;

	Math::Vector3					m_normal	= Math::Vector3::Zero;		// 法線ベクトル

	// 歩行場所
	//Position						m_nowMovingPos = Position::Ground;
	UINT							m_nowMovingPos;

	const float						m_jumpPow = 5.0f;		// ジャンプ力

	// 移動状態フラグを切り替えるために使用する方向ベクトルを格納している可変長配列
	const std::vector<Math::Vector3>		m_vectorDir =
	{
		{Math::Vector3::Up		},			// 地面判定用
		{Math::Vector3::Down	},			// 天井判定用
		{Math::Vector3::Right	},			// 側面右判定用
		{Math::Vector3::Left	},			// 側面左判定用
		{Math::Vector3::Forward	},			// 側面奥判定用
		{Math::Vector3::Backward}			// 側面手前判定用
	};

	int								m_score = 0;				// スコアを格納するための変数
	// ↑スコアをここに足しこんでいき、リザルトシーンに渡す

	//std::weak_ptr<TimeLimit>	m_wpLimit;

	bool						m_isMenu = false;

	// ========= 確認用 ===========
	Math::Vector3	m_modelRotate = Math::Vector3::Zero;		// 3D座標における回転角度
	bool			m_touchWall = false;

	std::weak_ptr<KdGameObject>				 m_wpObject;		// 法線取得用レイと当たり判定を行ったオブジェクトのポインタをセットするためのウィークポインタ

	bool m_isJump = false;					// ジャンプしたかどうか

	bool m_isCeiling = false;				// 天井かどうか？

	bool m_isTitle = false;					// タイトルかどうか				

	// 進行方向を指定した角度だけ下向きに調整
	const Math::Vector3 AdJustDirection(const Math::Vector3& dir, float angleInDegrees);

	// 移動状態変更用関数
	// 引数の値をもとに移動状態フラグを変更する
	// 引数には、法線と6つの判定用ベクトルから内積を算出し
	// 一番判定用ベクトルに近かったもののベクトルの番号を返す
	// 1:Ground 2:Ceiling 3～6:Wall 7:Flying else:不具合
	void ChangeMovePosition(int num);

	// 移動状態の切替が必要か判定するための関数
	void CheckMovePosition(const Math::Vector3& normal, float rotY);

	// 進行方向に対してやや下向きにレイを発射するための関数
	const Math::Vector3 GetAdJustedRayDirection(const Math::Vector3& forward, const Math::Vector3& gravityDir, float downwardBias);
	/*
	forward		:前方方向
	gravityDir	:重力方向
	downwardBias:傾ける度合
	*/

	// 当たり判定系
	void HitJudge();
	void NormalCheck();			// 法線取得用レイ判定
	void GroundCheck();			// 重力方向へのレイ(足場確認用)
	bool FutureGroundCheck();	// 未来座標を利用して、オブジェクトの側面に張り付いたりする
	void HitSphereCheck();		// 横との当たり判定を確認するためのスフィア
	void EatFoodSphereCheck();	// 食事用スフィア
};