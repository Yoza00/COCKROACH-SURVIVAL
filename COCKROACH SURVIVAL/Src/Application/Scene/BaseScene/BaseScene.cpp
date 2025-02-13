#include "BaseScene.h"

// UI用
#include"../../GameObject/UI/UI.h"

void BaseScene::PreUpdate()
{
	// Updateの前の更新処理
	// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
	auto it = m_objList.begin();

	while (it != m_objList.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_objList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}

	// 不要なUIオブジェクトの削除
	auto _uis = m_UIList.begin();

	while (_uis != m_UIList.end())
	{
		if ((*_uis)->IsExpired())
		{
			_uis = m_UIList.erase(_uis);
		}
		else
		{
			++_uis;
		}
	}

	// ↑の後には有効なオブジェクトだけのリストになっている

	for (auto& obj : m_objList)
	{
		obj->PreUpdate();
	}

	for (auto& ui : m_UIList)
	{
		ui->PreUpdate();
	}
}

void BaseScene::Update()
{
	// ポーズ時の処理を行う場合は、このif文中で処理を記載し、処理終了時にリターンを行って、関数内の処理全てが実行されないようにしておく
	if (m_isPause)
	{
		// ここでポーズ中の処理を行う
		for (auto& ui : m_UIList)
		{
			// メニュー画面かどうかを確認
			// メニュー状態になっていない
			if (ui->GetIsMenu() == false)
			{
				ui->SetIsMenu(true);					// メニュー状態に変更
			}

			// 画面更新
			ui->Update();
		}

		return;
	}

	// KdGameObjectを継承した全てのオブジェクトの更新 (ポリモーフィズム)
	for (auto& obj : m_objList)
	{
		obj->Update();
	}

	// シーン毎のイベント処理
	Event();
}

void BaseScene::PostUpdate()
{
	if (m_isPause)return;

	for (auto& obj : m_objList)
	{
		obj->PostUpdate();
	}

	for (auto& ui : m_UIList)
	{
		ui->PostUpdate();
	}
}

void BaseScene::PreDraw()
{
	if (m_uniCamera)
	{
		m_uniCamera->SetToShader();
	}

	for (auto& obj : m_objList)
	{
		obj->PreDraw();
	}

	for (auto& ui : m_UIList)
	{
		ui->PreDraw();
	}
}

void BaseScene::Draw()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光を遮るオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginGenerateDepthMapFromLight();
	{
		for (auto& obj : m_objList)
		{
			obj->GenerateDepthMapFromLight();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndGenerateDepthMapFromLight();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のあるオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawLit();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクト(透明な部分を含む物体やエフェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawUnLit();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光源オブジェクト(自ら光るオブジェクトやエフェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_postProcessShader.BeginBright();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawBright();
		}
	}
	KdShaderManager::Instance().m_postProcessShader.EndBright();
}

void BaseScene::DrawSprite()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 2Dの描画はこの間で行う
	KdShaderManager::Instance().m_spriteShader.Begin();
	{
		// 通常の描画オブジェクトを描画
		for (auto& obj : m_objList)
		{
			obj->DrawSprite();
		}

		// ポーズメニューは通常のオブジェクト描画が完了したら実行する
		// 描画順を意識する
		// ポーズ中のみ実行
		if (m_isPause)
		{
			for (auto& obj : m_UIList)
			{
				obj->DrawSprite();
			}
		}
		
	}
	KdShaderManager::Instance().m_spriteShader.End();
}

void BaseScene::DrawDebug()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// デバッグ情報の描画はこの間で行う
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawDebug();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();
}

void BaseScene::Event()
{
	// 各シーンで必要な内容を実装(オーバーライド)する
}

void BaseScene::Init()
{
	// 各シーンで必要な内容を実装(オーバーライド)する
}
