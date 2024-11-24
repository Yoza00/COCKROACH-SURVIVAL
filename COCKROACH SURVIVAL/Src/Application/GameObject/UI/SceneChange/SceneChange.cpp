#include "SceneChange.h"

#include"../../../Scene/SceneManager.h"

void SceneChange::Update()
{
	if (!m_spTex)return;

	if (m_isIncrease)
	{
		m_alpha += m_alphaPow;

		if (m_alpha > 1.0f)
		{
			m_isIncrease = false;

			ChangeScene();
		}
	}
	else
	{
		m_alpha -= m_alphaPow;

		if (m_alpha < 0.0f)
		{
			m_isExpired = true;
		}
	}
}

void SceneChange::DrawSprite()
{
	if (!m_spTex)return;

	// 切り取り範囲を更新
	Math::Rectangle	_rec = { long(m_startPos.x),long(m_startPos.y),long(m_width),long(m_height) };

	// 切り取り範囲をもとに描画
	// 更新された切り取り範囲を利用して描画
	Math::Color	_color = { 0.0f,0.0f,0.0f,m_alpha };
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_drawPos.x, m_drawPos.y, _rec.width, _rec.height, &_rec, &_color, { 0.5f,0.5f });
}

void SceneChange::Init()
{
	UI::Init();
}

void SceneChange::ChangeScene()
{
	switch (SceneManager::Instance().GetSceneType())
	{
	case SceneManager::SceneType::Title:
		SceneManager::Instance().SetNextScene(
			SceneManager::SceneType::Game
		);
		break;
	case SceneManager::SceneType::Game:
		SceneManager::Instance().SetNextScene(
			SceneManager::SceneType::Result
		);
		break;
	case SceneManager::SceneType::Result:
		SceneManager::Instance().SetNextScene(
			SceneManager::SceneType::Title
		);
		break;
	default:
		break;
	}
}
