#include "BarHp.h"

#include"../../../../GameObject/Character/Player/Player.h"

void BarHp::Update()
{
	if (!m_spTex)return;

	// プレイヤーのウィークポインタが有効かどうかをチェック
	const std::shared_ptr<Player>	_spPlyer = m_wpPlayer.lock();

	if (_spPlyer)
	{
		// HP残量を取得
		m_life = _spPlyer->GetHP();
	}
}

void BarHp::DrawSprite()
{
	if (!m_spTex)return;

	// 切り取り範囲を更新
	Math::Rectangle	_rec = { long(m_startPos.x),long(m_startPos.y),long(m_singleW * m_life),long(m_height) };

	// 切り取り範囲をもとに描画
	// 更新された切り取り範囲を利用して描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_drawPos.x, m_drawPos.y, _rec.width, _rec.height, &_rec, nullptr, { 0.0f,0.5f });
}

void BarHp::Init()
{
	UI::Init();

	m_life		= 100;
	m_singleW	= m_width / 100.0f;
}
