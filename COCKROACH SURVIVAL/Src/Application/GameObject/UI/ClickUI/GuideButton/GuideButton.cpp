﻿#include "GuideButton.h"

#include"../DrawGuide/DrawGuide.h"

#include"../../../../Scene/SceneManager.h"

void GuideButton::ButtonClicked()
{
	// UIクラス(普通のUI描画クラス)で描画する
	std::shared_ptr<DrawGuide>	_spUI = std::make_shared<DrawGuide>();
	_spUI->SetFilePath("Asset/Textures/GameObject/UI/GameMenu/Menu_KeyGuide.png");
	_spUI->Init();
	_spUI->SetDrawPos();	// 画面の中心で描画する
	SceneManager::Instance().AddUI(_spUI);

	m_wpDrawGuide = _spUI;
}
