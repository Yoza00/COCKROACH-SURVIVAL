#pragma once

#include"Json/nlohmann/json.hpp"

struct Position
{
	float x;
	float y;
	float z;
};

struct Object
{
	std::string m_uiType;
	std::string	m_name;
	std::string	m_filePath;
	Position	m_pos;
	float		m_rot;
	float		m_scale;
};

class ObjectManager
{
public:

	// ====================================================
	//セッター類
	// ====================================================
	void SaveJsonData(const nlohmann::json& _jsonData, const std::string& _filePath);	// JSONファイルにオブジェクトデータを保存する

	// ====================================================
	// ゲッター類
	// ====================================================
	nlohmann::json LoadJsonData(const std::string& _filePath);							// JSONファイルを読み込む

	bool LoadObjectsFromJson(const std::string& _filePath);								// オブジェクトデータが読み込めた場合はtrue、そうでない場合はfalseを返す
	bool LoadUIFromJson(const std::string& _filePath);									// UIのデータが読み込めたらtrue、そうでない場合はfalseが返る

	const std::vector<Object>& GetObjects()const { return m_objects; }					// オブジェクトリストを返す
	void ListClear() { m_objects.clear(); }

private:

	std::vector<Object>			m_objects;		// オブジェクトを管理する可変長配列

// シングルトンパターン(インクルードすれば、どこでもクラスを使用できる)
private:

	ObjectManager() {}

public:

	static ObjectManager& Instance()
	{
		static ObjectManager Instance;
		return Instance;
	}
};