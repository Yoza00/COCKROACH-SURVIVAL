#include "ObjectManager.h"

void ObjectManager::SaveJsonData(const nlohmann::json& _jsonData, const std::string& _filePath)
{
	std::ofstream _file(_filePath);

	// オープンチェック
	if (_file.is_open())
	{
		_file << _jsonData.dump(4);		// インデント付き度保存
		_file.close();
	}
}

nlohmann::json ObjectManager::LoadJsonData(const std::string& _filePath)
{
	nlohmann::json	_jsonData;

	std::ifstream	_file(_filePath);

	// オープンチェック
	if (_file.is_open())
	{
		_file >> _jsonData;			// ファイルに保存されているJSONデータを取得
		_file.close();
	}

	return _jsonData;				// JSONデータを返す
}

bool ObjectManager::LoadObjectsFromJson(const std::string& _filePath)
{
	nlohmann::json	_jsonData;

	std::ifstream	_file(_filePath);

	// オープンチェック
	if (!_file.is_open())return false;		// 開けなかったらfalseを返す

	_file >> _jsonData;
	_file.close();

	for (const auto& obj : _jsonData["objects"])
	{
		Object	_object;
		_object.m_name		= obj["name"];
		_object.m_filePath	= obj["filePath"];
		_object.m_pos.x		= obj["position"]["x"];
		_object.m_pos.y		= obj["position"]["y"];
		_object.m_pos.z		= obj["position"]["z"];
		_object.m_rot		= obj["rotate"];
		_object.m_scale		= obj["scale"];
		m_objects.push_back(_object);
	}

	return true;
}

bool ObjectManager::LoadUIFromJson(const std::string& _filePath, const std::string& useScene)
{
	nlohmann::json	_jsonData;

	std::ifstream	_file(_filePath);

	// オープンチェック
	if (!_file.is_open())return false;		// 開けなかったらfalseを返す

	_file >> _jsonData;
	_file.close();

	for (const auto& obj : _jsonData["UI"])
	{
		// シーンが異なれば読み込まない
		if (obj["use"] != useScene)continue;

		Object	_object;
		_object.m_useScene	= obj["use"];
		_object.m_uiType	= obj["type"];
		_object.m_name		= obj["name"];
		_object.m_filePath	= obj["filePath"];
		_object.m_pos.x		= obj["position"]["x"];
		_object.m_pos.y		= obj["position"]["y"];
		m_objects.push_back(_object);
	}

	return true;
}

bool ObjectManager::LoadUIRankFromJson(const std::string& filePath, const std::string& useUIName)
{
	nlohmann::json	_jsonData;

	std::ifstream	_file(filePath);

	// オープンチェック
	if (!_file.is_open())return false;

	_file >> _jsonData;
	_file.close();

	for (const auto& obj : _jsonData["Rank"])
	{
		if (obj["name"] != useUIName)continue;

		Object	_object;
		_object.m_useScene	= obj["use"];
		_object.m_uiType	= obj["type"];
		_object.m_name		= obj["name"];
		_object.m_filePath	= obj["filePath"];
		_object.m_pos.x		= obj["position"]["x"];
		_object.m_pos.y		= obj["position"]["y"];

		m_objects.push_back(_object);
		break;
	}

	// 念のための制御
	// ループを抜けた後でも、配列内が空の場合はfalseを返すようにしておく
	if (m_objects.empty())return false;

	return true;
}
