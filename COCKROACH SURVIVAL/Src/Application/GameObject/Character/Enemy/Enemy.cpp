#include "Enemy.h"
#include"../../../Scene/SceneManager.h"
#include"../Player/Player.h"

#include"../../../main.h"

Enemy::Enemy(std::vector<std::vector<int>>& grid) :
	m_grid(&grid)
{

}

void Enemy::Update()
{
	if (!m_spModel)return;

	if (SceneManager::Instance().GetIsMenu())return;

	// ノード情報をセット
	SetValueFromNodeInfo();

	// デバッグ
	m_pDebugWire->AddDebugSphere(
		m_rightHandPos,
		1.0f,
		kBlackColor
	);
	// ==========

	// ========== 重力による座標更新 ==========
	m_gravity	+= m_gravityPow;
	m_pos.y		-= m_gravity;
	// ========================================

	// 視界判定(判定し終えてから更新処理を行う)
	CheckSight();

	// ステートパターンを使用して、状態に応じた処理を行うように処理する
	if (m_nowState)
	{
		m_nowState->Update(*this);
	}

	/*
	ステートパターンを採用する理由
	・状態によって変化する変数等はステートパターン内で処理を行う為、更新処理関数内がきれいになる。
	・状態によって区切られているため、変更・改修が容易。
	*/

	// 行列関連
	{
		// 各要素の行列を作成
		Math::Matrix	_scaleMat = Math::Matrix::CreateScale(m_scale);									// 拡縮行列
		Math::Matrix	_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));	// Y軸における回転行列
		Math::Matrix	_transMat = Math::Matrix::CreateTranslation(m_pos);								// 座標行列

		// ワールド行列を作成(各要素の行列を合成したもの)
		m_mWorld = _scaleMat * _rotMat * _transMat;
	}
}

