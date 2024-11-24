#include "WallLeft.h"

void WallLeft::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrains/GameScene/wall/Left/wall_left1.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("wallleftCollision", m_spModel, KdCollider::TypeGround);

	}
}
