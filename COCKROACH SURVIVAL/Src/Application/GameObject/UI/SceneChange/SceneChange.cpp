#include"SceneChange.h"

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

	Math::Color	_color = { 0.0f,0.0f,0.0f,m_alpha };
	
	KdShaderManager::Instance().m_spriteShader.
		DrawTex(m_spTex, static_cast<int>(m_drawPos.x), static_cast<int>(m_drawPos.y), m_rec.width, m_rec.height, &m_rec, &_color);
}

void SceneChange::ChangeScene()
{
	if (m_isTitleBack)
	{
		SceneManager::Instance().SetNextScene(
			SceneManager::SceneType::Title
		);

		return;
	}

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