void Enemy::PostUpdate()
{
	if (!m_spModel)return;

	if (SceneManager::Instance().GetIsMenu())return;

	HitCheck();			// オブジェクトとの当たり判定を調べる

	// アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}

// ========== 描画関連 ==========
void Enemy::DrawLit()
{
	if (!m_spModel)return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Enemy::GenerateDepthMapFromLight()
{
	if (!m_spModel)return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
// ==============================

// 初期化
void Enemy::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		//*m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Character/Enemy/Enemy.gltf");
		//*m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Character/Enemy/Enemy_people.gltf");
		//*m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Character/Enemy/Enemy_people_animation2.gltf");
		
		//*m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Character/Enemy/Enemy_people_animation3.gltf");
		//*m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Character/Enemy/Enemy-people_AddHandPositionNode_test.gltf");
		*m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Character/Enemy/Enemy-people_Add_Attack.gltf");
	}

	// ウェイポイントの情報セット
	if (!LoadWayPointsFronJson("Asset/Data/Json/WayPoints/WayPoints.json"))
	{
		assert(0 && "ファイルの読み込みに失敗しました。");		// 読み込めなかった場合はエラー表示
		return;
	}

	m_wayNumber = m_minWayNumber;								// 念のため、ポイント番号を初期座標に変更
	m_pos		= m_wayPoints[m_wayNumber].m_pos;				// 初期座標をセット
	m_wayNumber++;

	m_charaType = CharaType::E_Chara;

	// 初期状態のステートを設定
	ChangeState(std::make_shared<Search>());

	// アニメーション関連
	m_spAnimator = std::make_shared<KdAnimator>();
	//m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Walk"));	// 初期アニメーション設定
	ChangeAnimation("Walk");	// 初期アニメーション設定

	// ========== デバッグ用 ==========
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	// ================================
}

// Math::Vector3をNodeに変換し、ゴールノードの情報に応じて使用する経路探索の処理を変更する
void Enemy::SetGoal(const Math::Vector3& goalPos)
{
	// 現在のゴールのグリッド座標
	Node	_goalNode = WorldToGrid(goalPos);

	// ゴールのグリッド座標をもとに実行する経路探索の判定
	// 0=障害物ではない場所、1=障害物の場所
	if ((*m_grid)[_goalNode.z][_goalNode.x] == 0)
	{
		// 通常の経路探索で、スタートからゴールまでの経路が求められる
		FindPath(m_pos, goalPos);			// 経路計算
	}
	else if ((*m_grid)[_goalNode.z][_goalNode.x] == 1)
	{
		// ゴールが障害物の場所であり、移動可能な範囲で一番近づくことができる経路を求められる
		FindNearestWalkableNode(_goalNode);	// 経路探索
	}
}

// 通常の経路探索(スタートからゴールまでの経路を探索する(ゴールが障害物の場合は経路探索の終了までに時間がかかる)
void Enemy::FindPath(const Math::Vector3& start, const Math::Vector3& goal)
{
	m_path.clear();					// 経路を初期化
	Node	_start	= WorldToGrid(start);
	Node	_goal	= WorldToGrid(goal);

	std::priority_queue < Node, std::vector<Node>, std::function<bool(Node, Node)>>	_openList
	{
		[](Node a,Node b)
		{
			return a.fCost() > b.fCost();
		}
	};

	std::unordered_map<Node, float, NodeHash>	_gScore;
	_openList.push(_start);
	_gScore[_start] = 0;

	// 動的確保したノード追跡用リスト
	std::vector<Node*>	_allocatedNodes;

	while (!_openList.empty())
	{
		Node	_current = _openList.top();
		_openList.pop();

		if (_current == _goal)
		{
			for (Node* node = &_current; node != nullptr; node = node->parent)
			{
 				m_path.push_back(*node);		// 経路に追加
			}

			std::reverse(m_path.begin(), m_path.end());
			m_currentPathIndex = 0;

			// 動的確保したノードを解放
			for (Node* node : _allocatedNodes)
			{
				delete node;
			}

			return;
		}

		const std::vector<std::pair<int, int>>	_directions = {
			{1,0},
			{0,1},
			{-1,0},
			{0,-1}
		};

		for (auto& dir : _directions)
		{
			Node	_neighbor = { _current.x + dir.first,_current.z + dir.second };

			// 範囲外の探索をしないように制御
			if (_neighbor.z < 0 || _neighbor.x < 0 || _neighbor.z >= m_grid->size() || _neighbor.x >= (*m_grid)[0].size())
			{
				continue;
			}

			// 障害物があれば処理しない
			if ((*m_grid)[_neighbor.z][_neighbor.x] == 1)
			{
				continue;
			}

			float	_tentativeGScore = _gScore[_current] + 1;

			if (_gScore.find(_neighbor) == _gScore.end() || _tentativeGScore < _gScore[_neighbor])
			{
				_gScore[_neighbor]	= _tentativeGScore;
				_neighbor.gCost		= _tentativeGScore;
				_neighbor.hCost		= Heuristic(_neighbor, _goal);

				// 親ノードを動的確保し、追跡リストに追加
				_neighbor.parent	= new Node(_current);
				_allocatedNodes.push_back(_neighbor.parent);

				_openList.push(_neighbor);
			}
		}
	}

	// 探索失敗時、動的確保したノードを解放
	for (Node* node : _allocatedNodes)
	{
		delete node;
	}
}

// ゴールノードが障害物のある場所である際に使用する経路探索(移動できる範囲内で一番近くに移動できる経路を探索する)
Node Enemy::FindNearestWalkableNode(const Node& targetNode)
{
	std::queue<Node>					_searchQueue;
	std::unordered_set<Node, NodeHash>	_visited;

	_searchQueue.push(targetNode);
	_visited.insert(targetNode);

	const std::vector<std::pair<int, int>>	_directions = {
			{1,0},
			{0,1},
			{-1,0},
			{0,-1}
	};

	// 周囲のノードを探索して最も近い移動可能なノードを探索する
	// 中身がある場合
	while (!_searchQueue.empty())
	{
		Node	_currentNode = _searchQueue.front();
		_searchQueue.pop();

		// 現在のノードが移動可能ならそれを返す
		if ((*m_grid)[_currentNode.z][_currentNode.x] == 0)
		{
			return	_currentNode;
		}

		// 周囲のノードを探索
		for (auto& dir : _directions)
		{
			Node	_neighbor = { _currentNode.x + dir.first,_currentNode.z + dir.second };

			// 範囲外チェック
			if (_neighbor.x < 0 || _neighbor.z < 0 || _neighbor.x >= (*m_grid)[0].size() || _neighbor.z >= m_grid->size())
			{
				continue;
			}

			// 訪問済みか確認
			if (_visited.find(_neighbor) != _visited.end())
			{
				continue;
			}

			// キューに追加
			_searchQueue.push(_neighbor);
			_visited.insert(_neighbor);
		}
	}

	// 適切な移動先が見つからなかった場合、デフォルトで元のターゲットノードを返す
	return	targetNode;
}

// Math::Vector3型の座標ををNode座標(グリッド座標)に変換
Node Enemy::WorldToGrid(const Math::Vector3& WorldPos)
{
	int	_gridX = static_cast<int>(std::round(WorldPos.x - m_mapOrigin.x));
	int	_gridZ = static_cast<int>(std::round(m_mapOrigin.z - WorldPos.z));

	return { _gridX,_gridZ };
}

// Node座標(グリッド座標)をMath::Vector3型の座標に変換
Math::Vector3 Enemy::GridToWorld(const Node& node)
{
	float	_worldX = m_mapOrigin.x + node.x;
	float	_worldZ = m_mapOrigin.z - node.z;

	return { _worldX,0.04f,_worldZ };
}

// オブジェクトとの当たり判定を調べる
void Enemy::HitCheck()
{
	// 地面との当たり判定
	KdCollider::RayInfo	_rayInfo;
	_rayInfo.m_pos		= m_pos;						// レイの発射座標(必要に応じて補正をかける)
	_rayInfo.m_dir		= Math::Vector3::Down;			// レイの発射角度(地面と判定するので、下方向へのベクトルにする必要がある)
	_rayInfo.m_range	= 3.0f;							// レイの長さ(必要に応じて、長さを調節すべし)
	_rayInfo.m_type		= KdCollider::TypeGround;		// 当たり判定を行うタイプ

	// ========== レイ情報のデバッグ表示 ==========
	m_pDebugWire->AddDebugLine(
		_rayInfo.m_pos,
		_rayInfo.m_dir,
		_rayInfo.m_range,
		kBlackColor
	);
	// ============================================

	std::list<KdCollider::CollisionResult>	_retRayList;		// 当たったオブジェクトを格納するリスト

	// すべてのオブジェクトとの当たり判定
	for (auto& _objects : SceneManager::Instance().GetObjList())
	{
		_objects->Intersects(_rayInfo, &_retRayList);			// レイ情報をもとに当たり判定を行い、当たっていればリストに格納
	}

	// 一番近いオブジェクトを探すのに必要な変数
	bool			_isHit		= false;						// 当たったかどうかのフラグ
	float			_maxOverLap = 0.0f;							// 最大の重なり量
	Math::Vector3	_groundPos	= {};							// ヒット座標(地面座標)

	// リストに格納されていうrオブジェクトの中で、一番近くであたっているオブジェクトを調べる
	for (auto& _object : _retRayList)
	{
		// 先ほど宣言したローカル行列より、当たり判定の結果の重なり量の方が大きかったら
		if (_maxOverLap < _object.m_overlapDistance)
		{
			_maxOverLap = _object.m_overlapDistance;			// 重なり量更新
			_groundPos	= _object.m_hitPos;						// ヒット座標更新
			_isHit		= true;									// フラグを起動
		}
	}

	if (_isHit)
	{
		m_pos		= _groundPos;								// 座標をヒット座標に更新
		m_pos		+= {0.0f, 0.05f, 0.0f};
		m_gravity	= 0.0f;										// 重力の強さはリセット
	}
}

// 視界内 かつ プレイヤーを視認できているかどうかを確認
void Enemy::CheckSight()
{
	const std::shared_ptr<Player>	_spPlayer = m_wpPlayer.lock();
	if (!_spPlayer)
	{
		// プレイヤーのポインタがない場合は処理しない
		return;
	}

	Math::Vector3	_nowDir			= m_mWorld.Backward();		// 向いている方向(視界の中心となる方向ベクトル)
	Math::Vector3	_nowPlayerPos	= _spPlayer->GetPos();		// プレイヤーの今の座標
	Math::Vector3	_mixVecDir		= _nowPlayerPos - m_sightPos;	// 2つの座標から１つの方向ベクトルを作成(自分(エネミー)から見たプレイヤーへのベクトル)
	//Math::Vector3	_mixVecDir		= _nowPlayerPos - m_pos;	// 2つの座標から１つの方向ベクトルを作成(自分(エネミー)から見たプレイヤーへのベクトル)

	// 一定距離以上離れてしまっている場合は処理しない
	if (_mixVecDir.LengthSquared() >= m_sightRange)
	{
		m_isSight = false;				// 視界内にいないことにする
		return;
	}

	// 方向ベクトルの正規化
	_nowDir.Normalize();
	_mixVecDir.Normalize();

	// 内積を計算
	float			_dot			= _nowDir.Dot(_mixVecDir);
	
	// 内積値の補正
	// 念のため-1.0f<=_dot<=1.0fの範囲内になるようにしておく
	_dot = std::clamp(_dot, -1.0f, 1.0f);

	bool			_isSight		= false;												// 視界内かどうか
	float			_sightAria		= cos(DirectX::XMConvertToRadians(m_sightSize));		// 視野の端っこ

	// 視界内にプレイヤーがいる場合
	if (_dot > _sightAria)
	{
		// ========== レイ判定(オブジェクトに視界を邪魔されていないか) ==========
		_isSight = GetDotResult(_mixVecDir);
		// ======================================================================
	}

	m_isSight = _isSight;			// 実行した結果をもとに、フラグを更新
	/* 実行結果から、視界内に居ればメンバー変数のフラグがtrueに、視界外に居ればfalseになる */

	if (!m_isSight)
	{
		return;						// 視界外に居る場合はリターン
	}

	// モデルの回転処理
	float	_angle = DirectX::XMConvertToDegrees(acos(_dot));		// 内積値のacosで角度を求める
	
	// 閾値よりも値が小さければ処理しない
	if (_angle < m_rotThreshold)
	{
		return;
	}

	if (_angle > m_maxAngle)					// 角度制御
	{
		_angle = m_maxAngle;
	}

	Math::Vector3	_cross;				// 外積格納用変数
	_cross = _mixVecDir.Cross(_nowDir);	// 外積計算

	// Y軸回転のみ行うため、Y要素にのみ処理を作成
	if (_cross.y >= 0.0f)
	{
		m_angle -= _angle;

		// -360.0f(１回転分)を超えないように制御
		if (m_angle < m_minDegAngle)
		{
			m_angle += m_maxDegAngle;
		}
	}
	else
	{
		m_angle += _angle;

		// 360.0f(１回転分)を超えないように制御
		if (m_angle >= m_maxDegAngle)
		{
			m_angle -= m_maxDegAngle;
		}
	}
}

void Enemy::SetValueFromNodeInfo()
{
	// エネミー本体から見た視界の根本の座標
	{
		const KdModelWork::Node* _pSightNode = m_spModel->FindNode("sightPoint");
		if (_pSightNode)
		{
			m_localSightMat = _pSightNode->m_worldTransform;
		}

		Math::Vector3 _sightPos = (m_localSightMat * m_mWorld).Translation();

		m_sightPos = _sightPos;
	}

	// 右手の座標
	{
		const KdModelWork::Node* _pRightHandNode = m_spModel->FindNode("RightHandNode");
		if (_pRightHandNode)
		{
			m_rightHandMat = _pRightHandNode->m_worldTransform;
		}

		Math::Vector3 _rightHandPos = (m_rightHandMat * m_mWorld).Translation();

		m_rightHandPos = _rightHandPos;
	}

	// 高所判定の座標
	{
		const KdModelWork::Node* _pAttackPoint_highNode = m_spModel->FindNode("AttackPoint_high");
		if (_pAttackPoint_highNode)
		{
			m_attackPoint_HighMat = _pAttackPoint_highNode->m_worldTransform;
		}

		Math::Vector3	_attackPoint_highPos = (m_attackPoint_HighMat * m_mWorld).Translation();

		m_attackPoint_highPos = _attackPoint_highPos;
	}

	// 低所判定の座標
	{
		const KdModelWork::Node* _pAttackPoint_lowNode = m_spModel->FindNode("AttackPoint_low");
		if (_pAttackPoint_lowNode)
		{
			m_attackPoint_LowMat = _pAttackPoint_lowNode->m_worldTransform;
		}

		Math::Vector3	_attackPoint_lowPos = (m_attackPoint_LowMat * m_mWorld).Translation();

		m_attackPoint_LowPos = _attackPoint_lowPos;
	}
}

void Enemy::RotateToDirection(const Math::Vector3& toDir)
{
	// ゼロベクトルなら処理しない
	if (toDir.LengthSquared() < 0.0001f)
	{
		return;
	}

	// 正規化された進行方向
	Math::Vector3	_normalizedDirection = toDir;		// 向きたい方向ベクトルをコピー
	_normalizedDirection.Normalize();					// 先ほど作成したものを正規化
	Math::Vector3	_nowDir = m_mWorld.Backward();
	_nowDir.Normalize();

	// 内積を用いて回転角度を計算
	float	_dot	= _nowDir.Dot(_normalizedDirection);	// モデルの前方方向へのベクトルと向きたい方向ベクトルとの内積を算出
	_dot = std::clamp(_dot, -1.0f, 1.0f);								// 内積の値を-1～1の範囲に制限
	float	_angle	= DirectX::XMConvertToDegrees(acos(_dot));			// 逆余弦関数で求められたラジアン角の値をディグリー角に変換

	// 閾値よりも値が小さければ処理しない
	if (_angle < m_rotThreshold)
	{
		return;
	}

	// 回転角度が最大値よりも大きい場合は、回転角度を最大値に設定する
	if (_angle > m_maxAngle)					// 角度制御
	{
		_angle = m_maxAngle;
	}

	// 外積を用いて回転軸を計算
	Math::Vector3	_cross = _normalizedDirection.Cross(_nowDir);

	// 徐々に回転させる
	if (_cross.y >= 0.0f)
	{
		m_angle -= _angle;

		// -360.0f(１回転分)を超えないように制御
		if (m_angle < m_minDegAngle)
		{
			m_angle += m_maxDegAngle;
		}
	}
	else
	{
		m_angle += _angle;

		// 360.0f(１回転分)を超えないように制御
		if (m_angle >= m_maxDegAngle)
		{
			m_angle -= m_maxDegAngle;
		}
	}
}

// 内積の判定の結果を返す
bool Enemy::GetDotResult(const Math::Vector3& toDir)
{
	KdCollider::RayInfo _rayInfo;
	_rayInfo.m_pos		= m_sightPos;
	_rayInfo.m_dir		= toDir;
	_rayInfo.m_range	= m_sightRange;
	_rayInfo.m_type		= KdCollider::TypeGround | KdCollider::TypeSight;

	// ========== デバッグ表示 ==========
	m_pDebugWire->AddDebugLine
	(
		_rayInfo.m_pos,
		_rayInfo.m_dir,
		_rayInfo.m_range,
		kBlueColor
	);
	// ==================================

	std::list<KdCollider::CollisionResult>	_retRayList;		// ヒットオブジェクト格納用リスト
	std::vector<UINT>						_charaNumber;		// ヒットオブジェクトのキャラナンバー格納用配列

	for (auto& _objects : SceneManager::Instance().GetObjList())
	{
		if (_objects->Intersects(_rayInfo, &_retRayList))			// レイ情報をもとに、ヒットオブジェクトをリストに格納
		{
			_charaNumber.push_back(_objects->GetCharaType());		// 可変長配列にヒットオブジェクトのキャラナンバーを格納
		}
	}

	int		_cnt		= 0;			// カウンタ変数
	UINT	_number		= 0;			// 最終的なキャラナンバー
	float	_maxOverLap = 0.0f;			// 重なり量
	bool	_isHit		= false;		// ヒットフラグ

	for (auto& _ret : _retRayList)
	{
		if (_maxOverLap < _ret.m_overlapDistance)
		{
			_number = _charaNumber[_cnt];			// 可変長配列からほしい番号を取得

			_maxOverLap = _ret.m_overlapDistance;		// 重なり量を更新
			_isHit = true;							// フラグ起動
		}

		_cnt++;			// カウンタ更新
	}

	if (_isHit)
	{
		// エネミーのプレイヤーへの視線がオブジェクトで妨げられているかどうか
		// 判定方法は、レイに一番近くであたっているのがプレイヤーかどうかで判定
		if (_number == CharaType::P_Chara)
		{
			return true;			// プレイヤーが一番近い(オブジェクトに妨げられていない)
		}
	}

	return false;
}

bool Enemy::LoadWayPointsFronJson(const std::string& _filePath)
{
	nlohmann::json	_jsonData;

	std::ifstream	_file(_filePath);

	// ファイルのオープンチェック
	if (!_file.is_open())
	{
		return false;			// 開けなかった場合はfalseを返す
	}

	// JSONデータを解析
	_file >> _jsonData;			// jsonデータを解析
	_file.close();				// ファイルを閉じる

	for (const auto& _waypoints : _jsonData["waypoints"])
	{
		WayPoint _waypoint;
		_waypoint.m_number	= _waypoints["number"];			// 番号を格納
		_waypoint.m_pos.x	= _waypoints["position"]["x"];	// 座標を格納
		_waypoint.m_pos.y	= _waypoints["position"]["y"];
		_waypoint.m_pos.z	= _waypoints["position"]["z"];

		m_wayPoints.push_back(_waypoint);					// 配列に追加
	}

	return true;				// 無事に開ければtrueが返る
}

void Enemy::ChangeAnimation(const std::string& animationName, bool isLoop)
{
	m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation(animationName), isLoop);
}

