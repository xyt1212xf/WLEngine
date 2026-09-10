#include "UIFactory.h"
#include "WLEngine.h"
#include "UIWnd.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UILabel.h"
#include "UIAnimation.h"

namespace WL
{
	CUIWnd* CUIFactory::createUI(WndType type)
	{
		CUIWnd* pWnd = nullptr;
		switch (type)
		{
		case ui_wnd:
		{
			pWnd = WL_NEW(CUIWnd, Wnd);
			pWnd->mTypeName = "UIWnd";
		}
		break;
		case ui_animation:
		{
			pWnd = WL_NEW(CUIAnimation, Wnd);
			pWnd->mTypeName = "UIAnimation";
		}
		break;
		case ui_button:
		{
			pWnd = WL_NEW(CUIButton, Wnd);
			pWnd->mTypeName = "UIButton";
		}
		break;
		case ui_image:
		{
			pWnd = WL_NEW(CUIImage, Wnd);
			pWnd->mTypeName = "UIImage";
		}
		break;
		case ui_label:
		{
			pWnd = WL_NEW(CUILabel, Wnd);
			pWnd->mTypeName = "UILabel";
		}
		break;
		}
		return pWnd;
	}
}


