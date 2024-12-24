#include "BackButton.h"

#include"../../Menu_Icon/Menu_Icon.h"

void BackButton::ButtonClicked()
{
	const std::shared_ptr<Menu_Icon>	_spMIcon = m_wpMIcon.lock();
	
	// 情報の取得ができなければリターン
	if (!_spMIcon)return;

	// メニューフラグを解除する必要があるので、Menu_Iconクラスのm_isMenuフラグを解除する
	_spMIcon->SetIsMenu(false);
}