// ========== ステートパターン関係 ==========
void Enemy::ChangeState(const std::shared_ptr<StateBase>& nextSate)
{
	if (m_nowState)
	{
		m_nowState->Exit(*this);
	}
	m_nowState = nextSate;
	m_nowState->Enter(*this);
}

// ========== 捜索 ==========
void Enemy::Search::Enter(Enemy& owner)
{
	owner.m_searchCnt = 0;			// カウンタをリセットしておく(０に戻しておく)
}

void Enemy::Search::Update(Enemy& owner)
{
	// 視界判定の結果、フラグが切り替わっている(視界内にプレイヤーがいる)場合、ステートを切り替える
	if (owner.m_isSight)
	{
		owner.ChangeState(std::make_shared<Chase>());			// 追跡状態に切り替え
		return;
	}

	owner.m_searchCnt++;										// カウンタを更新
	if (owner.m_searchCnt > owner.m_seaMaxCnt)
	{
		owner.ChangeState(std::make_shared<MoveOtherPos>());	// 時間が経過したらステート切り替え
		return;
	}
}

void Enemy::Search::Exit(Enemy& owner)
{

}

// ========== その他の場所へ移動 ==========
void Enemy::MoveOtherPos::Enter(Enemy& owner)
{
	owner.m_isTurnFinish	= false;											// ターン官僚フラグを解除
	owner.m_isMoveNextPos	= false;											// 次の場所への移動フラグを解除
	owner.m_nextPos			= owner.m_wayPoints[owner.m_wayNumber].m_pos;		// 移動先座標更新

	// アニメーション変更
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Walk"));
}

