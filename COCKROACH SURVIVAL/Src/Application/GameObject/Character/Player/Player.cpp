#include "Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../../main.h"

#include"../../../GameObject/Camera/TPSCamera/TPSCamera.h"

#include"../../../GameObject/Stage/ObjectData/ObjectData.h"
#include"../../../GameObject/UI/Notice/Notice.h"
#include"../../../GameObject/UI/EatTips/EatTips.h"

Player::Player() :
	m_pos(m_startPlayerPos),
	m_actionType(ActionType::Idle),
	m_nowMovingPos(Position::Ground)
{
}

void Player::Update()
{
	if (m_isDead)return;

	// ========== デバッグ(プレイヤーキル) ==========
	if (GetAsyncKeyState('P') & 0x8000)
	{
		m_isDead = true;
	}
	// ==============================================

	// フラグ制御
	if (m_isJump)
	{
		if (m_touchWall)
		{
			// ジャンプ状態にあるにも関わらず張り付き状態を継続している場合は、張り付きフラグを解除する
			m_touchWall = false;
		}
	}

	const std::shared_ptr<Notice>	_spNotice = m_wpNotice.lock();
	if (!_spNotice)
	{
		m_isRestDecreaseStart = true;
	}
	else
	{
		m_isRestDecreaseStart = false;
	}

	if (m_isRestDecreaseStart)
	{
		// 満腹度の制御
		if (m_restNum <= 0.0f)		// 空腹状態
		{
			m_restNum = 0.0f;		// 負の値にならないように制御

			m_isHungry = true;		// 空腹フラグを起動
		}
		else
		{
			if (m_actionType == ActionType::Idle)
			{
				m_restNum -= m_notActionDecreaseRestValue;		// 満腹度減少
			}
			else
			{
				m_restNum -= m_actionDecreaseRestValue;
			}

			// 空腹フラグが起動している状態で、空腹度が少しでもある状態なら空腹フラグを戻す
			m_isHungry = false;
		}
	}

	// 空腹状態の処理
	if (m_isHungry == true)
	{
		// 体力の減少処理
		if (m_actionType == ActionType::Idle)
		{
			// 待機状態なら
			m_life -= m_notActionDecreaseLifeValue;
		}
		else
		{
			// 何らかのアクション状態
			m_life -= m_actionDecreaseLifeValue;
		}
	}

	if (m_life <= 0.0f)
	{
		m_isDead = true;		// 体力がなくなれば死亡フラグを起動
	}

	UINT	_tmpActionType = m_actionType;		// アクションタイプを一時的に保存しておく
	//			↑現時点でのアクションタイプを格納するための変数
	m_actionType = ActionType::Idle;			// 待機モーションを入れておく
	// (待機モーションを入れておくことで、キー入力がなかった場合にモーション切り替えが行われるようにできる)

	// カメラのY軸回転行列を取得
	Math::Matrix	_cameraMat;		// 進行方向決定用回転行列
	const std::shared_ptr<CameraBase>	_spCamera = m_wpCamera.lock();

	if (_spCamera)
	{
		// 必要なカメラの回転角度を取得する
		// カメラの向きで進行方向が決定する
		_cameraMat = _spCamera->GetRotationCameraMatrix(m_nowMovingPos);
	}


	// 移動処理
	Math::Vector3	_moveVec;

	if (m_isRestDecreaseStart)
	{
		// 移動処理
		{
			bool	_isBackWalk = false;

			// 前進以外は得意としないので、前進以外の移動量はやや低く設定する
			if (GetAsyncKeyState('W') & 0x8000)
			{
				_moveVec += Math::Vector3::TransformNormal({ 0.0f,0.0f,1.0f }, _cameraMat);
				m_actionType = ActionType::Run;
			}
			if (GetAsyncKeyState('A') & 0x8000)
			{
				_moveVec += Math::Vector3::TransformNormal({ -0.5f,0.0f,0.0f }, _cameraMat);
				m_actionType = ActionType::Run;
			}
			if (GetAsyncKeyState('D') & 0x8000)
			{
				_moveVec += Math::Vector3::TransformNormal({ 0.5f,0.0f,0.0f }, _cameraMat);
				m_actionType = ActionType::Run;
			}
			if (GetAsyncKeyState('S') & 0x8000)
			{
				_moveVec += Math::Vector3::TransformNormal({ 0.0f,0.0f,-0.3f }, _cameraMat);
				m_actionType = ActionType::Run;
				_isBackWalk = true;
			}

			m_isBackWalk = _isBackWalk;
		}
	}

	// 格納されているアニメーションタイプが違う時にアニメーションを切り替える関数を呼び出す
	if (m_actionType != _tmpActionType)
	{
		AnimationUpdate();
	}
	else
	{
		m_actionType = _tmpActionType;
	}

	// ジャンプ
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		// ジャンプ状態でないとき
		if (!m_isJump)
		{
			// ジャンプフラグを切り替える
			m_isJump = true;

			m_pos -= m_gravityDir * m_jumpPow;

			m_gravityDir = Math::Vector3::Down;			// 重力方向を下方向に切り替え
		}
	}

	_moveVec.Normalize();
	m_moveDir = _moveVec;

	m_direction = m_moveDir;

	m_pos += _moveVec * m_moveSpeed;	// 座標更新

	// 重力関係
	m_gravityDir.Normalize();			// 念のため重力の方向ベクトルを正規化

	m_pos += (m_gravityDir * m_gravity);
	m_gravity += m_gravityPow;			// 重力の強さを更新

	// 当たり判定の処理が実行できない場所に行ってしまうのを防止
	// スフィアの半径サイズ以上の強さにならないように補正
	if (m_gravity > m_adJustHeight)
	{
		// 重力の強さをスフィアの半径サイズよりやや小さい値補正
		m_gravity = m_adJustHeight - 0.05f;
	}

	Math::Matrix	_scaleMat	= Math::Matrix::CreateScale(m_scale);

	Math::Matrix	_rotMat		= Math::Matrix::Identity;

	// タイトルかどうか
	if (m_isTitle)
	{
		_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f)) * _cameraMat;
	}
	else
	{
		_rotMat = _cameraMat;
	}

	// 合成行列作成
	Math::Matrix	_transMat	= Math::Matrix::CreateTranslation(m_pos);
	m_mWorld					= _scaleMat * _rotMat * _transMat;

}

