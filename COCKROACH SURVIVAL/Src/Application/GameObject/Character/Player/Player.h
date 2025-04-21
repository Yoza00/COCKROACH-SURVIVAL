#pragma once

class Notice;
class EatTips;
class TPSCamera;

class Player :public KdGameObject
{
public:

	// 歩行場所
	enum Position
	{
		Ground,			// 地面(通常平らな部分)
		Ceiling,		// 天井
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
	void SetNotice(const std::shared_ptr<Notice>& spNotice);

	void SetTips(const std::shared_ptr<EatTips>& spTips);

	// カメラのポインタをセットする
	void SetCamera(const std::shared_ptr<TPSCamera>& _spCamera)
	{
		m_wpCamera = _spCamera;
	}

	// プレイヤー座標をセットする
	void SetPos(const Math::Vector3	_pos)
	{
		m_pos = _pos;
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

	// 現在のスコアを取得
	const int GetScore()const
	{
		return m_score;
	}

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

	// 座標を取得
	Math::Vector3 GetPos()const override
	{ 
		return m_pos; 
	}

	// 生存フラグを返す
	const bool IsDead()const
	{
		return m_isDead; 
	}

	// 餌を食べられる距離の値を返す
	const float	GetCanEatDistance()const
	{
		return m_canEatDistance;
	}

private:	// 変数たちを宣言

	// ========================
	// 定数関連
	// ========================
	const Math::Vector3				m_startPlayerPos				= { 0.0f,10.0f,0.0f };	// 起動時に最初に配置する座標

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

	const float	m_moveSpeed						= 0.15f;	// 移動速度
	const float	m_scale							= 0.15f;	// 拡縮率

	const float	m_gravityPow					= 0.005f;	// 重力の強さを増やす際に使用する変数(毎フレームこれをm_gravityに加算することで、重力を強くする)
	const float	m_jumpPow						= 5.0f;		// ジャンプ力
	const float	m_canEatDistance				= 0.3f;		// 餌を食べられる距離

	const float	m_MaxHP_RestNumValue			= 100.0f;	// 体力と空腹度の最大値
	const float	m_notActionDecreaseRestValue	= 0.005f;	// 未アクション時の空腹度減少量
	const float	m_actionDecreaseRestValue		= 0.010f;	// アクション時の空腹度減少量
	const float	m_notActionDecreaseLifeValue	= 0.001f;	// 未アクション時のライフ減少量
	const float	m_actionDecreaseLifeValue		= 0.0025f;	// アクション時のライフ減少量

	const int	m_decreaceFoodLife				= 1;		// 餌のライフの減少量
	// ========================

	std::shared_ptr<KdModelWork>	m_spModel		= nullptr;		// モデルデータなどがまとまっているクラスの変数
	std::shared_ptr<KdAnimator>		m_spAnimator	= nullptr;		// モデルデータに含まれているアニメーションなどを管理するためのクラスの変数

	// ウィークポインタ
	std::weak_ptr<Notice>			m_wpNotice;				// ゲームの開始や終了などの画像
	std::weak_ptr<EatTips>			m_wpTips;				// 食べ方を記載した画像
	std::weak_ptr<TPSCamera>		m_wpCamera;				// カメラ
	std::weak_ptr<KdGameObject>		m_wpObject;				// 法線取得用レイと当たり判定を行ったオブジェクトのポインタをセットするためのウィークポインタ

	Math::Vector3 m_pos;									// 座標
	Math::Vector3 m_gravityDir	= Math::Vector3::Down;		// 重力の方向(3D空間上での重力のかかる方向を設定)
	Math::Vector3 m_direction	= Math::Vector3::Zero;		// 方向ベクトル
	Math::Vector3 m_normal		= Math::Vector3::Zero;		// 法線ベクトル

	int		m_score				= 0;			// スコア
												
	UINT	m_actionType;						// アクションタイプ
	UINT	m_nowMovingPos;						// その時の歩行場所
												
	float	m_adJustHeight		= 0.0f;			// 座標補正用変数(足元を0.0とするために必要な変数)
	float	m_enableStepHeight	= 0.005f;		// 自然に飛び越えられる高さの許容範囲
	float	m_gravity			= 0.0f;			// 重力の強さ
	float	m_life				= 100.0f;		// 体力
	float	m_restNum			= 100;			// 満腹度(満腹:100 空腹:0)
												
	bool	m_isMove				= true;		// 移動しても良いかどうか
	bool	m_isEat					= false;	// 食べられるかどうか
	bool	m_isPush				= false;	// キーフラグ(１回押し制限を設けるためのフラグ)
	bool	m_isDead				= false;	// 死亡フラグ
	bool	m_isHungry				= false;	// 空腹かどうか(満腹度が0以下になればtrueになり、そうでない場合はfalse)
	bool	m_touchWall				= false;	// 壁にいるかどうか
	bool	m_isJump				= false;	// ジャンプしたかどうか
	bool	m_isCeiling				= false;	// 天井かどうか？
	bool	m_isTitle				= false;	// タイトルかどうか	
	bool	m_isRestDecreaseStart	= false;	// 空腹度を減少させるかどうか
	bool	m_isBackWalk			= false;	// 後ろ歩きかどうか

private:	// 関数たちを宣言

	// アニメーションを変更する必要があるときに実行される関数
	void AnimationUpdate();

	// 進行方向を指定した角度だけ下向きに調整
	const Math::Vector3 AdJustDirection(const Math::Vector3& dir, float angleInDegrees);

	// 移動状態変更用関数
	void ChangeMovePosition(UINT num);
	/*
	引数の値をもとに移動状態フラグを変更する
	引数には、法線と6つの判定用ベクトルから内積を算出し
	一番判定用ベクトルに近かったもののベクトルの番号を返す
	1:Ground 2:Ceiling 3～6:Wall 7:Flying else:不具合
	*/

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
	void HitSphereCheck();		// 横との当たり判定を確認するためのスフィア
	void EatFoodSphereCheck();	// 食事用スフィア
};