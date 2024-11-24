#include "Stage.h"

void Stage::DrawLit()
{
	//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel);

	/*if (m_spTitleHouse)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spTitleHouse);
	}
	else
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spFloor);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWallFront);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWallBack);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWallRight);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWallLeft);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCeiling);
	}*/

	if (!m_spModel)return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

//void Stage::Init()
//{
//	/*m_pCollider = std::make_unique<KdCollider>();
//
//	if (!m_spFloor)
//	{
//		m_spFloor = std::make_shared<KdModelData>();
//		m_spFloor->Load("Asset/Models/Terrains/GameScene/floor/floor1.gltf");
//		m_pCollider->RegisterCollisionShape("floorCollision", m_spFloor, KdCollider::TypeGround);
//	}
//	if (!m_spWallFront)
//	{
//		m_spWallFront = std::make_shared<KdModelData>();
//		m_spWallFront->Load("Asset/Models/Terrains/GameScene/wall/front/wall_front1.gltf");
//		m_pCollider->RegisterCollisionShape("frontCollision", m_spWallFront, KdCollider::TypeGround);
//	}
//	if (!m_spWallBack)
//	{
//		m_spWallBack = std::make_shared<KdModelData>();
//		m_spWallBack->Load("Asset/Models/Terrains/GameScene/wall/back/wall_back1.gltf");
//		m_pCollider->RegisterCollisionShape("backCollision", m_spWallBack, KdCollider::TypeGround);
//	}
//	if (!m_spWallRight)
//	{
//		m_spWallRight = std::make_shared<KdModelData>();
//		m_spWallRight->Load("Asset/Models/Terrains/GameScene/wall/Right/wall_right1.gltf");
//		m_pCollider->RegisterCollisionShape("rightCollision", m_spWallRight, KdCollider::TypeGround);
//	}
//	if (!m_spWallLeft)
//	{
//		m_spWallLeft = std::make_shared<KdModelData>();
//		m_spWallLeft->Load("Asset/Models/Terrains/GameScene/wall/Left/wall_left1.gltf");
//		m_pCollider->RegisterCollisionShape("leftCollision", m_spWallLeft, KdCollider::TypeGround);
//	}
//	if (!m_spCeiling)
//	{
//		m_spCeiling = std::make_shared<KdModelData>();
//		m_spCeiling->Load("Asset/Models/Terrains/GameScene/ceiling/ceiling.gltf");
//		m_pCollider->RegisterCollisionShape("ceilingCollision", m_spCeiling, KdCollider::TypeGround);
//	}
//
//	if (m_spTitleHouse)
//	{
//		m_pCollider->RegisterCollisionShape("TitleCollition", m_spTitleHouse, KdCollider::TypeGround);
//	}*/
//
//	//// ポインタが生成されていない時、生成する
//	//if (!m_spModel)
//	//{
//	//	m_spModel = std::make_shared<KdModelData>();
//
//	//	// コライダー設定
//	//	m_pCollider = std::make_unique<KdCollider>();
//	//	m_pCollider->RegisterCollisionShape("StageCollision", m_spModel, KdCollider::TypeGround);
//	//}
//
//	// 初期化の段階でモデルデータがない場合は早期リターン
//	//if (!m_spModel)return;
//
//	//m_pCollider = std::make_unique<KdCollider>();
//	//m_pCollider->RegisterCollisionShape("ModelCollision", m_spModel, KdCollider::TypeGround);
//}

//void Stage::SetTitleHouse(std::string _filePath)
//{
//	if (!m_spTitleHouse)
//	{
//		m_spTitleHouse = std::make_shared<KdModelData>();
//		m_spTitleHouse->Load(_filePath);
//	}
//}

//void Stage::SetModel(const std::string& _filePath) const
//{
//	// 念のための早期リターン
//	if (!m_spModel)return;			// ポインタが生成されていない場合に実行される
//
//	// 引数で受け取ったパスのモデルをロードする
//	m_spModel->Load(_filePath);
//}

//void Stage::SetModel(std::string _filePath)
//{
//	// モデルデータがロードされていない場合に実行される
//	if (!m_spModel)
//	{
//		m_spModel = std::make_shared<KdModelData>();
//		m_spModel->Load(_filePath);
//	}
//}
