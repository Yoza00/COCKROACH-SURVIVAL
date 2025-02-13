#include "KdGameObject.h"

void KdGameObject::DrawDebug()
{
	// 早期リターン
	if (!m_pDebugWire)return;

	m_pDebugWire->Draw();
}

void KdGameObject::SetScale(float scalar)
{
	Math::Vector3 scale(scalar);

	SetScale(scale);
}

void KdGameObject::SetScale(const Math::Vector3& scale)
{
	Math::Vector3 vecX = m_mWorld.Right();
	Math::Vector3 vecY = m_mWorld.Up();
	Math::Vector3 vecZ = m_mWorld.Backward(); 
	
	vecX.Normalize();
	vecY.Normalize();
	vecZ.Normalize();

	m_mWorld.Right(vecX * scale.x);
	m_mWorld.Up(vecY * scale.y);
	m_mWorld.Backward(vecZ * scale.z);
}

Math::Vector3 KdGameObject::GetScale() const
{
	return Math::Vector3(m_mWorld.Right().Length(), m_mWorld.Up().Length(), m_mWorld.Backward().Length());
}

void KdGameObject::CalcDistSqrFromCamera(const Math::Vector3& camPos)
{
	m_distSqrFromCamera = (m_mWorld.Translation() - camPos).LengthSquared();
}

bool KdGameObject::Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetShape, m_mWorld, pResults);
}

bool KdGameObject::Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetBox, m_mWorld, pResults);
}

bool KdGameObject::Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetShape, m_mWorld, pResults);
}

void KdGameObject::SetRotationY(float rotationY)
{
	m_rotationY = rotationY;
}

const float KdGameObject::CalcDotValue(const Math::Vector3& currentVec, const Math::Vector3& toVec) const
{
	float	_dotValue = currentVec.Dot(toVec);

	// 値が範囲外に行かないように制御しておく
	static const float	m_clampMinValue = -1.0f;		// 範囲の最小値
	static const float	m_clampMaxValue = 1.0f;			// 範囲の最大値

	_dotValue = std::clamp(_dotValue, m_clampMinValue, m_clampMaxValue);

	return _dotValue;
}

Math::Vector3 KdGameObject::CalcCrossValue(const Math::Vector3& currentvec, const Math::Vector3& toVec) const
{
	return	Math::Vector3(toVec.Cross(currentvec));
}
