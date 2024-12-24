#pragma once

#include"../UI.h"

class ClickUI :public UI
{
public:

	ClickUI()										{}
	~ClickUI()							override	{}

	void Update()						override;

	// クリックされたかどうかを返す
	const bool GetIsClicked()const
	{
		return m_isClicked;
	}

protected:

	bool	m_isClicked = false;		// クリックされたかどうか
	bool	m_isKey		= false;

	// マウスでUIがクリックされたかどうかの確認
	void ClickCheck();

	// ボタンがクリックされた際に使用する関数
	virtual void ButtonClicked() {}
};