#include "WallRight.h"

void WallRight::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrains/GameScene/wall/Right/wall_right1.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("wallrightCollision", m_spModel, KdCollider::TypeGround);

	}
}