void Enemy::MoveOtherPos::Update(Enemy& owner)
{
	if (owner.m_isSight)
	{
		if (owner.m_isTurnFinish)
		{
			owner.ChangeState(std::make_shared<Chase>());			// 見つけたらステート切り替え
			return;
		}
	}

	Math::Vector3	_dist	= owner.m_nextPos - owner.m_pos;	// 距離
	Math::Vector3	_toDir	= _dist;							// 次の場所への方向ベクトル
	_toDir.Normalize();

	// 回転が終了していなければ回転処理を行う
	if (!owner.m_isTurnFinish)
	{
		Turn(owner,_toDir);										// ターンが終わっていなければ、ターンさせる
	}
	else
	{
		owner.m_pos += _toDir * owner.m_moveSpeed;			// 移動

		CheckMoveFinish(owner, _dist);
	}
}

void Enemy::MoveOtherPos::Exit(Enemy& owner)
{
}

void Enemy::MoveOtherPos::Turn(Enemy& owner,const Math::Vector3& dir)
{
	Math::Vector3	_nowDir = owner.m_mWorld.Backward();		// 現在の向き
	Math::Vector3	_toDir	= dir;								// 次の場所へのベクトル

	// ベクトルの正規化
	_nowDir.Normalize();
	
	// 内積計算
	float			_dot = _nowDir.Dot(_toDir);

	// 内積値の補正
	_dot = std::clamp(_dot, -1.0f, 1.0f);

	float			_angle = DirectX::XMConvertToDegrees(acos(_dot));

	if (_angle < owner.m_rotThreshold)
	{
		owner.m_isTurnFinish	= true;							// ターン終了フラグを起動
		owner.m_isMoveNextPos	= true;							// 次の場所への移動フラグを起動
		return;
	}

	// 角度制御
	if (_angle > owner.m_maxAngle)
	{
		_angle = owner.m_maxAngle;
	}

	// 外積計算
	Math::Vector3	_cross = _toDir.Cross(_nowDir);

	if (_cross.y >= 0.0f)
	{
		owner.m_angle -= _angle;

		if (owner.m_angle < owner.m_minDegAngle)
		{
			owner.m_angle += owner.m_maxDegAngle;
		}
	}
	else
	{
		owner.m_angle += _angle;

		if (owner.m_angle >= owner.m_maxDegAngle)
		{
			owner.m_angle -= owner.m_maxDegAngle;
		}
	}
}

