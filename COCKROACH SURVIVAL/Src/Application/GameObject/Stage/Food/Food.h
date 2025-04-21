#pragma once

#include"../Stage.h"

class Player;

class Food :public Stage
{
public:

	Food()							{}
	~Food()				override	{}

	// 食べ物のステータス等をまとめた構造体
	struct FoodParm
	{
		float	m_addLife	= 0.0f;	// 加算する体力
		float	m_addRest	= 0.0f;	// 加算する満腹度
		int		m_score		= 0;	// 加算するスコア
	};

	void Update()		override;
	void PostUpdate()	override;
	void Init()			override;
	void SetState(const std::string& _filePath, const Math::Vector3& _pos, const float _scale, const int _number)	override;

	Math::Vector3 GetPos() const { return m_pos; }

	void SetLife(const int	_life)override;

	void SetPlayer(const std::shared_ptr<Player>& _spPlayer)
	{
		m_wpPlayer = _spPlayer;
	}

	// 食べ物のパラメータをセット
	void SetParm(float life, float rest, float score);

	const int GetLife()const { return m_life; }

private:

	std::weak_ptr<Player>	m_wpPlayer;

	const int		m_lifeValue		= 100;		// 体力
	const int		m_maxLifeSpan	= 3000;		// 消えるまでのタイムリミット

	Math::Vector3	m_pos		= Math::Vector3::Zero;

	// 存在に関係するもの
	// 生成されてから一定時間経過後にステージに残っている。もしくは、プレイヤーが食事することでライフがなくなることで消える
	int				m_lifeSpan	= 0;	// 存在時間
	int				m_life		= 0;	// ライフ(０になると消える)

	float			m_gravity	= 0.0f;

	FoodParm		m_parm;
};