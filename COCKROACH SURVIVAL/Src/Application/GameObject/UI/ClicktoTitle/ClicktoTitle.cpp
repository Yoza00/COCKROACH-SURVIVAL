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
	if (!m_isDraw)return;
	if (!m_isActive)return;

	Math::Color		_col = { 1.0f,1.0f,1.0f,m_alpha };

	Math::Rectangle _rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_width),(long)m_height };

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_drawPos.x, m_drawPos.y, _rec.width, _rec.height, &_rec, &_col, { 0.0f,0.5f });
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
