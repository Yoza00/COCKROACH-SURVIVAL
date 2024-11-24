#pragma once
#include"../../../GameObject/Stage/Stage.h"

class Floor :public Stage
{
public:

	Floor() {}
	~Floor()override {}

	void Init()override;

private:

	std::vector<Math::Vector3>	m_norPos;
	Math::Vector3				m_normalPos;
};