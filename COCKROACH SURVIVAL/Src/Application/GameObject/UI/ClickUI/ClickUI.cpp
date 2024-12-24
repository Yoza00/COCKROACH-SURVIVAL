#include "ClickUI.h"

void ClickUI::Update()
{
	if (!m_isDraw)return;
	if (!m_isMenu)return;
	if (!m_isButtonActive)return;

	// 左クリックされたら、クリックに対応する処理を行う
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (!m_isKey)
		{
			m_isKey = true;

			ClickCheck();
		}
	}
	else
	{
		m_isKey = false;
	}

	// クリックされていたら、対応する処理を行う
	if (m_isClicked)
	{
		m_isClicked = false;

		// クリックされた結果の処理を実行
		ButtonClicked();
	}
}

void ClickUI::ClickCheck()
{
	POINT _mouse;				// マウス座標
	
	GetCursorPos(&_mouse);		// マウス座標格納

	// スクリーンの影響範囲内に指定する
	ScreenToClient(GetActiveWindow(), &_mouse);

	// UI座標にマウス座標を合わせる
	// マウス座標は左上が原点であるが、UIの描画は(0,0)なので補正する必要がある
	// もし左上でクリック(マウス座標でいう{0,0}の座標の場合、UI座標は{-640,360}である)
	float _uiX = _mouse.x - 640.0f;
	float _uiY = 360.0f - _mouse.y;

	bool _isClicked = false;

	// クリック座標をもとに、どのUIがクリックされたかを判定する
	// UIのpivotを真ん中の左側に設定しているため、横は描画座標から画像の横幅の範囲で判定する
	// 横幅の確認
	if (_uiX > m_drawPos.x && _uiX < m_drawPos.x + m_width)
	{
		// 縦幅の確認
		// 上下は画像の高さの半分の範囲で判定する
		// クリックされた座標をもとに、上半分の判定を行うか下半分の判定を行うかの判定を行う
		// 画像上半分の判定
		if (_uiY > m_drawPos.y)
		{
			if (_uiY < m_drawPos.y + (m_height / 2))
			{
				_isClicked = true;
			}
		}
		// 下半分の判定
		else
		{
			if (_uiY > m_drawPos.y - (m_height / 2))
			{
				_isClicked = true;
			}
		}
	}

	// 結果をメンバー変数に格納
	m_isClicked = _isClicked;
}
