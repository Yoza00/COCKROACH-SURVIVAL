#include "ResultScene.h"
#include"../SceneManager.h"
#include"../../GameObject/Stage/ObjectManager/ObjectManager.h"

#include"../../GameObject/Stage/ObjectData/ObjectData.h"
#include"../../GameObject/UI/UI.h"
#include"../../GameObject/UI/SceneChange/SceneChange.h"

#include"../../GameObject/UI/ResultScore/ResultScore.h"

void ResultScene::Event()
{
	/*if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (!m_isClick)
		{
			m_isClick = true;

			std::shared_ptr<SceneChange>	_spChange = std::make_shared<SceneChange>();
			_spChange->SetFilePath("Asset/Textures/GameObject/SceneChange/BlackScreen.png");
			_spChange->Init();
			_spChange->SetDrawPos({ 0.0f,0.0f });
			m_UIList.push_back(_spChange);
		}
	}*/
}

void ResultScene::Init()
{
	SceneManager::Instance();

	// UIのロード
	if (ObjectManager::Instance().LoadUIFromJson("Asset/Data/Json/UIData/UIData.json"))
	{
		const std::vector<Object>& _uis = ObjectManager::Instance().GetObjects();

		for (const auto& ui : _uis)
		{
			if (ui.m_useScene != "result")
			{
				continue;
			}

			if (ui.m_uiType == "Score_String")
			{
				int _score = m_drawScore;
				int _index = 0;

				// 必要な桁数を求める
				// 負の値ではない、1桁の数になるまで計算する。
				// ループが一周するたびに桁数を加算させる
				while (1)
				{
					_score /= 10;

					if (_score >= 0 && _score < 10)
					{
						break;
					}

					_index += 1;
				}

				// スコアの桁数分、スコアの画像を用意する
				for (int cnt = 0; cnt < _index; cnt += 1)
				{
					const std::shared_ptr<ResultScore>	_spScore = std::make_shared<ResultScore>();
					_spScore->SetFilePath(ui.m_filePath);
					_spScore->Init();
					_spScore->SetDrawPos({ ui.m_pos.x,ui.m_pos.y });
					_spScore->SetDrawScore(m_drawScore);
					_spScore->SetIndex(cnt);
					m_UIList.push_back(_spScore);
				}
			}
			else
			{
				const std::shared_ptr<UI>	_spUI = std::make_shared<UI>();
				_spUI->SetFilePath(ui.m_filePath);
				_spUI->Init();
				_spUI->SetDrawPos({ ui.m_pos.x,ui.m_pos.y });
				m_UIList.push_back(_spUI);
			}
		}

		// エラー防止用
		ObjectManager::Instance().ListClear();		// リストの中身全削除
	}
}
