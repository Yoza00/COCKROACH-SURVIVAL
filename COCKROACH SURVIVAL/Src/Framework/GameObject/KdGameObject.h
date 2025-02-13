#pragma once

// ゲーム上に存在するすべてのオブジェクトの基底となるクラス
class KdGameObject : public std::enable_shared_from_this<KdGameObject>
{
public:

	// どのような描画を行うのかを設定するTypeID：Bitフラグで複数指定可能
	enum
	{
		eDrawTypeLit = 1 << 0,
		eDrawTypeUnLit = 1 << 1,
		eDrawTypeBright = 1 << 2,
		eDrawTypeUI = 1 << 3,
		eDrawTypeDepthOfShadow = 1 << 4,
	};

	// ステージオブジェクトのタイプ
	enum StageObjectType
	{
		None,
		Map,		// ステージマップ
		Furniture,	// 家具
		Feed,		// 餌
		Max
	};

	// キャラクタータイプ
	enum CharaType
	{
		P_Chara,	// プレイヤー
		E_Chara,	// エネミー
		N_Chara		// キャラではない
	};

	KdGameObject() {}
	virtual ~KdGameObject() { Release(); }

	// 生成される全てに共通するパラメータに対する初期化のみ
	virtual void Init() {}

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}

	// それぞれの状況で描画する関数
	virtual void GenerateDepthMapFromLight() {}
	virtual void PreDraw() {}
	virtual void DrawLit() {}
	virtual void DrawUnLit() {}
	virtual void DrawBright() {}
	virtual void DrawSprite() {}

	virtual void DrawDebug();

	virtual void SetAsset(const std::string&) {}

	virtual void SetPos(const Math::Vector3& pos) { m_mWorld.Translation(pos); }
	virtual Math::Vector3 GetPos() const { return m_mWorld.Translation(); }

	// 拡大率を変更する関数
	void SetScale(float scalar);
	virtual void SetScale(const Math::Vector3& scale);
	virtual Math::Vector3 GetScale() const;

	const Math::Matrix& GetMatrix() const { return m_mWorld; }

	virtual bool IsExpired() const { return m_isExpired; }

	virtual bool IsVisible()	const { return false; }
	virtual bool IsRideable()	const { return false; }

	// 視錐台範囲内に入っているかどうか
	virtual bool CheckInScreen(const DirectX::BoundingFrustum&) const { return false; }

	// カメラからの距離を計算
	virtual void CalcDistSqrFromCamera(const Math::Vector3& camPos);

	float GetDistSqrFromCamera() const { return m_distSqrFromCamera; }

	UINT GetDrawType() const { return m_drawType; }

	bool Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);

	const UINT GetStageObjeType() const { return m_stageObjeType; }

	const UINT GetCharaType()const		{ return m_charaType; }

	virtual void SetLife(const int) {}

	virtual void SetRotationY(float);

	const float GetRotationY()const { return m_rotationY; }

	// 内積を計算して結果を返す
	// 計算に使用する２つのベクトルを引数としてセットする
	// 使用するベクトルは必ず正規化したベクトルを使用すること
	const float CalcDotValue(const Math::Vector3& currentVec, const Math::Vector3& toVec)const;

	// 外積計算
	// 計算に使用する２つのベクトルを引数としてセットする
	// 使用するベクトルは必ず正規化したベクトルを使用すること
	Math::Vector3 CalcCrossValue(const Math::Vector3& currentvec, const Math::Vector3& toVec)const;

protected:

	void Release() {}

	// 描画タイプ・何の描画を行うのかを決める / 最適な描画リスト作成用
	UINT m_drawType = 0;

	// カメラからの距離
	float m_distSqrFromCamera = 0;

	// 存在消滅フラグ
	bool m_isExpired = false;

	// 3D空間に存在する機能
	Math::Matrix	m_mWorld;

	// 当たり判定クラス
	std::unique_ptr<KdCollider> m_pCollider = nullptr;

	// デバッグ情報クラス
	std::unique_ptr<KdDebugWireFrame> m_pDebugWire = nullptr;

	UINT m_stageObjeType	= StageObjectType::None;

	UINT m_charaType		= CharaType::N_Chara;

	float	m_rotationY = 0.0f;
};
