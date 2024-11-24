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
	//void SetModel(const std::string& _filePath)const;
	//void SetModel(std::string	_filePath);

	//void SetTitleHouse(std::string	_filePath);

	// 引数のパスのモデルをロードする
	virtual void SetModel(std::string	_filePath) {}

	// 引数のパスのモデルをロードし、任意の場所に任意の大きさで出現させる
	virtual void SetState(const std::string	_filePath,  const Math::Vector3	_pos, const float	_scale, const int	_number) {}

protected:

	// モデルデータ
	//std::shared_ptr<KdModelData>		m_spModel = nullptr;

	//std::shared_ptr<KdModelData> m_spTitleHouse = nullptr;

	/*std::shared_ptr<KdModelData> m_spFloor		= nullptr;
	std::shared_ptr<KdModelData> m_spWallRight	= nullptr;
	std::shared_ptr<KdModelData> m_spWallLeft	= nullptr;
	std::shared_ptr<KdModelData> m_spWallFront	= nullptr;
	std::shared_ptr<KdModelData> m_spWallBack	= nullptr;
	std::shared_ptr<KdModelData> m_spCeiling	= nullptr;*/

	std::shared_ptr<KdModelData>	m_spModel = nullptr;
};