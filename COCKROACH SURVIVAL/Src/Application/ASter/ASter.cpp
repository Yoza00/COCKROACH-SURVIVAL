#include "ASter.h"

// 経路探索のメイン関数
std::vector<std::tuple<int, int>> ASter::FindPath(int startX, int startZ, int goalX, int goalZ, const std::vector<std::vector<int>>& grid)
{
	// グリッドサイズを取得
	int _sizeX = grid.size();
	int _sizeZ = grid[0].size();

	// 開始ノードと終了ノード
	Node	_start{ startX,startZ,0.0f,Heuristic(startX,startZ,goalX,goalZ) };
	Node	_goal{ goalX,goalZ,0.0f,0.0f };

	// 優先度キューのオープンリスト
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>>	_openList;
	_openList.push(_start);

	// コストの記録
	std::unordered_map<int, float>	_costMap;
	_costMap[startX + startZ * _sizeX] = 0.0f;

	// 経路を逆追跡するためのマップ
	std::unordered_map<int, std::tuple<int, int>>	_cameFrom;

	// 隣接ノードの方向(4方位)
	std::vector<std::tuple<int, int>>	_directions =
	{
		// 2D方向(上下左右)
		{1,0},
		{-1,0},
		{0,1},
		{0,-1}
	};

	while (!_openList.empty())
	{
		Node	_current = _openList.top();
		_openList.pop();

		// ゴールに到達した場合、経路を再構築
		if (_current.x == _goal.x && _current.z == _goal.z)
		{
			std::vector<std::tuple<int, int>>	_path;
			std::tuple<int, int>	_step = { _current.x,_current.z };

			while (_cameFrom.find(_current.x + _current.z * _sizeX) != _cameFrom.end())
			{
				_path.push_back(_step);
				_step = _cameFrom[_current.x + _current.z * _sizeX];
			}

			std::reverse(_path.begin(), _path.end());
			
			return _path;
		}

		// 隣接ノードを探索
		for (const auto& [dx, dz] : _directions)
		{
			int _neighborX = _current.x + dx;
			int _neighborZ = _current.z + dz;

			// 範囲外チェック
			if (_neighborX < 0 || _neighborX >= _sizeX || _neighborZ < 0 || _neighborZ >= _sizeZ)
			{
				continue;
			}

			// 進行不可能なセルのチェック
			// 障害物を発見( 1=障害物)
			if (grid[_neighborX][_neighborZ] == 1)
			{
				continue;
			}

			// 新しいコストの計算
			float	_newCost = _costMap[_current.x + _current.z * _sizeX] + 1.0f;
			int		_neighborIndex = _neighborX + _neighborZ * _sizeX;

			// コストが低ければ更新
			if (_costMap.find(_neighborIndex) == _costMap.end() || _newCost < _costMap[_neighborIndex])
			{
				_costMap[_neighborIndex] = _newCost;

				float	_priority = _newCost + Heuristic(_neighborX, _neighborZ, goalX, goalZ);

				_openList.push({ _neighborX,_neighborZ,_newCost,_priority });
				_cameFrom[_neighborIndex] = { _current.x,_current.z };
			}
		}
	}

	// ゴールに到達できない場合
	return {};			// 何もしない
}

// ヒューリスティック関数(ゴールまでの推定距離)
float ASter::Heuristic(int x1, int z1, int x2, int z2)
{
	// ユークリッド距離を使用
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(z2 - z1, 2));		// 2D距離を返す
}
