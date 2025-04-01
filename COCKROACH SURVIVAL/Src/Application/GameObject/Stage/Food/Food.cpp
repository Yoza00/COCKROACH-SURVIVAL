#include "Food.h"

#include"../../../Scene/SceneManager.h"

#include"../Floor/Floor.h"
#include"../../../GameObject/UI/Bar/BarHungry/BarHungry.h"
#include"../../../GameObject/Character/Player/Player.h"

void Food::Update()
{
	if (!m_spModel)return;

	// 生存時間の更新
	m_lifeSpan--;					// 生存時間を更新(どんどん減少させていく)
	
	// 生存時間がなくなる or プレイヤーが食べきったら、必要な処理を行い、このオブジェクトを削除する
	if (m_lifeSpan < 0)
	{
		m_isExpired = true;
	}
	else if (m_life <= 0)
	{
		const std::shared_ptr<Player>	_spPlayer = m_wpPlayer.lock();
		if (_spPlayer)
		{
			// プレイヤーが食べきったら、食べ物のデータをもとに、プレイヤーに情報を知らせる
			_spPlayer->SetAteFood(m_parm.m_addLife, m_parm.m_addRest, m_parm.m_score);
		}

		m_isExpired = true;
	}

	Math::Matrix	_transMat = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = _transMat;
}

void Food::PostUpdate()
{
	// 描画するモデルが存在しない or 重力による移動が無効状態のときは、以下の処理を実行しない
	if (!m_spModel/* || !m_isMove*/)return;

	// レイ判定(家具もしくは地面と当たり判定を行う(乗れる状態にあるオブジェクトすべて))
	KdCollider::RayInfo	_rayInfo;
	_rayInfo.m_pos		= m_pos;
	_rayInfo.m_dir		= Math::Vector3::Down;
	_rayInfo.m_range	= 0.00005f;
	_rayInfo.m_type		= KdCollider::TypeGround;

	// リストを作成
	std::list<KdCollider::CollisionResult>	_retRayList;

	for (auto& _obj : SceneManager::Instance().GetObjList())
	{
		_obj->Intersects(_rayInfo, &_retRayList);
	}

	bool			_isHit		= false;
	float			_maxOverLap = 0.0f;
	Math::Vector3	_groundPos	= {};

	for (auto& _ret : _retRayList)
	{
		if (_maxOverLap < _ret.m_overlapDistance)
		{
			_maxOverLap = _ret.m_overlapDistance;
			_groundPos	= _ret.m_hitPos;
			_isHit		= true;
		}
	}

	if (_isHit)
	{
		m_pos		= _groundPos;
		m_gravity	= 0.0f;
	}
}

void Food::Init()
{
	m_lifeSpan		= m_maxLifeSpan;	// 生存(起動)時間 およそ50秒
	m_life			= m_lifeValue;		// 残り体力(この値が0になる = 食べきった)

	m_stageObjeType = StageObjectType::Feed;
}

void Food::SetState(const std::string& _filePath, const Math::Vector3& _pos, const float _scale, const int _number)
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel = KdAssets::Instance().m_modeldatas.GetData(_filePath);

		m_pos = _pos;

		Math::Matrix	_scaleMat = Math::Matrix::CreateScale(_scale);
		Math::Matrix	_transMat = Math::Matrix::CreateTranslation(_pos);
		m_mWorld = _scaleMat * _transMat;

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("FoodCollision" + std::to_string(_number), m_spModel, KdCollider::TypeBump);
	}
}

void Food::SetLife(const int _life)
{
	// 呼び出されるとライフを削除する
	m_life -= _life;
}

void Food::SetParm(float life, float rest, float score)
{
	m_parm.m_addLife	= life;
	m_parm.m_addRest	= rest;
	m_parm.m_score		= static_cast<int>(score);
}
