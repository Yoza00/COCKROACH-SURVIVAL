#include"ObjectData.h"

void ObjectData::Update()
{
	if (!m_spModel)return;

	Math::Matrix	_scaleMat	= Math::Matrix::CreateScale(m_scale);
	Math::Matrix	_rotMat		= GetRotateMatrix();
	Math::Matrix	_transMat	= Math::Matrix::CreateTranslation(m_pos);

	m_mWorld = _scaleMat * _rotMat * _transMat;
}

void ObjectData::PostUpdate()
{
	KdCollider::SphereInfo	_sphere;
	_sphere.m_sphere.Center = m_pos;
	_sphere.m_sphere.Radius = m_scale;
}

void ObjectData::DrawLit()
{
	if (!m_spModel)return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void ObjectData::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();	// インスタンスを作成
		m_spModel->SetModelData(m_filePath);			// モデル読込

		// コライダー設定
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape(m_name, m_spModel, KdCollider::Type::TypeGround);

		// 生成されるオブジェクトは家具である
		m_stageObjeType = StageObjectType::Furniture;
	}
}
