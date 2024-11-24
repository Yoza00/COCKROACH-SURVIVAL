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

	for (const auto& _obj : _jsonData["objects"])
	{
		Object	_object;
		_object.m_name = _obj["name"];
		_object.m_filePath = _obj["filePath"];
		_object.m_pos.x = _obj["position"]["x"];
		_object.m_pos.y = _obj["position"]["y"];
		_object.m_pos.z = _obj["position"]["z"];
		_object.m_rot	= _obj["rotate"];
		_object.m_scale = _obj["scale"];
		m_objects.push_back(_object);
	}

	return true;
}

bool ObjectManager::LoadUIFromJson(const std::string& _filePath)
{
	nlohmann::json	_jsonData;

	std::ifstream	_file(_filePath);

	// オープンチェック
	if (!_file.is_open())return false;		// 開けなかったらfalseを返す

	_file >> _jsonData;
	_file.close();

	for (const auto& _obj : _jsonData["UI"])
	{
		Object	_object;
		_object.m_uiType = _obj["type"];
		_object.m_name = _obj["name"];
		_object.m_filePath = _obj["filePath"];
		_object.m_pos.x = _obj["position"]["x"];
		_object.m_pos.y = _obj["position"]["y"];
		m_objects.push_back(_object);
	}

	return true;
}
