#include "House.h"

void House::Init()
{
	if (m_spModel)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("HouseCollision", m_spModel, KdCollider::TypeGround);
	}
}

void House::SetModel(std::string _filePath)
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load(_filePath);
	}
}
