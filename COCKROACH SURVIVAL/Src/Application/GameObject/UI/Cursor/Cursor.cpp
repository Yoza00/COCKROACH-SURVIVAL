#include "Cursor.h"

void Cursor::Update()
{
	// メニュー状態 あるいは 表示しない場合はリターン
	if (!m_isMenu || !m_isDraw)return;

	// マウス座標を取得
	GetCursorPos(&m_mousePos);

	// マウス座標をスクリーンのマウスの有効範囲内での座標に変換する
	ScreenToClient(GetActiveWindow(), &m_mousePos);

	m_mousePos.x = m_mousePos.x - 640.0f;
	m_mousePos.y = 360.0f - m_mousePos.y;
}

void Cursor::DrawSprite()
{
	// メニュー状態 あるいは 表示しない場合はリターン
	if (!m_isMenu || !m_isDraw)return;

	// 切り取り範囲指定
	Math::Rectangle _rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_width),(long)m_height };

	// 描画	
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_mousePos.x, m_mousePos.y, _rec.width, _rec.height, &_rec, nullptr, { 0.0f,1.0f });
}
