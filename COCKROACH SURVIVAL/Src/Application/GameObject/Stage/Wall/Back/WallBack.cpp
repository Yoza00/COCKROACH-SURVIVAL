﻿#include "WallBack.h"

void WallBack::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrains/GameScene/wall/back/wall_back1.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("wallbackCollision", m_spModel, KdCollider::TypeGround);
	}
}