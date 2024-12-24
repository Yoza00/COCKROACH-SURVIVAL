#include "Floor.h"

void Floor::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		//m_spModel->Load("Asset/Models/Terrains/GameScene/back_ground/back_ground.gltf");
		//m_spModel->Load("Asset/Models/Terrains/GameScene/floor/floor1.gltf");
		//m_spModel->Load("Asset/Models/Terrains/GameScene/floor/stage_tmp3.gltf");
		//m_spModel->Load("Asset/Models/Terrains/GameScene/floor/stage_tmp4.gltf");
		m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrains/GameScene/floor/stage_model7.gltf");
		//m_spModel->Load("Asset/Models/Terrains/GameScene/floor/stage_model7.gltf");
		//m_spModel->Load("Asset/Models/Terrains/GameScene/floor/stage_model6.gltf");
		//m_spModel->Load("Asset/Models/Terrains/GameScene/floor/stage_model5.gltf");
		//m_spModel->Load("Asset/Models/Terrains/GameScene/floor/floor_tmp.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("floorCollision", m_spModel, KdCollider::TypeGround);

		// このオブジェクトはマップである
		m_stageObjeType = StageObjectType::Map;
	}
}