void Player::PostUpdate()
{
	if (!m_spModel)return;

	// 当たり判定
	HitJudge();

	// アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}

void Player::GenerateDepthMapFromLight()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Player::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Player::Init()
{
	if (!m_spModel)
	{
		// モデルデータのロード
		m_spModel = std::make_shared<KdModelWork>();
		*m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Character/Player/Player_mini-animation.gltf");

		// コライダー設定
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("SightPlayer", m_spModel, KdCollider::Type::TypeSight);

		// 初期アニメーションをセット
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Idle"));

		m_charaType = CharaType::P_Chara;
	}

	m_pos.y += m_adJustHeight;

	m_direction = m_mWorld.Forward();
}

void Player::SetNotice(const std::shared_ptr<Notice>& spNotice)
{
	m_wpNotice			= spNotice;
}

void Player::SetTips(const std::shared_ptr<EatTips>& spTips)
{
	m_wpTips = spTips;
}

void Player::SetAteFood(float life, float restNum, int score)
{
	m_life		+= life;		// HPを増加
	m_restNum	+= restNum;		// 満腹度回復 
	m_score		+= score;		// スコア更新

	// 各種変数の値を制御
	// ここはプレイヤーのパラメータをセットする関数を作成した際に、
	// 引数に変数の最大値をセットして、初期化の段階で各種変数に最大数をセットするように書き換える
	if (m_life > m_MaxHP_RestNumValue)
	{
		m_life = m_MaxHP_RestNumValue;
	}
	if (m_restNum > m_MaxHP_RestNumValue)
	{
		m_restNum = m_MaxHP_RestNumValue;
	}
}

void Player::AnimationUpdate()
{
	// 格納されているアクションタイプを確認し、適切なアクションを行うようにする
	switch (m_actionType)
	{
	case ActionType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Idle"));
		m_isMove = false;
		break;
	case ActionType::Run:
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Run"));
		m_isMove = true;
		break;
	case ActionType::Jump:
		break;
	default:
		break;
	}
}

const Math::Vector3 Player::AdJustDirection(const Math::Vector3& dir, float angleInDegrees)
{
	// 進行方向ベクトルを正規化
	Math::Vector3	_normalizedDirection = dir;
	_normalizedDirection.Normalize();

	// 角度をラジアンに変換
	float angleInRadians = DirectX::XMConvertToRadians(angleInDegrees);

	// 下向きベクトル
	Math::Vector3 downVec = Math::Vector3::Down;

	Math::Vector3 adjustedDirection = _normalizedDirection * cos(angleInRadians) + downVec * sin(angleInRadians);
	adjustedDirection.Normalize();

	return adjustedDirection;
}

void Player::ChangeMovePosition(UINT num)
{
	// 呼び出された際に引数と現在の移動状態フラグが同じ場合は
	// 同じ処理を行う必要がないのでリターン
	if (m_nowMovingPos == num)return;

	m_touchWall = false;		// 壁に張り付いていない
	m_isJump	= false;		// ジャンプしていない
	m_isCeiling = false;

	// 移動状態フラグ切替
	switch (num)
	{
	case 0:
		// オブジェクトの上移動状態
		// 下から上方向への方向ベクトル
		m_nowMovingPos = Position::Ground;
		m_gravityDir = Math::Vector3::Down;

		break;

	case 1:
		// 天井にぶら下がっている状態
		// 上から下方向への方向ベクトル
		m_nowMovingPos = Position::Ceiling;
		m_gravityDir = Math::Vector3::Up;
		m_touchWall = true;
		m_isCeiling = true;
		break;

	case 2:
		// 側面右
		m_nowMovingPos = Position::Wall_Left;
		m_gravityDir = Math::Vector3::Left;		// 右から左のベクトル
		m_touchWall = true;

		break;

	case 3:
		// 側面左
		m_nowMovingPos = Position::Wall_Right;
		m_gravityDir = Math::Vector3::Right;	// 左から右のベクトル
		m_touchWall = true;

		break;

	case 4:
		// 側面手前
		m_nowMovingPos = Position::Wall_Backward;
		m_gravityDir = Math::Vector3::Backward;
		m_touchWall = true;

		break;

	case 5:
		// 側面奥
		m_nowMovingPos = Position::Wall_Forward;
		m_gravityDir = Math::Vector3::Forward;
		m_touchWall = true;

		break;

	case 6:
		// 飛行状態
		// 上記のどれにも該当しない
		m_nowMovingPos = Position::Flying;
		m_gravityDir = Math::Vector3::Down;
		m_isJump = true;

		break;
	}

	// カメラに移動状態を知らせる
	const std::shared_ptr<CameraBase> _spCamera = m_wpCamera.lock();
	if (_spCamera)
	{
		_spCamera->SetMovingPos(m_nowMovingPos);
	}
}

// 引数で持ってきた法線は回転を考慮していない、モデルデータからとってきた法線情報なので、
// 回転を考慮した法線情報に変更する
void Player::CheckMovePosition(const Math::Vector3& normal, float rotY)
{
	// 角度制御
	if (rotY > 180.0f)
	{
		rotY -= 180.0f;
	}

	// オブジェクトのY軸の回転角度
	float	_objectRotationY = DirectX::XMConvertToRadians(rotY);

	// Y軸の回転行列の生成
	Math::Matrix	_rotationYMat = Math::Matrix::CreateRotationY(_objectRotationY);

	// 法線ベクトルをオブジェクトのローカル座標系に変換
	Math::Matrix	_invertRotationYMat = _rotationYMat.Invert();
	Math::Vector3	_tmpNormal = Math::Vector3::TransformNormal(normal, _invertRotationYMat);

	Math::Vector3	_tmpBox		= Math::Vector3::Zero;
	float			_dotProduct;					// 内積値を格納するための変数
	float			_resultDot	= -1.0f;			// 最終的な内積値が格納される変数
	int				_dirNum		= 0;				// 移動状態の番号用
	int				_resultNum	= 0;				// 最終的な移動状態の番号

	// 全ての方向ベクトルと法線ベクトルとを比較して、
	// 法線ベクトルに一番近い基準ベクトルがどれかを求める
	for (auto& dir : m_vectorDir)
	{
		// 内積値を算出
		_dotProduct = CalcDotValue(_tmpNormal, dir);

		// さっき求めた内積値が、それまでに算出された内積値の最高値よりも大きい場合
		if (_dotProduct > _resultDot)
		{
			_resultDot = _dotProduct;		// 内積値の更新
			_resultNum = _dirNum;			// 移動状態用の番号更新
		}

		_dirNum++;							// 番号更新
	}

	// 6つの基準ベクトルに番号を用意し、
	// 法線ベクトルがどの基準ベクトルに一番近いかを求め、
	// 基準ベクトルの番号を移動状態変更用関数に持っていく
	ChangeMovePosition(_resultNum);
}

const Math::Vector3 Player::GetAdJustedRayDirection(const Math::Vector3& forward, const Math::Vector3& gravityDir, float downwardBias)
{
	// 進行方向に重力方向(下向き成分)を加算
	Math::Vector3	_adJustedDirection = forward + (gravityDir * downwardBias);

	// 正規化してレイの方向ベクトルを取得
	_adJustedDirection.Normalize();

	// 正規化された方向ベクトルを返す
	return _adJustedDirection;
}

// 当たり判定をまとめたクラス(当たり判定に関係しているものはこの関数内で呼び出す)
// この関数自体はPostUpdate()内で呼び出す
void Player::HitJudge()
{
	NormalCheck();			// オブジェクトの法線取得
	
	GroundCheck();			// 地面との当たり判定

	HitSphereCheck();		// 横方向にあるオブジェクトの当たり判定
	
	EatFoodSphereCheck();	// 食事用当たり判定(一定のエリア内にいるかどうか)
}

void Player::NormalCheck()
{
	// 後ろ歩きでは処理を実行しない
	// 後ろ歩きは苦手とするため
	if (m_isBackWalk)
	{
		return;
	}

	// =============================
	// 法線取得用レイ(進行方向にやや下方向に飛ばすレイ)
	// =============================
	KdCollider::RayInfo _normalCheckRay;
	_normalCheckRay.m_pos = m_pos;
	_normalCheckRay.m_dir = GetAdJustedRayDirection(m_direction, m_gravityDir, 1.0f);
	_normalCheckRay.m_range = 1.0f;
	_normalCheckRay.m_type = KdCollider::Type::TypeGround;

	m_wpObject.reset();

	std::list<KdCollider::CollisionResult>	_retNormalRayList;

	// 全オブジェクトからレイ情報をもとに当たっているオブジェクトをリストに格納する
	for (auto& _object : SceneManager::Instance().GetObjList())
	{
		if (_object->Intersects(_normalCheckRay, &_retNormalRayList))
		{
			if (_object->GetStageObjeType() == StageObjectType::Furniture)
			{
				m_wpObject = _object;
			}
		}
	}

	Math::Vector3	_normalVec	= {};			// 法線ベクトル保存用
	bool			_isHit		= false;		// フラグをリセット
	float			_maxOverLap = 0.0f;			// 重なり量リセット
	float			_rotY		= 0.0f;			// レイとの判定を行うオブジェクトのY軸の回転角度

	// 作成されたリストをもとに一番近くのものを探す
	for (auto& _result : _retNormalRayList)
	{
		if (_maxOverLap < _result.m_overlapDistance)
		{
			_maxOverLap = _result.m_overlapDistance;
			_normalVec = _result.m_normal;
			_isHit = true;
		}
	}

	// 最終的に行う処理
	if (_isHit)
	{
		// ここで取得できた法線情報をセットし、
		// 法線情報をもとに必要な処理を作成していく
		// (重力方向の変更、モデルの回転、など)
		_normalVec.Normalize();
		m_normal = _normalVec;

		// 念のための正規化
		m_normal.Normalize();

		std::shared_ptr<KdGameObject>	_spObject = m_wpObject.lock();
		if (_spObject)
		{
			_rotY = _spObject->GetRotationY();
		}

		CheckMovePosition(m_normal, _rotY);
	}
}

void Player::GroundCheck()
{
	// =========================================
	// プレイヤーから真下(地面方向)へのレイ
	// =========================================
	KdCollider::RayInfo		_rayInfo;						// レイ情報
	Math::Vector3			_gravityDir = m_gravityDir;
	_gravityDir.Normalize();

	// レイ判定に使用するレイの情報を設定
	_rayInfo.m_pos		= m_pos;						// レイの発射座標 = プレイヤー座標
	_rayInfo.m_dir		= _gravityDir;					// レイを飛ばす方向は重力方向
	_rayInfo.m_range	= m_adJustHeight + 0.05f;		// レイの長さはキャラクターのスフィアの半径よりもやや長いレイを飛ばす
	_rayInfo.m_type		= KdCollider::TypeGround;		// 当たり判定を行うタイプ

	std::list<KdCollider::CollisionResult>	_retRayList;		// レイ判定時に当たったオブジェクトを格納するためのリスト

	for (auto& _obj : SceneManager::Instance().GetObjList())
	{
		// オブジェクトリストからレイ情報をもとに、当たっているオブジェクトをリストに格納する
		_obj->Intersects(_rayInfo, &_retRayList);
	}

	bool			_isHit		= false;
	float			_maxOverLap = 0.0f;
	Math::Vector3	_groundPos	= {};						// 地面の場所(押し返す座標)を格納するための変数

	// レイに当たっているオブジェクトから一番近くで当たっているオブジェクトを調査する処理
	for (auto& _ret : _retRayList)
	{
		// レイの飛び出した長さが最も長い(一番近い)オブジェクトを求める
		if (_maxOverLap < _ret.m_overlapDistance)
		{
			_maxOverLap = _ret.m_overlapDistance;
			_groundPos	= _ret.m_hitPos;
			_isHit		= true;
		}
	}

	// 当たり判定の結果行う処理(地面に触れている際の処理)
	if (_isHit)
	{
		// 壁移動中でない場合
		if (!m_touchWall)
		{
			m_pos	= _groundPos;			// プレイヤーの座標を地面(レイが当たった座標)に移動
			m_pos.y += m_adJustHeight;		// 高さの情報を(補正した分)戻す
		}
		else // 壁移動中の場合
		{
			Math::Vector3	_directionDir = _groundPos - m_pos;		// 押し返さないといけない方向ベクトル(対象物から見たプレイヤーへの方向ベクトル)
			_directionDir.Normalize();
			Math::Vector3	_overlap = _directionDir * m_adJustHeight;
			m_pos = _groundPos - _overlap;
		}

		m_gravity = 0.0f;				// 重力の強さをリセット
	}
	else // レイにあたっていない
	{
		ChangeMovePosition(6);
	}
}

void Player::HitSphereCheck()
{
	// =============================
	// スフィア判定
	// =============================
	KdCollider::SphereInfo	_sphere;						// スフィア情報
	
	if (!m_isCeiling)
	{
		_sphere.m_sphere.Center = m_pos;						// スフィアの中心座標
	}
	else
	{
		_sphere.m_sphere.Center = m_pos - Math::Vector3(0.0f, 0.1f, 0.0f);
	}

	_sphere.m_sphere.Radius = m_adJustHeight;				// スフィアの半径
	_sphere.m_type = KdCollider::Type::TypeGround;

	std::list<KdCollider::CollisionResult>	_retSphereList;	// 当たり判定の結果を保存していくリスト

	// オブジェクトリストの中身をすべて確認して、レイに当たったオブジェクトを作成したリストに格納していく
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		obj->Intersects(_sphere, &_retSphereList);			// スフィア情報をもとに当たっているかどうかを確認
		// 当たっている場合にのみ_retRayListに格納されている
	}

	float			_maxOverLap = 0.0f;		// 重なり量をリセット
	Math::Vector3	_hitDir		= {};		// 当たった向き
	bool			_isHit		= false;	// フラグをリセット

	// オブジェクトリストから当たったオブジェクトをリストに格納する
	for (auto& ret : _retSphereList)
	{
		// 一番近くで当たっているオブジェクトを捜索
		if (_maxOverLap < ret.m_overlapDistance)
		{
			_maxOverLap = ret.m_overlapDistance;	// 重なり量更新
			_hitDir		= ret.m_hitDir;				// 当たり方向更新
			_isHit		= true;						// フラグ更新
		}
	}

	// 当たり判定の結果行う処理
	if (_isHit)
	{
		_hitDir.Normalize();
		m_pos		+= _hitDir * _maxOverLap;
		m_gravity	= 0.0f;
	}
}

void Player::EatFoodSphereCheck()
{
	std::shared_ptr<EatTips>	_spTips = m_wpTips.lock();

	// アクセスチェックを行い、アクセスできる場合は、描画を行わないように設定しておく
	if (_spTips)
	{
		_spTips->SetIsDraw(false);
	}

	// 食べる処理用の球判定
	KdCollider::SphereInfo	_eatSphere;
	_eatSphere.m_sphere.Center	= m_pos;
	_eatSphere.m_sphere.Radius	= m_canEatDistance;
	_eatSphere.m_type			= KdCollider::TypeBump;

	// 全てのオブジェクトと設定したスフィアとの当たり判定を行い、
	// Tipsを表示するかどうかを設定する
	for (auto& _obj : SceneManager::Instance().GetObjList())
	{
		// 設定したスフィアとオブジェクトが当たった場合
		if (_obj->Intersects(_eatSphere, nullptr))
		{
			if (_spTips)
			{
				// Tipsを表示させる
				_spTips->SetIsDraw(true);
			}

			// 表示されている間、左クリックで食べることができる
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				// 一定の値を引数に置き、餌のライフを減少させていく
				_obj->SetLife(m_decreaceFoodLife);
			}
		}
	}
}
