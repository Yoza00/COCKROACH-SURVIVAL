#include "ClicktoTitle.h"

void ClicktoTitle::Update()
{
	if (!m_spTex)return;
	
	if (!m_isActive)
	{
		m_timeToActive--;

		if (m_timeToActive <= 0)
		{
			m_isActive = true;
		}

		return;
	}

	AlphaUpdate();
}

void ClicktoTitle::DrawSprite()
{
	if (!m_spTex)return;
	if (!m_isActive)return;

	Math::Color		_col = { 1.0f,1.0f,1.0f,m_alpha };

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, static_cast<int>(m_drawPos.x), static_cast<int>(m_drawPos.y), m_rec.width, m_rec.height, &m_rec, &_col, { 0.0f,0.5f });
}

void ClicktoTitle::AlphaUpdate()
{
	if (m_isAlphaIncrease)
	{
		m_alpha += m_alphaIncreasePow;

		if (m_alpha >= 1.0f)
		{
			m_isAlphaIncrease = false;
		}
	}
	else
	{
		m_alpha -= m_alphaIncreasePow;

		if (m_alpha <= 0.1f)
		{
			m_isAlphaIncrease = true;
		}
	}
}