void Enemy::MoveOtherPos::CheckMoveFinish(Enemy& owner,const Math::Vector3& dist)
{
	// 次の場所との距離が一定以下まで近づいた
	if (dist.LengthSquared() <= owner.m_ignoreLength)
	{
		owner.ChangeState(std::make_shared<SearchAround>());	// ステート切り替え
		owner.m_wayNumber++;									// 移動場所の更新
		
		// 配列外に行かないように制御
		if (owner.m_wayNumber >= owner.m_maxWayPoint)
		{
			owner.m_wayNumber = owner.m_minWayNumber;
		}

		return;
	}
}

// ========== 追跡 ==========
void Enemy::Chase::Enter(Enemy& owner)
{
	owner.ChangeAnimation("Run");
}

void Enemy::Chase::Update(Enemy& owner)
{
	Math::Vector3	_playerPos = Math::Vector3::Zero;
	
	{
		const std::shared_ptr<Player>	_spPlayer = owner.m_wpPlayer.lock();
		if (!_spPlayer)
		{
			owner.ChangeState(std::make_shared<Search>());			// ステート切り替え
			return;
		}

		// プレイヤーの座標を取得
		_playerPos = _spPlayer->GetPos();
	}
	

	// 視界外に居る場合
	if (!owner.m_isSight)
	{
		owner.m_loseSightPos = _playerPos;						// 見失った座標を更新
		owner.ChangeState(std::make_shared<LoseSight>());		// ステート切り替え
		return;
	}

	// 経路を辿っている最中の場合は処理を継続し、
	// それ以外の場合は、プレイヤーの場所までの最短経路を算出する(次のループから算出された経路を辿る)
	if (owner.m_currentPathIndex < owner.m_path.size())
	{
		// 現在のターゲットノード
		Math::Vector3	_target = owner.GridToWorld(owner.m_path[owner.m_currentPathIndex]);
		
		// エネミーの現在位置とターゲットの位置の間を補間
		Math::Vector3	_direction = _target - owner.m_pos;

		// 移動速度よりもターゲットまでの距離が短い場合(移動してしまうとターゲットを通過してしまう場合)
		if (_direction.LengthSquared() < owner.m_moveSpeed)
		{
			owner.m_pos = _target;				// 座標をターゲットの座標に更新
			owner.m_currentPathIndex++;			// 次のノードへ
		}
		else
		{
			_direction.Normalize();

			owner.RotateToDirection(_direction);

			owner.m_pos += _direction * owner.m_moveSpeed;		// エネミーをターゲットの方向へ移動
		}
	}
	else
	{
		owner.SetGoal(_playerPos);
	}

	// 攻撃可能かどうかを確認
	if (ChackAttackAble(owner) == true)
	{
		if (owner.m_pos.y >= owner.m_attackPoint_highPos.y)
		{
			// 高所
			owner.ChangeState(std::make_shared<Attack_HighPosition>());
			return;
		}
		else if (owner.m_pos.y <= owner.m_attackPoint_LowPos.y)
		{
			// 低所
			owner.ChangeState(std::make_shared<Attack_LowPosition>());
			return;
		}
		else
		{
			// 中間
			owner.ChangeState(std::make_shared<Attack_MidPosition>());
			return;
		}
	}
}

