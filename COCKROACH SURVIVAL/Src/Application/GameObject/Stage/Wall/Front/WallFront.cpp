#include "WallFront.h"

void WallFront::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrains/GameScene/wall/front/wall_front1.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("wallfrontCollision", m_spModel, KdCollider::TypeGround);

	}
}
