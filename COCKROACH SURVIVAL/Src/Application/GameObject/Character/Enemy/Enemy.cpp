#include "Enemy.h"
#include"../../../Scene/SceneManager.h"
#include"../Player/Player.h"

#include"../../../main.h"

void Enemy::Update()
{
	if (!m_spModel)return;

	// ねみー本体から見た視界の根本の座標
	Math::Vector3 _sightPos = (m_localSightMat * m_mWorld).Translation();

	// ========== デバッグ表示 ==========
	m_pDebugWire->AddDebugSphere
	(
		_sightPos,
		0.5f,
		kBlackColor
	);
	// ==================================

	m_sightPos = _sightPos;

	// ========== 重力による座標更新 ==========
	m_gravity	+= m_gravityPow;
	m_pos.y		-= m_gravity;
	// ========================================

	// 視界判定(判定し終えてから更新処理を行う)
	CheckSight();

	// ステートパターンを使用して、状態に応じた処理を行うように処理する
	if (m_nowState)
	{
		m_nowState->Update(*this);
	}

	/*
	ステートパターンを採用する理由
	・状態によって変化する変数等はステートパターン内で処理を行う為、更新処理関数内がきれいになる。
	・状態によって区切られているため、変更・改修が容易。
	*/

	// 行列関連
	{
		// 各要素の行列を作成
		Math::Matrix	_scaleMat	= Math::Matrix::CreateScale(m_scale);									// 拡縮行列
		Math::Matrix	_rotMat		= Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));	// Y軸における回転行列
		Math::Matrix	_transMat	= Math::Matrix::CreateTranslation(m_pos);								// 座標行列

		// ワールド行列を作成(各要素の行列を合成したもの)
		m_mWorld = _scaleMat * _rotMat * _transMat;
	}
}

void Enemy::PostUpdate()
{
	if (!m_spModel)return;

	HitCheck();			// オブジェクトとの当たり判定を調べる
}

// ========== 描画関連 ==========
void Enemy::DrawLit()
{
	if (!m_spModel)return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Enemy::GenerateDepthMapFromLight()
{
	if (!m_spModel)return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
// ==============================

// 初期化
void Enemy::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		*m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Character/Enemy/Enemy.gltf");
		//*m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Character/Enemy/Enemy_people.gltf");

		// モデルからポイントノードを探して取得
		const KdModelWork::Node* _pNode = m_spModel->FindNode("sightPoint");

		if (_pNode)
		{
			m_localSightMat = _pNode->m_worldTransform;
		}
	}
	
	m_charaType = CharaType::E_Chara;

	// 初期状態のステートを設定
	ChangeState(std::make_shared<Search>());

	

	// ========== デバッグ用 ==========
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	// ================================
}

// オブジェクトとの当たり判定を調べる
void Enemy::HitCheck()
{
	// 地面との当たり判定
	KdCollider::RayInfo	_rayInfo;
	_rayInfo.m_pos		= m_pos;						// レイの発射座標(必要に応じて補正をかける)
	_rayInfo.m_dir		= Math::Vector3::Down;			// レイの発射角度(地面と判定するので、下方向へのベクトルにする必要がある)
	_rayInfo.m_range	= 3.0f;							// レイの長さ(必要に応じて、長さを調節すべし)
	_rayInfo.m_type		= KdCollider::TypeGround;		// 当たり判定を行うタイプ

	// ========== レイ情報のデバッグ表示 ==========
	m_pDebugWire->AddDebugLine(
		_rayInfo.m_pos,
		_rayInfo.m_dir,
		_rayInfo.m_range,
		kBlackColor
	);
	// ============================================

	std::list<KdCollider::CollisionResult>	_retRayList;		// 当たったオブジェクトを格納するリスト

	// すべてのオブジェクトとの当たり判定
	for (auto& _objects : SceneManager::Instance().GetObjList())
	{
		_objects->Intersects(_rayInfo, &_retRayList);			// レイ情報をもとに当たり判定を行い、当たっていればリストに格納
	}

	// 一番近いオブジェクトを探すのに必要な変数
	bool			_isHit		= false;						// 当たったかどうかのフラグ
	float			_maxOverLap = 0.0f;							// 最大の重なり量
	Math::Vector3	_groundPos	= {};							// ヒット座標(地面座標)

	// リストに格納されていうrオブジェクトの中で、一番近くであたっているオブジェクトを調べる
	for (auto& _object : _retRayList)
	{
		// 先ほど宣言したローカル行列より、当たり判定の結果の重なり量の方が大きかったら
		if (_maxOverLap < _object.m_overlapDistance)
		{
			_maxOverLap = _object.m_overlapDistance;			// 重なり量更新
			_groundPos	= _object.m_hitPos;						// ヒット座標更新
			_isHit		= true;									// フラグを起動
		}
	}

	if (_isHit)
	{
		m_pos		= _groundPos;								// 座標をヒット座標に更新
		m_pos		+= {0.0f, 0.05f, 0.0f};
		m_gravity	= 0.0f;										// 重力の強さはリセット
	}
}

