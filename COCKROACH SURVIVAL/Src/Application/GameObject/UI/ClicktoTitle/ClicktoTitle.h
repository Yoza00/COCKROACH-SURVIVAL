#pragma once

#include"../UI.h"

class ClicktoTitle :public UI
{
public:

	ClicktoTitle()					{}
	~ClicktoTitle()		override	{}

	void Update()		override;
	void DrawSprite()	override;

private:

	int			m_timeToActive		= 120;
	float		m_alpha				= 0.0f;		// 色の濃さ
	const float	m_alphaIncreasePow	= 0.016f;	// 濃さの変化の度合い

	bool		m_isActive			= false;	// 起動フラグ
	bool		m_isAlphaIncrease	= false;	// 濃さを増加させるかどうか

	void AlphaUpdate();		// 濃さを更新

};