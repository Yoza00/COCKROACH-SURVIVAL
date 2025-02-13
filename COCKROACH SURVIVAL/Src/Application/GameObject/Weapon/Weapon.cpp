#include "Weapon.h"
#include"../Character/Enemy/Enemy.h"

void Weapon::Update()
{
	if (!m_isModelDraw)
	{
		return;
	}

	if (!m_spModel)
	{
		return;
	}

	// 行列計算
	{
		const std::shared_ptr<Enemy>	_spParent = m_wpParent.lock();
		
		if (!_spParent)
		{
			return;
		}

		Math::Vector3	_parentPos = _spParent->GetRightHandPos();

		Math::Matrix		_transMat = Math::Matrix::CreateTranslation(_parentPos);
		m_mWorld = _transMat;
	}

	{
		const KdModelWork::Node* _pNozzlePoint = m_spModel->FindNode("nozzlePoint");
		if (_pNozzlePoint)
		{
			m_nozzleMat = _pNozzlePoint->m_worldTransform;
		}

		m_nozzleMat = (m_nozzleMat * m_mWorld);
	}
}

void Weapon::DrawLit()
{
	if (!m_isModelDraw)
	{
		return;
	}

	if (!m_spModel)
	{
		return;
	}

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Weapon::GenerateDepthMapFromLight()
{
	if (!m_isModelDraw)
	{
		return;
	}

	if (!m_spModel)
	{
		return;
	}

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Weapon::Init()
{
	if (!m_spModel)
	{
		m_spModel	= std::make_shared<KdModelWork>();
		*m_spModel	= KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Weapon/spray.gltf");
	}

	const KdModelWork::Node* _pNozzlePoint = m_spModel->FindNode("nozzlePoint");
	if (_pNozzlePoint)
	{
		m_nozzleMat = _pNozzlePoint->m_worldTransform;
	}
}
