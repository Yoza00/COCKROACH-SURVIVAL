#include "Ceiling.h"

void Ceiling::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrains/GameScene/ceiling/ceiling.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("ceilingCollision", m_spModel, KdCollider::TypeGround);

	}
}
