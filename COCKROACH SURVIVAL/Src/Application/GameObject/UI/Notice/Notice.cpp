#include "Notice.h"

#include"../../../Scene/SceneManager.h"

void Notice::Update()
{
	if (!m_spTex)
	{
		return;
	}

	// カウンタが無くなり次第、インスタンス破棄フラグを起動する
	// 次フレームの前更新の際に、インスタンスが破棄される
	m_incetanceDeleteLimit--;
	if (m_incetanceDeleteLimit < 0)
	{
		m_isExpired = true;
		return;
	}
}

void Notice::DrawSprite()
{
	if (!m_spTex)return;

	// 描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, static_cast<int>(m_drawPos.x), static_cast<int>(m_drawPos.y), m_rec.width, m_rec.height, &m_rec);
}
