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
	std::string	m_useScene	= "";					// 使用する場面
	std::string m_uiType	= "";					// オブジェクトのタイプ
	std::string	m_name		= "";					// オブジェクトの名前
	std::string	m_filePath	= "";					// データのファイルパス
	Position	m_pos		= { 0.0f,0.0f,0.0f };	// 描画座標
	float		m_rot		= 0.0f;					// 回転角度
	float		m_scale		= 0.0f;					// 拡縮率
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
	bool LoadUIFromJson(const std::string& _filePath, const std::string& useScene);		// UIのデータが読み込めたらtrue、そうでない場合はfalseが返る
	// ↑　第2引数で使用するシーンを小文字で記載することで、そのシーンのオブジェクトのみデータを取得する
	
	bool LoadUIRankFromJson(const std::string& filePath, const std::string& useUIName);	// 指定した名前のUIの情報を読み込む
	bool LoadUIDataFromJson(const std::string& filePath, const std::string& useScene, const std::string& useUIName);

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