void Enemy::Chase::Exit(Enemy& owner)
{

}

bool Enemy::Chase::ChackAttackAble(Enemy& owner)
{
	const std::shared_ptr<Player>	_spPlayer = owner.m_wpPlayer.lock();

	if (!_spPlayer)
	{
		return false;
	}

	Math::Vector3	_playerPos	= _spPlayer->GetPos();
	Math::Vector3	_distance	= owner.m_pos - _playerPos;

	// プレイヤー情報の取得ができなかった場合とプレイヤーとの距離が一定以上離れている場合はfalseを返す
	if (_distance.LengthSquared() >= owner.m_attackDistance)
	{
		return false;
	}
	
	return true;
}

// ========== 見失った ==========
void Enemy::LoseSight::Enter(Enemy& owner)
{
	// 見失った座標をグリッド座標に変換
	Node	_loseSightNode = owner.WorldToGrid(owner.m_loseSightPos);

	// 見失ったグリッド座標が進行不能オブジェクトであるかを確認
	if ((*owner.m_grid)[_loseSightNode.z][_loseSightNode.x] == 1)
	{
		// 移動可能な範囲内で最も近づくことのできる経路を計算
		_loseSightNode = owner.FindNearestWalkableNode(_loseSightNode);
	}

	// 経路計算
	owner.FindPath(owner.m_pos, owner.GridToWorld(_loseSightNode));
}

