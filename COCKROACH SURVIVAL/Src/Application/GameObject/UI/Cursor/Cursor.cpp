#include "Cursor.h"

void Cursor::Update()
{
	// メニュー状態 あるいは 表示しない場合はリターン
	if (!m_isMenu)return;

	// マウス座標を取得
	GetCursorPos(&m_mousePos);

	// マウス座標をスクリーンのマウスの有効範囲内での座標に変換する
	ScreenToClient(GetActiveWindow(), &m_mousePos);

	m_mousePos.x = static_cast<long>(m_mousePos.x - 640.0f);
	m_mousePos.y = static_cast<long>(360.0f - m_mousePos.y);
}

void Cursor::DrawSprite()
{
	// メニュー状態 あるいは 表示しない場合はリターン
	if (!m_isMenu)return;

	// 描画	
	KdShaderManager::Instance().m_spriteShader.
		DrawTex(m_spTex, static_cast<int>(m_mousePos.x), static_cast<int>(m_mousePos.y), m_rec.width, m_rec.height, &m_rec, nullptr, { 0.0f,1.0f });
}