// 視界内 かつ プレイヤーを視認できているかどうかを確認
void Enemy::CheckSight()
{
	const std::shared_ptr<Player>	_spPlayer = m_wpPlayer.lock();
	if (!_spPlayer)
	{
		// プレイヤーのポインタがない場合は処理しない
		return;
	}

	Math::Vector3	_nowDir			= m_mWorld.Backward();		// 向いている方向(視界の中心となる方向ベクトル)
	Math::Vector3	_nowPlayerPos	= _spPlayer->GetPos();		// プレイヤーの今の座標
	Math::Vector3	_mixVecDir		= _nowPlayerPos - m_sightPos;	// 2つの座標から１つの方向ベクトルを作成(自分(エネミー)から見たプレイヤーへのベクトル)
	//Math::Vector3	_mixVecDir		= _nowPlayerPos - m_pos;	// 2つの座標から１つの方向ベクトルを作成(自分(エネミー)から見たプレイヤーへのベクトル)

	// 一定距離以上離れてしまっている場合は処理しない
	if (_mixVecDir.LengthSquared() >= m_sightRange)
	{
		m_isSight = false;				// 視界内にいないことにする
		return;
	}

	// 方向ベクトルの正規化
	_nowDir.Normalize();
	_mixVecDir.Normalize();

	// 内積を計算
	float			_dot			= _nowDir.Dot(_mixVecDir);
	
	// 内積値の補正
	// 念のため-1.0f<=_dot<=1.0fの範囲内になるようにしておく
	_dot = std::clamp(_dot, -1.0f, 1.0f);

	bool			_isSight		= false;												// 視界内かどうか
	float			_sightAria		= cos(DirectX::XMConvertToRadians(m_sightSize));		// 視野の端っこ

	// 視界内にプレイヤーがいる場合
	if (_dot > _sightAria)
	{
		// ========== レイ判定(オブジェクトに視界を邪魔されていないか) ==========
		_isSight = GetDotResult(_mixVecDir);
		// ======================================================================
	}

	m_isSight = _isSight;			// 実行した結果をもとに、フラグを更新
	/* 実行結果から、視界内に居ればメンバー変数のフラグがtrueに、視界外に居ればfalseになる */

	if (!m_isSight)
	{
		return;						// 視界外に居る場合はリターン
	}

	// モデルの回転処理
	float	_angle = DirectX::XMConvertToDegrees(acos(_dot));		// 内積値のacosで角度を求める
	
	if (_angle < m_rotThreshold)
	{
		return;
	}

	if (_angle > m_maxAngle)					// 角度制御
	{
		_angle = m_maxAngle;
	}

	Math::Vector3	_cross;				// 外積格納用変数
	_cross = _mixVecDir.Cross(_nowDir);	// 外積計算

	// Y軸回転のみ行うため、Y要素にのみ処理を作成
	if (_cross.y >= 0.0f)
	{
		m_angle -= _angle;

		// 負の値にならないように制御
		if (m_angle < m_minDegAngle)
		{
			m_angle += m_maxDegAngle;
		}
	}
	else
	{
		m_angle += _angle;

		// 360.0f(１回転分)を超えないように制御
		if (m_angle >= m_maxDegAngle)
		{
			m_angle -= m_maxDegAngle;
		}
	}
}

