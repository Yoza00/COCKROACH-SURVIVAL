#pragma once

#include"../UI.h"

class SceneChange :public UI
{
public:

	SceneChange()					{}
	~SceneChange()		override	{}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

private:

	void ChangeScene();

	float		m_alpha			= 0.0f;			// 濃さ
	const float	m_alphaPow		= 0.016f;		// 濃さを加算する値

	bool		m_isIncrease	= true;			// 値を加算する？しない場合は減算
};