void Enemy::LoseSight::Update(Enemy& owner)
{
	// 視界にプレイヤーを確認したらステートを切り替える
	if (owner.m_isSight)
	{
		owner.ChangeState(std::make_shared<Chase>());
		return;
	}

	if (owner.m_currentPathIndex < owner.m_path.size())
	{
		// 現在のターゲットノード
		Math::Vector3	_targetPos = owner.GridToWorld(owner.m_path[owner.m_currentPathIndex]);

		// エネミーの現在位置とターゲット位置の間を補間
		Math::Vector3	_direction			= _targetPos - owner.m_pos;

		if (_direction.LengthSquared() < owner.m_moveSpeed)
		{
			// ターゲットノードに到着
			owner.m_pos = _targetPos;
			owner.m_currentPathIndex++;		// 次のノードへ
		}
		else
		{
			// ターゲット方向にエネミー速度で移動
			_direction.Normalize();
			owner.m_pos += _direction * owner.m_moveSpeed;
		}
	}
	else
	{
		// 経路の移動が完了したらステートを変更する
		owner.ChangeState(std::make_shared<SearchAround>());		// ステート切り替え
		return;
	}
}

void Enemy::LoseSight::Exit(Enemy& owner)
{
	owner.m_isFixNextPos = true;
}

void Enemy::LoseSight::CheckMoveFinish(Enemy& owner, const Math::Vector3& dist)
{
	if (dist.LengthSquared() <= owner.m_ignoreLength)
	{
		owner.ChangeState(std::make_shared<SearchAround>());			// ステート切り替え
		return;
	}
}

// ========== 周囲の捜索 ==========
void Enemy::SearchAround::Enter(Enemy& owner)
{
	owner.m_searchPhase = SearchPhase::RotRight;
	owner.m_searchTimer = owner.m_resetTimer;
	owner.m_baseAngle	= owner.m_angle;
	owner.m_isNowRotFin = false;
}

void Enemy::SearchAround::Update(Enemy& owner)
{
	owner.m_searchTimer--;				// 捜索制限時間の更新

	// 視界内にプレイヤーがいれば
	if (owner.m_isSight)
	{
		owner.ChangeState(std::make_shared<Chase>());
		return;
	}

	if (owner.m_searchTimer >= 0 || !owner.m_isNowRotFin)
	{
		SearchPlayer(owner);				// プレイヤーを捜索
	}
	else
	{
		owner.m_isSearchFin = true;			// フラグ起動
	}

	// 探索終了
	if (owner.m_isSearchFin)
	{
		// 巡回ルートに戻る為に次の場所を決定する必要がある場合に、場所を決定する
		if (owner.m_isFixNextPos)
		{
			FixNextPos(owner);										// 現在位置から一番近いウェイポイントを検出
		}

		owner.ChangeState(std::make_shared<MoveOtherPos>());		// ステート切り替え
		return;
	}
}

void Enemy::SearchAround::Exit(Enemy& owner)
{
	owner.m_isSearchFin		= false;		// 起動状態のフラグを解除
	owner.m_isFixNextPos	= false;		// 次の場所を決定すべきで無い
}