// 内積の判定の結果を返す
bool Enemy::GetDotResult(const Math::Vector3& toDir)
{
	KdCollider::RayInfo _rayInfo;
	_rayInfo.m_pos		= m_sightPos;
	_rayInfo.m_dir		= toDir;
	_rayInfo.m_range	= m_sightRange;
	_rayInfo.m_type		= KdCollider::TypeGround | KdCollider::TypeSight;

	// ========== デバッグ表示 ==========
	m_pDebugWire->AddDebugLine
	(
		_rayInfo.m_pos,
		_rayInfo.m_dir,
		_rayInfo.m_range,
		kBlueColor
	);
	// ==================================

	std::list<KdCollider::CollisionResult>	_retRayList;		// ヒットオブジェクト格納用リスト
	std::vector<UINT>						_charaNumber;		// ヒットオブジェクトのキャラナンバー格納用配列

	for (auto& _objects : SceneManager::Instance().GetObjList())
	{
		if (_objects->Intersects(_rayInfo, &_retRayList))			// レイ情報をもとに、ヒットオブジェクトをリストに格納
		{
			_charaNumber.push_back(_objects->GetCharaType());		// 可変長配列にヒットオブジェクトのキャラナンバーを格納
		}
	}

	int		_cnt		= 0;			// カウンタ変数
	UINT	_number		= 0;			// 最終的なキャラナンバー
	float	_maxOverLap = 0.0f;			// 重なり量
	bool	_isHit		= false;		// ヒットフラグ

	for (auto& _ret : _retRayList)
	{
		if (_maxOverLap < _ret.m_overlapDistance)
		{
			_number = _charaNumber[_cnt];			// 可変長配列からほしい番号を取得

			_maxOverLap = _ret.m_overlapDistance;		// 重なり量を更新
			_isHit = true;							// フラグ起動
		}

		_cnt++;			// カウンタ更新
	}

	if (_isHit)
	{
		// エネミーのプレイヤーへの視線がオブジェクトで妨げられているかどうか
		// 判定方法は、レイに一番近くであたっているのがプレイヤーかどうかで判定
		if (_number == CharaType::P_Chara)
		{
			return true;			// プレイヤーが一番近い(オブジェクトに妨げられていない)
		}
	}

	return false;
}

// ========== ステートパターン関係 ==========
void Enemy::ChangeState(const std::shared_ptr<StateBase>& nextSate)
{
	if (m_nowState)
	{
		m_nowState->Exit(*this);
	}
	m_nowState = nextSate;
	m_nowState->Enter(*this);
}

// ========== 捜索 ==========
void Enemy::Search::Enter(Enemy& owner)
{

}

void Enemy::Search::Update(Enemy& owner)
{
	// 視界判定の結果、フラグが切り替わっている(視界内にプレイヤーがいる)場合、ステートを切り替える
	if (owner.m_isSight)
	{
		owner.ChangeState(std::make_shared<Chase>());			// 追跡状態に切り替え
		return;
	}
}

void Enemy::Search::Exit(Enemy& owner)
{

}

// ========== その他の場所へ移動 ==========
void Enemy::MoveOtherPos::Enter(Enemy& owner)
{
}

void Enemy::MoveOtherPos::Update(Enemy& owner)
{
	if (owner.m_isSight)
	{
		owner.ChangeState(std::make_shared<Chase>());
		return;
	}
}

void Enemy::MoveOtherPos::Exit(Enemy& owner)
{
}

// ========== 追跡 ==========
void Enemy::Chase::Enter(Enemy& owner)
{
}

void Enemy::Chase::Update(Enemy& owner)
{
	const std::shared_ptr<Player>	_spPlayer = owner.m_wpPlayer.lock();
	if (!_spPlayer)
	{
		owner.ChangeState(std::make_shared<Search>());			// 探索状態に切り替え
		return;
	}

	Math::Vector3	_playerPos = _spPlayer->GetPos();			// プレイヤー座標取得

	// 視界判定の結果、プレイヤーが視界外に居る場合、ステートを切り替える
	if (!owner.m_isSight)
	{
		owner.m_loseSightPos = _playerPos;						// 見失った座標をコピー
		owner.ChangeState(std::make_shared<LoseSight>());		// 見失った状態に切り替え
		return;
	}

	Math::Vector3	_VectorDir = _playerPos - owner.m_pos;		// 自分から見たプレイヤーへのベクトルを算出
	_VectorDir.Normalize();										// 方向ベクトルなので正規化しておく

	owner.m_pos	+= _VectorDir * owner.m_moveSpeed;				// 座標を更新
}

void Enemy::Chase::Exit(Enemy& owner)
{
}

// ========== 見失った ==========
void Enemy::LoseSight::Enter(Enemy& owner)
{
	owner.m_isMoveFinish = false;			// フラグ切り替え(移動が完了していない状態に)
}

