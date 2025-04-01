#pragma once

#include"Json/nlohmann/json.hpp"

class Player;
class Weapon;

// ウェイポイント用構造体
struct WayPoint
{
	int				m_number	= 0;					// ポイントの番号
	Math::Vector3	m_pos		= Math::Vector3::Zero;	// ポイントの座標
};

// ========== 経路探索用 ==========
	// グリッド情報を定義
struct Node
{
	int		x, z;						// ノードの座標
	float	gCost = 0;					// スタートからこのノードまでのコスト
	float	hCost = 0;					// このノードからゴールまでの推定コスト
	Node* parent = nullptr;				// 親ノード

	// 総コスト計算
	float fCost()const
	{
		return gCost + hCost;
	}

	bool operator==(const Node& other)const
	{
		return x == other.x && z == other.z;
	}
};

// ノードをハッシュ化するための関数
struct NodeHash
{
	size_t operator()(const Node& node) const
	{
		return std::hash<int>()(node.x) ^ std::hash<int>()(node.z);
	}
};
// ================================

class Enemy :public KdGameObject
{
public:

	Enemy(std::vector<std::vector<int>>& grid);
	~Enemy()								override	{}

	// 周囲の捜索に必要なやつ
	enum SearchPhase
	{
		RotRight,				// 右回転
		PauseRight,				// 右回転終了、停止
		RotLeft,				// 左回転
		PauseLeft				// 左回転終了、停止
	};

	// 更新
	void Update()							override;
	void PostUpdate()						override;

	// 描画
	void DrawLit()							override;
	void GenerateDepthMapFromLight()		override;

	// 初期化
	void Init()override;

	// ========== セッター ==========
	void SetPlayer(const std::shared_ptr<Player>& spPlayer)
	{
		m_wpPlayer = spPlayer;
	}

	void SetWeapon(const std::shared_ptr<Weapon>& spWeapon)
	{
		m_wpWeapon = spWeapon;
	}

	void SetScale(float scale)
	{
		m_scale = scale;
	}
	// ==============================

	// ========== 経路探索用 ==========
	// ゴールの設定と経路探索
	void SetGoal(const Math::Vector3& goalPos);
	// ================================

	const Math::Vector3& GetRightHandPos()const { return m_rightHandPos; }

private:

	const int						m_int_resetValue		= 0;						// int型の初期化の値
	const Math::Vector3				m_correctionPosValue	= { 0.0f, 0.05f, 0.0f };	// 座標補正値

	// =========== エフェクト関連 ==========
	const int						m_sprayAnimationFinishCnt	= 10;
	int								m_currentSprayAnimationCnt	= m_int_resetValue;
	// =====================================

	std::shared_ptr<KdModelWork>	m_spModel		= nullptr;					// モデル

	std::shared_ptr<KdAnimator>		m_spAnimator	= nullptr;					// アニメーター

	std::weak_ptr<Player>			m_wpPlayer;									// Playerクラスのウィークポインタ
	std::weak_ptr<Weapon>			m_wpWeapon;

	Math::Vector3					m_pos			= Math::Vector3::Zero;		// 座標
	const float						m_moveSpeed		= 0.1f;						// 移動速度
	float							m_scale			= 1.0f;						// 拡縮サイズ

	float							m_gravity		= 0.0f;						// 重力
	const float						m_gravityPow	= 0.01f;					// 更新時に加算される重力の強さ

	Math::Vector3					m_loseSightPos	= Math::Vector3::Zero;		// 見失った場所

	const float						m_minDegAngle	= -360.0f;					// 回転角度の最小値
	const float						m_maxDegAngle	= 360.0f;					// 回転角度の最大値

	const float						m_attackDistance = 5.0f;

	Math::Matrix					m_attackPoint_HighMat	= Math::Matrix::Identity;	// 高所判定のポイントの行列
	Math::Matrix					m_attackPoint_LowMat	= Math::Matrix::Identity;	// 低所判定のポイントの行列
	Math::Vector3					m_attackPoint_highPos	= Math::Vector3::Zero;		// 高所判定のポイントの座標(これ以上の高さが高所判定)
	Math::Vector3					m_attackPoint_LowPos	= Math::Vector3::Zero;		// 低所判定のポイントの座標(これ以下の高さが低所判定)

