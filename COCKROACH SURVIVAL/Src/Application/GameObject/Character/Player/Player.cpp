#include "Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../../main.h"

#include"../../../GameObject/Camera/TPSCamera/TPSCamera.h"
//#include"../../../GameObject/Camera/FPSCamera/FPSCamera.h"

#include"../../../GameObject/Stage/ObjectData/ObjectData.h"
//#include"../../../GameObject/UI/Clock/TimeLimit/TimeLimit.h"

Player::Player() :
	m_actionType(ActionType::Idle),
	m_nowMovingPos(Position::Ground)
{
}

void Player::Update()
{
	if (m_isDead)return;

	// フラグ制御
	if (m_isJump)
	{
		if (m_touchWall)
		{
			// ジャンプ状態にあるにも関わらず張り付き状態を継続している場合は、張り付きフラグを解除する
			m_touchWall = false;
		}
	}

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
			m_restNum -= 0.00135f;		// 満腹度減少
		}
		else
		{
			m_restNum -= 0.00027f;
		}

		// 空腹フラグが起動している状態で、空腹度が少しでもある状態なら空腹フラグを戻す
		m_isHungry = false;
	}

	// 空腹状態の処理
	if (m_isHungry == true)
	{
		// 体力の減少処理
		if (m_actionType == ActionType::Idle)
		{
			// 待機状態なら
			m_life -= 0.001f;
		}
		else
		{
			// 何らかのアクション状態
			m_life -= 0.0025f;
		}

		if (m_life <= 0.0f)
		{
			m_isDead = true;		// 体力がなくなれば死亡フラグを起動
		}
	}

	UINT	_tmpActionType = m_actionType;		// アクションタイプを一時的に保存しておく
	//			↑現時点でのアクションタイプを格納するための変数
	m_actionType = ActionType::Idle;			// 待機モーションを入れておく
	// (待機モーションを入れておくことで、キー入力がなかった場合にモーション切り替えが行われるようにできる)

	// カメラのY軸回転行列を取得
	Math::Matrix	_cameraMat;		// 進行方向決定用回転行列
	const std::shared_ptr<CameraBase>	_spCamera = m_wpCamera.lock();
	//if (!m_wpCamera.expired())
	//{
	//	// カメラが有効の時、Y軸方向の回転行列を取得する
	//	//_cameraMat = m_wpCamera.lock()->GetRotationYMatrix();

	//	
	//}

	Math::Matrix	_charaRotationMatrix;			// キャラモデル回転用回転行列
	if (_spCamera)
	{
		// 必要なカメラの回転角度を取得する
		_cameraMat = _spCamera->GetRotationCameraMatrix(m_nowMovingPos);

		_charaRotationMatrix = _spCamera->GetRotationYMatrix();
	}


	// 移動処理
	Math::Vector3	_moveVec;

	// 移動処理
	{

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
	//m_gravityDir *= m_gravity;			// 重力方向の重力の強さを更新

	m_pos += (m_gravityDir * m_gravity);
	m_gravity += m_gravityPow;			// 重力の強さを更新
	// 当たり判定の処理が実行できない場所に行ってしまうのを防止
	// スフィアの半径サイズ以上の強さにならないように補正
	if (m_gravity > m_adJustHeight)
	{
		// 重力の強さをスフィアの半径サイズよりやや小さい値補正
		m_gravity = m_adJustHeight - 0.05f;
	}

	Math::Matrix	_scaleMat = Math::Matrix::CreateScale(m_scale);
	//m_modelRotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_modelRot));
	{
		// 回転行列作成
		Math::Matrix	_rotXMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_modelRotate.x));
		Math::Matrix	_rotYMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_modelRotate.y));
		Math::Matrix	_rotZMat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_modelRotate.z));

		m_modelRotMat = _rotXMat * _rotYMat * _rotZMat;

		//Application::Instance().m_log.AddLog("%0.2f\n", m_modelRotate.y);
	}
	//Math::Matrix	_rotMat = _cameraMat;

	Math::Matrix	_rotMat = m_modelRotMat * _charaRotationMatrix;
	//m_rotMat = _cameraMat;
	Math::Matrix	_transMat = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = _scaleMat * _rotMat * _transMat;
	//m_mWorld = _scaleMat * _rotMat * _transMat;
	//m_mWorld = _scaleMat * (/*m_modelRotMat * */m_rotMat) * _transMat;

}

