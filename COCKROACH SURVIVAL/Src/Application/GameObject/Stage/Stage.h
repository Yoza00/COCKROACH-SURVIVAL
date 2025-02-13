#pragma once

class Stage :public KdGameObject
{
public:

	Stage()								{}
	~Stage()				override	{}

	void Update()					{}
	void DrawLit()			override;
	void Init()				override {}

	// 引数のパスのモデルをロードする
	virtual void SetModel(const std::string&) {}

	// 引数のパスのモデルをロードし、任意の場所に任意の大きさで出現させる
	virtual void SetState(const std::string&, const Math::Vector3&, const float, const int) {}

protected:

	std::shared_ptr<KdModelData>	m_spModel = nullptr;
};