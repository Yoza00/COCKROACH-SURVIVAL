#include "FoodBarHP.h"

#include"../../../Stage/Food/Food.h"
#include"../../../Camera/TPSCamera/TPSCamera.h"
#include"../../../Character/Player/Player.h"

void FoodBarHP::DrawSprite()
{
	if (!m_spTex)return;

	if (CheckDistanceFromPlayerToFeed() == false)
	{
		return;
	}

	// 座標変換の結果を格納するための変数
	Math::Vector3	_result = Math::Vector3::Zero;

	std::shared_ptr<TPSCamera>	_spTPSCamera = m_wpCamera.lock();
	if (!_spTPSCamera)return;

	std::shared_ptr<KdCamera>	_spCamera = _spTPSCamera->GetCamera();
	if (!_spCamera)return;

	Math::Vector3	_foodPos = Math::Vector3::Zero;

	// 対象となる餌の座標から表示する予定の座標を算出
	{
		std::shared_ptr<Food>	_spFood = m_wpTarget.lock();
		if (!_spFood)return;

		_foodPos = _spFood->GetPos();

		// 座標を補正(少し上方向に移動)
		_foodPos.y += 0.5f;

		m_limit = _spFood->GetLife();
	}

	// 最終的な値を変換する
	_spCamera->ConvertWorldToScreenDetail(_foodPos, _result);

	m_rec = { long(m_startPos.x),long(m_startPos.y),long(m_singleW * m_limit),long(m_height) };

	// 最終結果を使用して描画
	KdShaderManager::Instance().m_spriteShader.
		DrawTex(m_spTex, static_cast<int>(_result.x), static_cast<int>(_result.y), m_rec.width, m_rec.height, &m_rec, nullptr, { 0.0f,0.5f });
}

void FoodBarHP::Init()
{
	if (!m_spTex)
	{
		m_spTex = std::make_shared<KdTexture>();
		m_spTex = KdAssets::Instance().m_textures.GetData(m_filePath);

		// テクスチャの幅・高さ情報取得
		m_width		= static_cast<float>(m_spTex->GetWidth());
		m_height	= static_cast<float>(m_spTex->GetHeight());
	}

	m_limit		= 100;
	m_singleW	= (static_cast<int>(m_width) / 100);	// 個の変数の中に、画像サイズの横幅の1/100の大きさの値が入る
}

bool FoodBarHP::CheckDistanceFromPlayerToFeed()
{
	Math::Vector3	_foodPos	= Math::Vector3::Zero;
	Math::Vector3	_playerPos	= Math::Vector3::Zero;

	float			_canEatDistance = 0.0f;

	{
		std::shared_ptr<Food>	_spFood		= m_wpTarget.lock();
		std::shared_ptr<Player>	_spPlayer	= m_wpPlayer.lock();

		if (!_spFood || !_spPlayer)
		{
			// 処理の続行が出来ないので、破棄フラグを起動しておく
			// 次のフレームのPreUpdate()で削除される
			m_isExpired = true;

			return false;
		}

		// オブジェクトの座標をコピー
		_foodPos	= _spFood->GetPos();
		_playerPos	= _spPlayer->GetPos();

		// 食事可能な範囲を取得
		_canEatDistance = _spPlayer->GetCanEatDistance();

		// 補正値を掛け合わせて描画距離を作成
		_canEatDistance *= m_correctionValue;
	}

	// 距離計算
	Math::Vector3	_distance = _foodPos - _playerPos;

	// 一定距離にいるかどうか
	if (_distance.Length() > _canEatDistance)
	{
		return false;
	}

	return true;
}