	// ========== 座標補正用変数 ==========
	float							m_adJustHeight	= -0.1f;						// モデルの高さ補正用変数
	// ====================================
	
	// ========== 視界関係で必要となる変数 ==========
	const float						m_sightSize		= 80.0f;					// 中心からの角度
	const float						m_sightAreaSize = 100.0f;					// ぼんやりと見えている範囲
	const float						m_sightRange	= 2000.0f;					// 判定の距離
	float							m_angle			= 0.0f;						// モデルの回転角度
	const float						m_maxAngle		= 1.0f;						// 回転角度の最大値
	const float						m_rotThreshold	= 3.5f;						// 回転する閾値(回転度数がここよりも小さい場合は回転処理をしない)
	const float						m_rotSpdCorrect = 2.0f;						// << モデルの回転時に使用する >> 回転させる速度を補正するための変数
	bool							m_isSight		= false;					// 視界内かどうか
	Math::Vector3					m_sightPos		= Math::Vector3::Zero;		// 視界の開始座標
	Math::Matrix					m_localSightMat;							// 視界の根本の行列

	int								m_searchCnt		= 0;						// Searchステートにいる間に使用するフレーム
	const int						m_seaMaxCnt		= 180;						// Searchステートにいる最大時間
	// ==============================================

	// ========== 周囲の捜索に必要な変数 ==========
	bool							m_isSearchFin	= false;					// 周囲の捜索が終了したかどうか
	bool							m_isNowRotFin	= false;					// 今、回転終了しているかどうか(途中であれば、回転完了まで待つ)
	SearchPhase						m_searchPhase	= SearchPhase::RotRight;	// 今の状態
	float							m_baseAngle		= 0.0f;						// 基準角度(ステートに入ったとき、一度だけ現在の角度を代入する)
	int								m_frameCnt		= 0;						// フレームカウンタ
	const float						m_rotationSpd	= 1.0f;						// 回転速度
	const float						m_maxDegAng		= 60.0f;					// 最大回転角度
	int								m_searchTimer	= 600;						// 捜索する制限時間(無くなれば次のステートへ)
	const int						m_resetTimer	= 600;						// タイマーのリセット値
	// ============================================

	// ========== 徘徊処理に必要な変数 ==========
	std::vector<WayPoint>			m_wayPoints;								// ウェイポイントの配列
	int								m_wayNumber		= 0;						// ウェイトポイントの番号
	const int						m_minWayNumber	= 0;						// ウェイポイントの最小番号
	const int						m_maxWayPoint	= 9;						// ウェイポイントの最大番号
	bool							m_isMoveNextPos = false;					// 次の場所へ移動するかどうか
	bool							m_isTurnFinish	= false;					// 回転処理が終わったかどうか(回転終わるまで移動しない)
	Math::Vector3					m_nextPos		= Math::Vector3::Zero;		// 次の場所の座標
	const float						m_ignoreLength	= 2.5f;						// 無視する距離
	bool							m_isFixNextPos	= false;					// 次の場所を決定すべきかどうか(見失ってから、巡回ルートに戻る際に使用する)
	// ==========================================

	// ========== 攻撃に必要な変数 ==========
	Math::Matrix					m_rightHandMat;								// 右手の行列
	Math::Vector3					m_rightHandPos = Math::Vector3::Zero;
	// ======================================

	// ========== 経路探索用 ==========
	bool							m_isActFindPath		= false;				// 経路の再計算を行うかどうか
	int								m_findPathCnt		= 0;					// 経路の再計算までのカウンタ(0でフラグが起動する)
	const int						Reset_findPathCnt	= 60;					// 経路の再計算を行う際に、カウンタをリセットする際に使用するリセット値

	std::vector<Node>				m_path;											// 計算された経路
	size_t							m_currentPathIndex	= 0;						// 現在の経路インデックス
	std::vector<std::vector<int>>*	m_grid;											// 参照するグリッド
	const Math::Vector3				m_mapOrigin			= { -62.0f,0.04f,79.0f };	// グリッドの原点補正用
	
	// ヒューリスティック関数
	float Heuristic(const Node& a, const Node& b)
	{
		return	std::abs(static_cast<float>(a.x) - static_cast<float>(b.x)) +
				std::abs(static_cast<float>(a.z) - static_cast<float>(b.z));
	}

