#pragma once

#include"../UI.h"

// 食べ物の前にいるときに描画する操作ガイドのクラス
class EatTips :public UI
{
public:

	EatTips()						{}
	~EatTips()			override	{}

	void DrawSprite()	override;

	// 描画フラグをセット
	void SetIsDraw(bool isDraw)
	{
		m_isDraw = isDraw;
	}

private:

	bool			m_isDraw = false;		// 描画フラグ

};