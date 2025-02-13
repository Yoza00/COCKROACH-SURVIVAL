#include "Score.h"

#include"../../../GameObject/Character/Player/Player.h"

void Score::Update()
{
	if (!m_spTex)return;

	// 表示限界がきている場合処理しない
	if (m_isMaxValue)return;

	const std::shared_ptr<Player>	_spPlayer = m_wpPlayer.lock();
	
	if (!_spPlayer)return;

	// スコア表示の切り替え
	// 現在のスコアと以前のスコアを比較して値が異なる場合に
	// 切り取り範囲の更新処理を行う
	int	_currentScore = _spPlayer->GetScore();

	if (_currentScore == m_oldScore)return;

	UpdateRectangle(_currentScore);
}

void Score::DrawSprite()
{
	if (!m_spTex)return;

	// 描画
	KdShaderManager::Instance().m_spriteShader.
		DrawTex(m_spTex, static_cast<int>(m_drawPos.x + (m_index * m_charaWidthSize)), static_cast<int>(m_drawPos.y), m_rec.width, m_rec.height, &m_rec);
}

void Score::Init()
{
	if (!m_spTex)
	{
		m_spTex = std::make_shared<KdTexture>();
		m_spTex = KdAssets::Instance().m_textures.GetData(m_filePath);

		// テクスチャの幅・高さ情報取得
		m_width = static_cast<float>(m_spTex->GetWidth());
		m_height = static_cast<float>(m_spTex->GetHeight());
	}

	m_startPos.x = 675.0f;

	m_charaWidthSize = m_width / 10.0f;

	// 切り取り範囲の設定
	m_rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_charaWidthSize),long(m_height) };
}

void Score::SetIndex(int index)
{
	// セットされる指数の値を制御
	// MAX_INDEXよりも値の大きいものは入らないように制御
	if (index > MAX_INDEX)return;

	m_index = index;
}

void Score::UpdateRectangle(int newScore)
{
	// 古いスコアを更新しておく
	m_oldScore		= newScore;

	// 念のための制御
	// 表示限界(10000)以上にならないように制御
	if (newScore > Maximum_Value)
	{
		newScore		= Maximum_Value;
		m_isMaxValue	= true;
	}

	// 表示中の値を更新
	m_DrawingScore	= newScore;

	// 位の数を求める
	// 10~nのn(指数)を求める
	int _index		= m_index - MAX_INDEX;
	int _absIndex	= abs(_index);

	// 各桁の最小値を求める
	int _Max = 1;
	for (int cnt = 0; cnt < _absIndex; cnt++)
	{
		_Max *= TEN_BASE;
	}

	// スコアを各桁の最小値で割る
	newScore /= _Max;
	 
	// 10以上の場合は、10で割った余りを算出する
	if (newScore >= 10)
	{
		newScore %= TEN_BASE;
	}

	// 負の値でないことを確定させておく
	if (newScore >= 0)
	{
		UpdateStartPosX(newScore);
	}
}

void Score::UpdateStartPosX(int index)
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

	// 切り取り範囲の設定
	m_rec = { (long)m_startPos.x,(long)m_startPos.y,long(m_charaWidthSize),long(m_height) };
}