	// 経路探索(A*)
	void FindPath(const Math::Vector3& start, const Math::Vector3& goal);

	// Vector3をグリッド座標に変換
	Node WorldToGrid(const Math::Vector3& WorldPos);

	// グリッド座標をVector3に変換
	Math::Vector3 GridToWorld(const Node& node);

	// 見失ったところが障害物であった際に使用する
	// 移動可能範囲内でできる限り近づくことのできる経路を探索するための関数
	Node FindNearestWalkableNode(const Node& targetNode);
	// ================================

	// 当たり判定を行う関数
	void HitCheck();

	// 視界内にプレイヤーがいるかの確認
	void CheckSight();

	// ノード情報を取得
	void SetValueFromNodeInfo();

	// モデルを進行方向に向ける関数
	void RotateToDirection(const Math::Vector3& toDir);

	// ========== 自クラスで使用するゲッター ==========
	bool GetDotResult(const Math::Vector3& toDir);								// 目線のレイ判定の結果を返す

	// Jsonファイルを使用するうえで必要となる関数
	bool LoadWayPointsFronJson(const std::string& _filePath);					// Jsonファイルが開けたかどうかの確認用関数
	// ================================================

	// アニメーション切り替え用
	// 第2引数にはアニメーションを繰り返すかどうかを入れる。第２引数なしの場合はループあり
	void ChangeAnimation(const std::string& animationName, bool isLoop = true);

	// ステートパターン
private:

	// ステートパターンのベースクラス
	class StateBase
	{
	public:

		virtual ~StateBase()				{}

		virtual void Enter(Enemy&)	{}	// その状態に入るときに１度だけ実行する処理
		virtual void Update(Enemy&)	{}	// その状態の間、行う処理
		virtual void Exit(Enemy&)	{}	// その状態から別の状態に変更するときに１度だけ実行する処理

		virtual void CheckMoveFinish(Enemy& , const Math::Vector3& ) {}
	};

	// 周囲を捜索する
	class Search :public StateBase
	{
	public:

		~Search()							{}

		void Enter(Enemy& owner)	override;
		void Update(Enemy& owner)	override;
		void Exit(Enemy& owner)		override;
	};

	// 移動
	class MoveOtherPos :public StateBase
	{
	public:

		~MoveOtherPos()								{}

		void Enter(Enemy& owner)			override;
		void Update(Enemy& owner)			override;
		void Exit(Enemy& owner)				override;

		void Turn(Enemy& owner, const Math::Vector3& dir);				// ターン
		void CheckMoveFinish(Enemy& owner, const Math::Vector3& dist)	override;
	};

	// 追跡
	class Chase :public StateBase
	{
	public:

		~Chase()							{}

		void Enter(Enemy& owner)	override;
		void Update(Enemy& owner)	override;
		void Exit(Enemy& owner)		override;

		// 攻撃できるかを確認
		bool ChackAttackAble(Enemy& owner);
	};

	// 見失う
	class LoseSight :public StateBase
	{
	public:

		~LoseSight()								{}

		void Enter(Enemy& owner)			override;
		void Update(Enemy& owner)			override;
		void Exit(Enemy& owner)				override;
	};

	// 周囲の捜索(周囲にプレイヤーがいるかどうかを確かめるためのステート)
	class SearchAround :public StateBase
	{
	public:

		~SearchAround()						{}

		void Enter(Enemy& owner)	override;
		void Update(Enemy& owner)	override;
		void Exit(Enemy& owner)		override;

		void SearchPlayer(Enemy& owner);		// プレイヤーを捜索
		void FixNextPos(Enemy& owner);			// 次の場所を決定
	};

	// ========== 攻撃関連 ==========
	// 攻撃のステートの選択条件は、プレイヤーとの距離が一定未満になったときにプレイヤーがいる場所によって切り替える
	// 低所への攻撃
	class Attack_LowPosition :public StateBase
	{
	public:

		~Attack_LowPosition() {}

		void Enter(Enemy& owner)	override;
		void Update(Enemy& owner)	override;
		void Exit(Enemy& owner)		override;

		bool CheckAttackArea(Enemy& owner);
	};

	// ==============================

	// ステート切り替え関数
	void ChangeState(const std::shared_ptr<StateBase>& nextSate);
	std::shared_ptr<StateBase>	m_nowState = nullptr;				// 現在のステート
};