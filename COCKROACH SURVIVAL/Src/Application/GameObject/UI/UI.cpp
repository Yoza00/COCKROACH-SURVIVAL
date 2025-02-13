#include "UI.h"

void UI::DrawSprite()
{
	if (!m_spTex)return;

	// 描画
	KdShaderManager::Instance().m_spriteShader.
		DrawTex(m_spTex, static_cast<int>(m_drawPos.x), static_cast<int>(m_drawPos.y), m_rec.width, m_rec.height, &m_rec, nullptr, { 0.0f,0.5f });
	// 引数は
	// 画像データ, 描画座標(x), 描画座標(y), 切り取り範囲の横幅, 切り取り範囲の縦幅, 切り取り範囲, 色, 原点の座標
}

void UI::Init()
{
	if (!m_spTex)
	{
		m_spTex = std::make_shared<KdTexture>();
		m_spTex = KdAssets::Instance().m_textures.GetData(m_filePath);

		// テクスチャの幅・高さ情報取得
		m_width		= static_cast<float>(m_spTex->GetWidth());
		m_height	= static_cast<float>(m_spTex->GetHeight());

		// 切り取り範囲指定
		m_rec = { (long)m_startPos.x,(long)m_startPos.y,(long)m_width,(long)m_height };
	}
}