void Player::PostUpdate()
{
	if (m_isMenu)return;

	if (!m_spModel)return;

	// 当たり判定
	HitJudge();

	// アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}

void Player::GenerateDepthMapFromLight()
{
	if (m_isMenu)return;

	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Player::DrawLit()
{
	if (m_isMenu)return;

	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Player::Init()
{
	if (!m_spModel)
	{
		// モデルデータのロード
		m_spModel = std::make_shared<KdModelWork>();
		//*m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Character/Player/Player_1.gltf");
		//m_spModel->SetModelData("Asset/Models/Character/Player/Player_1.gltf");
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

	// ========================================
	// デバッグ用
	// ========================================
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	//=========================================
}

void Player::SetAteFood(float life, float restNum, int score)
{
	m_life		+= life;		// HPを増加
	m_restNum	+= restNum;		// 満腹度回復 
	m_score		+= score;		// スコア更新

	// 各種変数の値を制御
	// ここはプレイヤーのパラメータをセットする関数を作成した際に、
	// 引数に変数の最大値をセットして、初期化の段階で各種変数に最大数をセットするように書き換える
	if (m_life > 100.0f)
	{
		m_life = 100.0f;
	}
	if (m_restNum > 100.0f)
	{
		m_restNum = 100.0f;
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
	case ActionType::Fly:
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
	//Math::Vector3	downVec = m_gravityDir;

	//// 判定用となる基準ベクトル(重力方向)
	//Math::Vector3	_nowGravityDir = m_gravityDir;
	//_nowGravityDir.Normalize();

	Math::Vector3 adjustedDirection = _normalizedDirection * cos(angleInRadians) + downVec * sin(angleInRadians);
	//Math::Vector3 adjustedDirection = _normalizedDirection * cos(angleInRadians) + _nowGravityDir * sin(angleInRadians);
	adjustedDirection.Normalize();

	// 進行方向と下向きのベクトルを補間して、調整した方向ベクトルを取得	
	//Math::Matrix rotationMat = Math::Matrix::CreateFromAxisAngle(downVec, angleInRadians);	// 任意のベクトルを中心とした回転を表す行列を作成
	//Math::Vector3 adjustedDirection = Math::Vector3::TransformNormal(dir, rotationMat);		// 特定の行列を使用してベクトルを変換する
	return adjustedDirection;
}

void Player::ChangeMovePosition(int num)
{
	// 呼び出された際に引数と現在の移動状態フラグが同じ場合は
	// 同じ処理を行う必要がないのでリターン
	if (m_nowMovingPos == num)return;

	m_touchWall = false;		// 壁に張り付いていない
	m_isJump	= false;		// ジャンプしていない
	m_isCeiling = false;

	/*
	張り付きフラグは、壁とレイが当たった時にtrueになる。
	あたっていない場合はfalseのまま。

	ジャンプフラグは空中にいる間のみfalseでそれ以外はtrueになる。
	*/

	// モデル回転用変数(実際は内積を使用して回転角度を算出し、外積を使用して回転させる方がコードもきれいだし、わかりやすい)
	Math::Vector3	_modelRotate = { 0.0f,0.0f,0.0f };		// 通常の状態
	// この関数が実行されると最初にこの値が入る。
	// この後の条件分岐で必要に応じて値を操作する。

	// 移動状態フラグ切替
	switch (num)
	{
	case 0:
		// オブジェクトの上移動状態
		// 下から上方向への方向ベクトル
		m_nowMovingPos = Position::Ground;
		//m_modelRot = 180.0f;
		//m_modelRot = 0.0f;
		m_gravityDir = Math::Vector3::Down;
		//m_touchWall = false;

		break;

	case 1:
		// 天井にぶら下がっている状態
		// 上から下方向への方向ベクトル
		m_nowMovingPos = Position::Ceiling;
		m_gravityDir = Math::Vector3::Up;
		m_touchWall = true;
		m_isCeiling = true;

		_modelRotate = { 0.0f,180.0f,0.0f };
		break;

	case 2:
		// 側面右
		m_nowMovingPos = Position::Wall_Left;
		m_gravityDir = Math::Vector3::Left;		// 右から左のベクトル
		//m_gravityDir = Math::Vector3::Right;		// 右から左のベクトル
		m_touchWall = true;

		break;

	case 3:
		// 側面左
		m_nowMovingPos = Position::Wall_Right;
		m_gravityDir = Math::Vector3::Right;	// 左から右のベクトル
		//m_gravityDir = Math::Vector3::Left;	// 左から右のベクトル
		m_touchWall = true;

		break;

	case 4:
		// 側面手前
		m_nowMovingPos = Position::Wall_Backward;
		m_gravityDir = Math::Vector3::Backward;
		//m_gravityDir = Math::Vector3::Forward;
		m_touchWall = true;

		break;

	case 5:
		// 側面奥
		m_nowMovingPos = Position::Wall_Forward;
		m_gravityDir = Math::Vector3::Forward;
		//m_gravityDir = Math::Vector3::Backward;
		m_touchWall = true;

		break;

	case 6:
		// 飛行状態
		// 上記のどれにも該当しない
		m_nowMovingPos = Position::Flying;
		//m_modelRot = 180.0f;
		m_gravityDir = Math::Vector3::Down;
		//m_touchWall = false;
		m_isJump = true;

		break;
	}

	// カメラに移動状態を知らせる
	const std::shared_ptr<CameraBase> _spCamera = m_wpCamera.lock();
	if (_spCamera)
	{
		_spCamera->SetMovingPos(m_nowMovingPos);
	}

	if (m_isTitle)
	{
		return;
	}

	// 最終的な回転角度をセットする
	SetModelRotate(_modelRotate);
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

	int num = 0;		// 判定の結果を格納するための変数

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
		_dotProduct = _tmpNormal.Dot(dir);	// 内積値を算出

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
	// =============================
	// 法線取得用レイ(進行方向にやや下方向に飛ばすレイ)
	// =============================
	KdCollider::RayInfo _normalCheckRay;
	_normalCheckRay.m_pos = m_pos;
	//_normalCheckRay.m_dir = AdJustDirection(m_direction, 0.0f);
	//_normalCheckRay.m_dir = AdJustDirection(m_direction, 15.0f);
	//_normalCheckRay.m_dir = AdJustDirection(m_direction, 45.0f);
	//_normalCheckRay.m_dir = GetAdJustedRayDirection(m_direction, m_gravityDir, 2.5f);		// いい感じ
	//_normalCheckRay.m_dir = GetAdJustedRayDirection(m_direction, m_gravityDir, 1.0f);
	_normalCheckRay.m_dir = GetAdJustedRayDirection(m_direction, m_gravityDir, 1.0f);
	//_normalCheckRay.m_dir = CalculateDownwardRayDirection(m_gravityDir, m_direction, 45.0f);
	//_normalCheckRay.m_range = 0.2825f;

	_normalCheckRay.m_range = 1.0f;
	//_normalCheckRay.m_range = 0.35f;			// 敢えて長めのレイを飛ばす

	//_normalCheckRay.m_range = 0.285f;			// おおよそ0.2807f位が地面に接する長さだが、念のため少しだけ長く宣言しておく
	//_normalCheckRay.m_range=0.3f;
	_normalCheckRay.m_type = KdCollider::Type::TypeGround;

	// =============================
	// デバッグ用
	// =============================
	/*m_pDebugWire->AddDebugLine(
		_normalCheckRay.m_pos,
		_normalCheckRay.m_dir,
		_normalCheckRay.m_range,
		kRedColor
	);*/
	// =============================

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
	 //Math::Vector3::Down方向にレイを飛ばす場合(3D空間上で真下にだけレイを飛ばす場合のレイ)
	_rayInfo.m_pos = m_pos;							// レイの発射座標 = プレイヤー座標

	_rayInfo.m_dir = _gravityDir;									// レイを飛ばす方向は重力方向
	_rayInfo.m_range = m_adJustHeight + 0.05f;		// レイの長さはキャラクターのスフィアの半径よりもやや長いレイを飛ばす
	//_rayInfo.m_range = m_gravity + (m_enableStepHeight * 4.0f);		// 重力の強さと自然に飛び越えられる高さ
	_rayInfo.m_type = KdCollider::TypeGround;						// 当たり判定を行うタイプ

	// ==================================
	// デバッグ用
	// ==================================
	/*m_pDebugWire->AddDebugLine
	(
		_rayInfo.m_pos,
		_rayInfo.m_dir,
		_rayInfo.m_range,
		kGreenColor
	);*/
	// ==================================

	std::list<KdCollider::CollisionResult>	_retRayList;		// レイ判定時に当たったオブジェクトを格納するためのリスト

	for (auto& _obj : SceneManager::Instance().GetObjList())
	{
		// オブジェクトリストからレイ情報をもとに、当たっているオブジェクトをリストに格納する
		_obj->Intersects(_rayInfo, &_retRayList);
	}

	//bool			_isHit = false;					// レイに当たったかどうか
	//float			_maxOverLap = 0.0f;						// レイに当たった場所からレイの端っこまでの余分な長さ(この値が大きいほど近くで当たっていることになる)
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
		// この設計だと、レイが当たっていなければ問答無用で重力方向を変更されてしまう

		// 未来座標を使用して未来でも当たり判定が実行されないのか検証
		// 進行方向のベクトルがある場合に限り実行されるものとする
		//if (!FutureGroundCheck())
		//{
		//	ChangeMovePosition(6);			// 未来座標で判定してもオブジェクトにヒットしていなければ、状態を切り替える
		//}
	}
}

bool Player::FutureGroundCheck()
{
	// 進行方向への方向ベクトルがゼロベクトルならリターン
	if (m_moveDir.LengthSquared() == 0.0f)
	{
		return false;
	}

	Math::Vector3	_moveDir	= m_moveDir;		// 移動方向
	Math::Vector3	_gravityDir = m_gravityDir;		// 重力方向

	// 作成した変数を正規化
	_moveDir.Normalize();
	_gravityDir.Normalize();

	// レイの長さ
	Math::Vector3	_rayPos = m_pos + (_moveDir * m_moveSpeed) + (_gravityDir * m_gravity);

	// 作成した変数を使用してレイ情報を設定
	KdCollider::RayInfo	_rayInfo;
	_rayInfo.m_pos		= _rayPos;
	_rayInfo.m_dir		= _moveDir * -1;
	_rayInfo.m_range	= m_adJustHeight + 0.05f;
	_rayInfo.m_type		= KdCollider::TypeGround;
	
	/*
	進行方向に進んで重力方向に移動した座標から進行方向と反対方向にレイを発射している。
	長さは通常の地面を判定する際に使用するレイの長さと同じで、レイと当たり判定を行うオブジェクトタイプも同じ
	*/

	// オブジェクト情報をリセット
	m_wpObject.reset();

	std::list<KdCollider::CollisionResult>	_retRayList;	// ヒットオブジェクト格納用リスト

	// レイ情報をもとにすべてのオブジェクトと当たり判定を行い、ヒットオブジェクトがあればリストに格納する
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->Intersects(_rayInfo, &_retRayList))
		{
			// ヒットオブジェクトが家具タイプであればウィークポイントとしてポインタを保持しておく
			// 保持しておくのは、家具オブジェクトの回転角度を取得るため
			if (obj->GetStageObjeType() == StageObjectType::Furniture)
			{
				m_wpObject = obj;
			}
		}
	}

	Math::Vector3	_groundPos	= {};		// ヒットした座標
	Math::Vector3	_normalVec	= {};		// 法線ベクトル
	float			_maxOverLap = 0.0f;		// 重なり量
	float			_rotY		= 0.0f;		// 家具の回転角度
	bool			_isHit		= false;	// 一番近くでヒットしているオブジェクトを求められたかどうか

	for (auto& ret : _retRayList)
	{
		// 当たり判定の結果、その時点の重なり量よりも重なり量が大きければ
		// 各パラメータを更新する
		if (_maxOverLap < ret.m_overlapDistance)
		{
			_maxOverLap = ret.m_overlapDistance;	// 重なり量更新
			_groundPos	= ret.m_hitPos;				// ヒット座標更新
			_normalVec	= ret.m_normal;				// ヒットオブジェクトの面からの法線ベクトル更新
			_isHit		= true;						// 一番近くでヒットしたオブジェクトが更新されたのでフラグも更新
		}
	}

	if (_isHit)
	{
		_normalVec.Normalize();		// 最終的な法線ベクトルを正規化

		const std::shared_ptr<KdGameObject>	_spObj = m_wpObject.lock();
		
		// アクセスチェックして、ヒットオブジェクトの回転角度を取得
		if (_spObj)
		{
			_rotY = _spObj->GetRotationY();
		}

		// 法線は回転が考慮されていないので、回転を考慮した法線ベクトルに変換する
		CheckMovePosition(_normalVec, _rotY);

		return true;
	}

	return false;
}

void Player::HitSphereCheck()
{
	// =============================
	// スフィア判定
	// =============================
	KdCollider::SphereInfo	_sphere;						// スフィア情報
	//_sphere.m_sphere.Center = m_pos;						// スフィアの中心座標
	
	if (!m_isCeiling)
	{
		_sphere.m_sphere.Center = m_pos;						// スフィアの中心座標
	}
	else
	{
		_sphere.m_sphere.Center = m_pos - Math::Vector3(0.0f, 0.1f, 0.0f);
	}

	//_sphere.m_sphere.Center = m_pos + Math::Vector3{ 0.0f,0.005f,0.0f };						// スフィアの中心座標
	_sphere.m_sphere.Radius = m_adJustHeight;				// スフィアの半径
	_sphere.m_type = KdCollider::Type::TypeGround;

	// ==================================
	// デバッグ用
	// ==================================
	m_pDebugWire->AddDebugSphere
	(
		_sphere.m_sphere.Center,
		_sphere.m_sphere.Radius,
		kBlackColor
	);
	// ==================================

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
	// 食べる処理用の球判定
	KdCollider::SphereInfo	_eatSphere;
	_eatSphere.m_sphere.Center = m_pos;
	_eatSphere.m_sphere.Radius = 0.3f;
	_eatSphere.m_type = KdCollider::TypeGround;

	std::list<KdCollider::CollisionResult>	_retEatSphereList;

	for (auto& _obj : SceneManager::Instance().GetObjList())
	{
		if (_obj->Intersects(_eatSphere, nullptr))
		{
			if (_obj->GetStageObjeType() == StageObjectType::Food)
			{
				// ここいつか変更したい
				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					_obj->SetLife(50);
				}
			}
		}
	}
	/*
	この処理は、自身の持つ一定の大きさのスフィア内に餌がある時に左クリックすれば食事ができるというもの。
	*/
}
