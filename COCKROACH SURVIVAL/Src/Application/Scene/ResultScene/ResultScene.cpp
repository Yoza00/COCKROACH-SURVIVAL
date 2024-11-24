#include "ResultScene.h"
#include"../SceneManager.h"
#include"../../GameObject/UI/SceneChange/SceneChange.h"

void ResultScene::Event()
{
	/*if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene(
			SceneManager::SceneType::Title
		);
	}*/

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (!m_isClick)
		{
			m_isClick = true;

			std::shared_ptr<SceneChange>	_spChange = std::make_shared<SceneChange>();
			_spChange->SetFilePath("Asset/Textures/GameObject/SceneChange/BlackScreen.png");
			_spChange->Init();
			_spChange->SetDrawPos({ 0.0f,0.0f });
			m_UIList.push_back(_spChange);
			m_objList.push_back(_spChange);
		}
	}
}

void ResultScene::Init()
{
}
