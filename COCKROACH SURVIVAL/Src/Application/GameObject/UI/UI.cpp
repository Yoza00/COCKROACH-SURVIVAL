#include "UI.h"

void UI::DrawSprite()
{
	if (!m_spTex)return;

	// 描画フラグチェック
	if (!m_isDraw)return;

	// 切り取り範囲指定
	Math::Rectangle _rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_width),(long)m_height };

	// 描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_drawPos.x, m_drawPos.y, _rec.width, _rec.height, &_rec, nullptr, { 0.0f,0.5f });
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
		m_width = m_spTex->GetWidth();
		m_height = m_spTex->GetHeight();
	}
}
