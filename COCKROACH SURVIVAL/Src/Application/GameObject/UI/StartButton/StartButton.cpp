#include "StartButton.h"

void StartButton::Update()
{
	if (!m_spTex)return;

	m_angle += 0.1f;
	if (m_angle >= 360.0f)
	{
		m_angle -= 360.0f;
	}

	m_drawPos.y += sin(m_angle) * m_moveSpd;
}