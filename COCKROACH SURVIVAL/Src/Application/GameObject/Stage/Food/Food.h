#pragma once

#include"../Stage.h"

class Player;

class Food :public Stage
{
public:

	Food() {}
	~Food() override {}

	// 食べ物のステータス等をまとめた構造体
	struct FoodParm
	{
		// 取り合えず、生存時間は無視
		float	m_addLife;		// 加算する体力
		float	m_addRest;		// 加算する満腹度
		int		m_score;		// 加算するスコア
	};

	void Update()override;
	void PostUpdate()override;
	void Init()override;
	void SetState(const std::string	_filePath, const Math::Vector3	_pos, float	_scale, int _number)override;

	Math::Vector3 GetPos() const { return m_pos; }

	void SetLife(const int	_life)override;

	void SetPlayer(const std::shared_ptr<Player>& _spPlayer)
	{
		m_wpPlayer = _spPlayer;
	}

	// 食べ物のパラメータをセット
	void SetParm(float life, float rest, float score);
	// ここを完成状態のときに、jsonファイルから読み込んで自動的にセットするようにしておく
private:

	std::weak_ptr<Player>	m_wpPlayer;

	Math::Vector3	m_pos = Math::Vector3::Zero;
	float			m_gravity = 0.0f;
	//bool			m_isMove = true;					// 着地後は落下処理はしない

	// 存在に関係するもの
	// 生成されてから一定時間経過後にステージに残っている。もしくは、プレイヤーが食事することでライフがなくなることで消える
	int				m_lifeSpan = 0.0f;					// 存在時間
	int				m_life = 0.0f;						// ライフ(０になると消える)

	FoodParm		m_parm;
};