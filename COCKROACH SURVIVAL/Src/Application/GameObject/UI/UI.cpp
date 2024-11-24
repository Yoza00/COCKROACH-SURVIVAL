#include "UI.h"

void UI::DrawSprite()
{
	if (!m_spTex)return;

	// 値が最小の時に表示する幅
	//float _width = m_width / 100.0f;		// 画像の幅の1/100サイズ

	// 切り取り範囲指定
	//Math::Rectangle _rec = { (long)m_startPos.x,(long)m_startPos.y,long(_width * m_width),(long)m_height };
	Math::Rectangle _rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_width),(long)m_height };

	// 描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_drawPos.x, m_drawPos.y, _rec.width, _rec.height, &_rec, nullptr, { 0.0f,0.5f });
}

void UI::Init()
{
	if (!m_spTex)
	{
		m_spTex = std::make_shared<KdTexture>();
		//m_spTex->Load(m_filePath);
		m_spTex = KdAssets::Instance().m_textures.GetData(m_filePath);

		// テクスチャの幅・高さ情報取得
		m_width = m_spTex->GetWidth();
		m_height = m_spTex->GetHeight();
	}
}
