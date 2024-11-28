#pragma once

class ASter
{
public:

	// 経路を計算する関数
	std::vector<std::tuple<int, int>> FindPath(
		int startX, int startZ, int goalX, int goalZ, const std::vector < std::vector<int>>& grid);

private:

	// ヒューリスティック関数(ゴールまでの推定距離)
	float Heuristic(int x1, int z1, int x2, int z2);

	// ノードのコストを管理するデータ構造
	struct Node
	{
		int		x, z;					// ノードの座標
		float	cost;					// 現在までのコスト
		float	priority;				// ヒューリスティックを含めた総コスト

		// 優先度キューで使うための比較演算子
		bool operator>(const Node& other)const
		{
			return priority > other.priority;
		}
	};
};