void Enemy::LoseSight::Update(Enemy& owner)
{
	const std::shared_ptr<Player>	_spPlayer = owner.m_wpPlayer.lock();
	if (!_spPlayer)
	{
		owner.ChangeState(std::make_shared<Search>());			// 探索状態に切り替え
		return;
	}

	Math::Vector3	_nowDir = owner.m_mWorld.Backward();			// エネミーの正面方向のベクトル
	Math::Vector3	_toDir	= owner.m_loseSightPos - owner.m_pos;	// 向きたい方向

	// それぞれのベクトル正規化
	_nowDir.Normalize();
	_toDir.Normalize();

	// ========== 移動処理 ===========
	owner.m_pos += _toDir * owner.m_moveSpeed;

	CheckMoveFinish(owner);
	// ===============================

	// ========== 内積を計算 ==========
	float	_dot	= _nowDir.Dot(_toDir);
	_dot			= std::clamp(_dot, -1.0f, 1.0f);				// 内積値を補正
	// ================================

	// ========== モデルの回転処理 ==========
	float	_angle = DirectX::XMConvertToDegrees(acos(_dot));		// 内積値のacosで角度を求める

	if (_angle > owner.m_maxAngle)
	{
		_angle = owner.m_maxAngle;
	}

	Math::Vector3	_cross = _toDir.Cross(_nowDir);					// 外積の計算

	if (_cross.y >= 0.0f)
	{
		owner.m_angle -= _angle;

		// 角度が範囲外に行かないように制御
		if (owner.m_angle < owner.m_minDegAngle)
		{
			owner.m_angle += owner.m_maxDegAngle;
		}
	}
	else
	{
		owner.m_angle += _angle;

		if (owner.m_angle >= owner.m_maxDegAngle)
		{
			owner.m_angle -= owner.m_maxDegAngle;
		}
	}
	// ======================================
}

void Enemy::LoseSight::Exit(Enemy& owner)
{
	owner.m_isMoveFinish = true;
}

void Enemy::LoseSight::CheckMoveFinish(Enemy& owner)
{
	Math::Vector3	_dist = owner.m_loseSightPos - owner.m_pos;			// 距離

	// 距離を確認
	if (_dist.LengthSquared() <= 2.5f)
	{
		owner.ChangeState(std::make_shared<SearchAround>());			// ステートを切り替える
		return;
	}
}

// ========== 周囲の捜索 ==========
void Enemy::SearchAround::Enter(Enemy& owner)
{
	owner.m_searchPhase = SearchPhase::RotRight;
	owner.m_searchTimer = 960;
	owner.m_baseAngle	= owner.m_angle;
}

void Enemy::SearchAround::Update(Enemy& owner)
{
	owner.m_searchTimer--;				// 捜索制限時間の更新

	// 視界内にプレイヤーがいれば
	if (owner.m_isSight)
	{
		owner.ChangeState(std::make_shared<Chase>());
		return;
	}

	// カウンタを参照して処理分岐
	if(owner.m_searchTimer >= 0)
	{
		SearchPlayer(owner);				// プレイヤーを捜索
	}
	else
	{
		owner.m_isSearchFin = true;			// フラグ起動
	}

	// 探索終了
	if (owner.m_isSearchFin)
	{
		owner.ChangeState(std::make_shared<MoveOtherPos>());		// ステート切り替え
		return;
	}
}

void Enemy::SearchAround::Exit(Enemy& owner)
{
	owner.m_isSearchFin = false;		// 起動状態のフラグを解除
}

void Enemy::SearchAround::SearchPlayer(Enemy& owner)
{
	// 現在の状態から判断
	switch (owner.m_searchPhase)
	{
	case SearchPhase::RotRight:	// 右回転
		
		owner.m_angle += owner.m_rotationSpd;		// 回転角度更新

		// 念のための回転制御
		if (owner.m_angle >= owner.m_maxDegAngle)
		{
			owner.m_angle -= owner.m_maxDegAngle;
		}

		// 回転終了
		if (owner.m_angle >= owner.m_baseAngle + owner.m_maxDegAng)
		{
			owner.m_angle = owner.m_baseAngle + owner.m_maxDegAng;
			owner.m_frameCnt = 0;
			owner.m_searchPhase = SearchPhase::PauseRight;
		}
		break;

	case SearchPhase::PauseRight:
		
		owner.m_frameCnt++;		// カウンタ更新

		// 一定フレーム経過後
		if (owner.m_frameCnt >= 20)
		{
			owner.m_searchPhase = SearchPhase::RotLeft;
		}
		break;

	case SearchPhase::RotLeft:

		owner.m_angle -= owner.m_rotationSpd;

		// 念のための回転制御
		if (owner.m_angle < owner.m_minDegAngle)
		{
			owner.m_angle += owner.m_maxDegAngle;
		}

		// 回転終了
		if (owner.m_angle <= owner.m_baseAngle - owner.m_maxDegAng)
		{
			owner.m_angle = owner.m_baseAngle - owner.m_maxDegAng;
			owner.m_frameCnt = 0;
			owner.m_searchPhase = SearchPhase::PauseLeft;
		}
		break;

	case SearchPhase::PauseLeft:

		owner.m_frameCnt++;

		if (owner.m_frameCnt >= 20)
		{
			owner.m_searchPhase = SearchPhase::RotRight;
		}
		break;
	}
}
