#include "TitleButton.h"

#include"../../../../GameObject/UI/SceneChange/SceneChange.h"
#include"../../../../Scene/SceneManager.h"

void TitleButton::ButtonClicked()
{
	const std::shared_ptr<SceneChange>	_spScChange = std::make_shared<SceneChange>();
	_spScChange->SetFilePath("Asset/Textures/GameObject/SceneChange/BlackScreen.png");
	_spScChange->Init();
	_spScChange->ActiveTitleBack();
	SceneManager::Instance().AddObject(_spScChange);
	SceneManager::Instance().AddUI(_spScChange);
}
