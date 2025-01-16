#pragma once

#include"Json/nlohmann/json.hpp"

class Player;

// ウェイポイント用構造体
struct WayPoint
{
	int				m_number;		// ポイントの番号
	Math::Vector3	m_pos;			// ポイントの座標
};

// ========== 経路探索用 ==========
	// グリッド情報を定義
struct Node
{
	int		x, z;							// ノードの座標
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

	void SetScale(float scale)
	{
		m_scale = scale;
	}
	// ==============================

	// ========== 経路探索用 ==========
	// ゴールの設定と経路探索
	void SetGoal(const Math::Vector3& goalPos);
	// ================================

private:

	std::shared_ptr<KdModelWork>	m_spModel		= nullptr;					// モデル

	std::shared_ptr<KdAnimator>		m_spAnimator	= nullptr;					// アニメーター

	Math::Vector3					m_pos			= Math::Vector3::Zero;		// 座標
	const float						m_moveSpeed		= 0.1f;						// 移動速度
	float							m_scale			= 1.0f;						// 拡縮サイズ

	float							m_gravity		= 0.0f;						// 重力
	const float						m_gravityPow	= 0.01f;					// 更新時に加算される重力の強さ

	Math::Vector3					m_loseSightPos	= Math::Vector3::Zero;		// 見失った場所

	const float						m_minDegAngle	= -360.0f;					// 回転角度の最小値
	const float						m_maxDegAngle	= 360.0f;					// 回転角度の最大値

	// ========== 座標補正用変数 ==========
	float							m_adJustHeight	= 0.0f;						// モデルの高さ補正用変数
	// ====================================
	
	// ========== 視界関係で必要となる変数 ==========
	const float						m_sightSize		= 80.0f;					// 中心からの角度
	const float						m_sightAreaSize = 100.0f;					// ぼんやりと見えている範囲
	const float						m_sightRange	= 2000.0f;					// 判定の距離
	float							m_angle			= 0.0f;						// モデルの回転角度
	const float						m_maxAngle		= 1.0f;						// 回転角度の最大値
	const float						m_rotThreshold	= 3.5f;						// 回転する閾値(回転度数がここよりも小さい場合は回転処理をしない)
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

	std::weak_ptr<Player>			m_wpPlayer;									// Playerクラスのウィークポインタ

	// ========== 経路探索用 ==========
	std::vector<Node>				m_path;											// 計算された経路
	Math::Vector3					m_currentGoal;									// 現在の目標座標
	size_t							m_currentPathIndex	= 0;						// 現在の経路インデックス
	std::vector<std::vector<int>>*	m_grid;											// 参照するグリッド
	const Math::Vector3				m_mapOrigin			= { -62.0f,0.04f,79.0f };	// グリッドの原点補正用
	
	// ヒューリスティック関数
	float Heuristic(const Node& a, const Node& b)
	{
		return std::abs(a.x - b.x) + std::abs(a.z - b.z);
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

	// モデルを進行方向に向ける関数
	void RotateToDirection(const Math::Vector3& toDir);

	// ========== 自クラスで使用するゲッター ==========
	bool GetDotResult(const Math::Vector3& toDir);								// 目線のレイ判定の結果を返す

	// Jsonファイルを使用するうえで必要となる関数
	bool LoadWayPointsFronJson(const std::string& _filePath);					// Jsonファイルが開けたかどうかの確認用関数
	// ================================================

	// ステートパターン
private:

	/*
	未発見状態の処理

	その場で、一定時間立ち止まって周囲を捜索。
	ゆっくり振り返って移動開始。
	場所についたら、立ち止まって周囲を捜索。
	ゆっくり振り返って移動開始。
	繰り返す。


	発見状態の処理

	発見する。(この際に、！等を出すと良い)
	オブジェクトがない場合は最短経路を使用して追跡。
	進行方向に障害物があれば、それを避けて追跡。
	追跡時に、見失った場合は、最後に発見した位置まで移動して周囲を捜索。
	それでも見つからなければ移動処理に戻る。
	見つかれば追跡再開。
	*/

	// ステートパターンのベースクラス
	class StateBase
	{
	public:

		virtual ~StateBase()				{}

		virtual void Enter(Enemy& owner)	{}	// その状態に入るときに１度だけ実行する処理
		virtual void Update(Enemy& owner)	{}	// その状態の間、行う処理
		virtual void Exit(Enemy& owner)		{}	// その状態から別の状態に変更するときに１度だけ実行する処理

		virtual void CheckMoveFinish(Enemy& owner, const Math::Vector3& dist) {}
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
	};

	// 見失う
	class LoseSight :public StateBase
	{
	public:

		~LoseSight()								{}

		void Enter(Enemy& owner)			override;
		void Update(Enemy& owner)			override;
		void Exit(Enemy& owner)				override;

		void CheckMoveFinish(Enemy& owner, const Math::Vector3& dist)	override;
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

	// ステート切り替え関数
	void ChangeState(const std::shared_ptr<StateBase>& nextSate);
	std::shared_ptr<StateBase>	m_nowState = nullptr;				// 現在のステート
};