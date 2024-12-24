#include "ResultScore.h"

void ResultScore::Update()
{
	if (!m_spTex)return;
}

void ResultScore::DrawSprite()
{
	if (!m_spTex)return;

	// 切り取り範囲の設定
	Math::Rectangle _rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_charaWidthSize),long(m_height) };

	// 描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_drawPos.x + (m_index * m_charaWidthSize), m_drawPos.y, _rec.width, _rec.height, &_rec, nullptr, { 0.5f,0.5f });
}

void ResultScore::Init()
{
	UI::Init();

	m_charaWidthSize = m_width / 10.0f;
}