void Enemy::SearchAround::SearchPlayer(Enemy& owner)
{
	// 現在の状態から判断
	switch (owner.m_searchPhase)
	{
	case SearchPhase::RotRight:

		if (owner.m_isNowRotFin)
		{
			owner.m_isNowRotFin = false;			// 回転完了状態であれば、観点途中状態に変更
		}

		owner.m_angle += owner.m_rotationSpd;		// 回転角度更新

		// 念のための回転制御
		if (owner.m_angle >= owner.m_maxDegAngle)
		{
			owner.m_angle -= owner.m_maxDegAngle;
		}

		// 回転終了
		if (owner.m_angle >= owner.m_baseAngle + owner.m_maxDegAng)
		{
			owner.m_isNowRotFin = true;				// 回転完了状態に変更

			owner.m_angle		= owner.m_baseAngle + owner.m_maxDegAng;
			owner.m_frameCnt	= 0;
			owner.m_searchPhase = SearchPhase::PauseRight;
		}
		break;

	case SearchPhase::PauseRight:
		
		owner.m_frameCnt++;		// カウンタ更新

		// 一定フレーム経過後
		if (owner.m_frameCnt >= 20)
		{
			owner.m_searchPhase = SearchPhase::RotLeft;
		}
		break;

	case SearchPhase::RotLeft:

		if (owner.m_isNowRotFin)
		{
			owner.m_isNowRotFin = false;			// 回転完了状態であれば、回転途中状態に変更
		}

		owner.m_angle -= owner.m_rotationSpd;

		// 念のための回転制御
		if (owner.m_angle < owner.m_minDegAngle)
		{
			owner.m_angle += owner.m_maxDegAngle;
		}

		// 回転終了
		if (owner.m_angle <= owner.m_baseAngle - owner.m_maxDegAng)
		{
			owner.m_isNowRotFin = true;

			owner.m_angle = owner.m_baseAngle - owner.m_maxDegAng;
			owner.m_frameCnt = 0;
			owner.m_searchPhase = SearchPhase::PauseLeft;
		}
		break;

	case SearchPhase::PauseLeft:

		owner.m_frameCnt++;

		if (owner.m_frameCnt >= 20)
		{
			owner.m_searchPhase = SearchPhase::RotRight;
		}
		break;
	}
}

void Enemy::SearchAround::FixNextPos(Enemy& owner)
{
	int				_pointNumber	= 0;		// 座標番号
	Math::Vector3	_dist = owner.m_wayPoints[_pointNumber].m_pos - owner.m_pos;			// エネミーとウェイポイント0番目の距離

	for (int i = 1; i < owner.m_wayPoints.size(); i++)
	{
		Math::Vector3	_distance = owner.m_wayPoints[i].m_pos - owner.m_pos;				// エネミーとウェイポイントi番目の距離

		// ２つの距離を比較
		if (_dist.LengthSquared() > _distance.LengthSquared())
		{
			_dist			= _distance;			// 座標情報をコピー
			_pointNumber	= i;					// 番号更新
		}
	}

	// ループが終了したら、最終的な番号が取れるので
	// それを使用して、一番近いポイントをセットしておく
	owner.m_wayNumber = _pointNumber;
}

// ========== 攻撃関連 =========
void Enemy::Attack_LowPosition::Enter(Enemy& owner)
{
	owner.ChangeAnimation("Attack_Low");
}

void Enemy::Attack_LowPosition::Update(Enemy& owner)
{
	if (CheckAttackArea(owner) == false)
	{
		owner.ChangeState(std::make_shared<Chase>());
		return;
	}
}

void Enemy::Attack_LowPosition::Exit(Enemy& owner)
{
}

bool Enemy::Attack_LowPosition::CheckAttackArea(Enemy& owner)
{
	Math::Vector3	_playerPos = Math::Vector3::Zero;

	{
		const std::shared_ptr<Player>	_spPlayer = owner.m_wpPlayer.lock();
		
		if (!_spPlayer)
		{
			return false;
		}

		_playerPos = _spPlayer->GetPos();
	}

	Math::Vector3	_distance = owner.m_pos - _playerPos;

	// ２転換の距離を比較し、攻撃範囲外に行ってしまっている場合
	if (_distance.LengthSquared() > owner.m_attackDistance)
	{
		return false;
	}

	return true;
}

void Enemy::Attack_MidPosition::Enter(Enemy& owner)
{
	owner.ChangeAnimation("Attack_Mid");
}

void Enemy::Attack_MidPosition::Update(Enemy& owner)
{
}

void Enemy::Attack_MidPosition::Exit(Enemy& owner)
{
}

void Enemy::Attack_HighPosition::Enter(Enemy& owner)
{
	owner.ChangeAnimation("Attack_High");
}

void Enemy::Attack_HighPosition::Update(Enemy& owner)
{
}

void Enemy::Attack_HighPosition::Exit(Enemy& owner)
{
}
