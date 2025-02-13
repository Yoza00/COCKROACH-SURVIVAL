#include "SceneManager.h"

#include "BaseScene/BaseScene.h"
#include "TitleScene/TitleScene.h"
#include "GameScene/GameScene.h"
#include"ResultScene/ResultScene.h"

// UI用
#include"../GameObject/UI/UI.h"

void SceneManager::PreUpdate()
{
	// シーン切替
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}

	m_currentScene->PreUpdate();
}

void SceneManager::Update()
{
	m_currentScene->Update();

	KdEffekseerManager::GetInstance().Update();
}

void SceneManager::PostUpdate()
{
	m_currentScene->PostUpdate();
}

void SceneManager::PreDraw()
{
	m_currentScene->PreDraw();
}

void SceneManager::Draw()
{
	m_currentScene->Draw();

	KdEffekseerManager::GetInstance().Draw();
}

void SceneManager::DrawSprite()
{
	m_currentScene->DrawSprite();
}

void SceneManager::DrawDebug()
{
	m_currentScene->DrawDebug();
}

void SceneManager::SetCurrentIsPause(const bool isPause)
{
	m_currentScene->SetPauseFlg(isPause);
}

const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_currentScene->GetObjList();
}

void SceneManager::AddObject(const std::shared_ptr<KdGameObject>& obj)
{
	m_currentScene->AddObject(obj);
}

// UI用
const std::list<std::shared_ptr<UI>>& SceneManager::GetUIList()
{
	return m_currentScene->GetUIList();
}

void SceneManager::AddUI(const std::shared_ptr<UI>& UI)
{
	m_currentScene->AddUIList(UI);
}
void SceneManager::DeleteUI()
{
	m_currentScene->DeleteUI();
}

const bool SceneManager::GetIsPauseThisScene() const
{
	return m_currentScene->GetIsPause();
}

void SceneManager::SetScore(int score)
{
	m_score = score;
}

//========================================

void SceneManager::ChangeScene(SceneType sceneType)
{
	// 音楽、エフェクトを全て停止
	KdAudioManager::Instance().StopAllSound();
	KdEffekseerManager::GetInstance().StopAllEffect();

	// 次のシーンを作成し、現在のシーンにする
	switch (sceneType)
	{
	case SceneType::Title:
		m_currentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::Game:
		m_currentScene = std::make_shared<GameScene>();
		break;
	case SceneType::Result:
		m_currentScene = std::make_shared<ResultScene>(m_score);
	}

	// 現在のシーン情報を更新
	m_currentSceneType = sceneType;

	// ポーズ状態を解除しておく
	m_currentScene->SetPauseFlg(false);
}
