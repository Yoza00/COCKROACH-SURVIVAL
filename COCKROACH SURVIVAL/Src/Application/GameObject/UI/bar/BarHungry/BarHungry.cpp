#include "BarHungry.h"
#include"../../../../GameObject/Character/Player/Player.h"

void BarHungry::Update()
{
	if (!m_spTex)return;

	// プレイヤーのウィークポインタが有効かどうかをチェック
	std::shared_ptr<Player>	_spPlyer = m_wpPlayer.lock();

	if (_spPlyer)
	{
		// 空腹度残量を取得
		m_restNum = _spPlyer->GetRestNum();
	}
}

void BarHungry::DrawSprite()
{
	if (!m_spTex)return;
	
	// 切り取り範囲を更新
	m_rec = { long(m_startPos.x),long(m_startPos.y),long(m_singleW * m_restNum),long(m_height) };

	// 更新された切り取り範囲を利用して描画
	KdShaderManager::Instance().m_spriteShader.
		DrawTex(m_spTex, static_cast<int>(m_drawPos.x), static_cast<int>(m_drawPos.y), m_rec.width, m_rec.height, &m_rec, nullptr, { 0.0f,0.5f });
}

void BarHungry::Init()
{
	UI::Init();

	m_restNum = 100.0f;
	m_singleW = m_width / 100.0f;	// 画像の幅の1/100
}
