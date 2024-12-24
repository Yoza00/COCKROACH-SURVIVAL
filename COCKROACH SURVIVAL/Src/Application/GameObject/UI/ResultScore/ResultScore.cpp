#include "ResultScore.h"

void ResultScore::Update()
{
	if (!m_spTex)return;
	if (m_isUpdateRectangleFinished)return;

	// 切り取り
	UpdateRectangle();
}

void ResultScore::DrawSprite()
{
	if (!m_spTex)return;

	// 切り取り範囲の設定
	Math::Rectangle _rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_charaWidthSize),long(m_height) };

	// 描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_drawPos.x - (m_index * m_charaWidthSize), m_drawPos.y, _rec.width, _rec.height, &_rec, nullptr, { 0.5f,0.5f });
}

void ResultScore::Init()
{
	UI::Init();

	m_charaWidthSize = m_width / 10.0f;
}

void ResultScore::UpdateRectangle()
{
	// 10^indexで考える
	// 0=1の位,1=10の位,2=100の位,...
	// 各位の最小値を求める
	int _Max = 1;
	for (int cnt = 0; cnt < m_index; cnt++)
	{
		// 10倍増加させることで、桁が１つ上がる
		_Max *= 10;
	}

	// 位の最小値を求めた結果、１桁か同課を判断し、２桁以上であれば余りを算出する
	// 最終的に求められた値を使用して、切り取り範囲の切り取り開始座標を更新する
	m_drawScore /= _Max;

	if (m_drawScore >= 10)
	{
		m_drawScore %= 10;
	}

	// 負の数でないことを確定させておく
	if (m_drawScore >= 0)
	{
		ChangeRectanglePosX(m_drawScore);
	}
}

void ResultScore::ChangeRectanglePosX(int index)
{
	// 各値の切り取り開始X座標を更新する(0∼9)
	// 画像の数字の並びは1,2,3,4,5,6,7,8,9,0
	// 0以外の数字はすべて1文字ずつ前に来ていて、0のみ9つ後ろに移動している
	// 0のみ最大値から1文字分のサイズを引けば求められる
	if (index == 0)
	{
		// 横幅から1文字分の横幅文手前側に移動させる
		m_startPos.x = m_width - m_charaWidthSize;
	}
	else if (index < 10)
	{
		m_startPos.x = m_charaWidthSize * (index - 1);
	}
	else
	{
		// 表示なし
		m_startPos.x = m_width;
	}

	m_isUpdateRectangleFinished = true;
}
