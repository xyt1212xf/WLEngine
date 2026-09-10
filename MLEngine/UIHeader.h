#pragma once
namespace WL
{
	enum WndType
	{
		ui_wnd = 0,				//窗口
		ui_button = 1,			//按钮
		ui_image = 5,			//图片
		//ui_checkbox = 2,		//复选
		//ui_progress = 3,		//进度条
		//ui_list = 4,			//列表
		//ui_dynamicimage = 6,	//动态图片
		//ui_editBox = 7,		//文本
		ui_label = 8,			//静态文本
		//ui_minwnd = 9,		//小窗口
		//ui_mouse = 10,		//鼠标
		//ui_radio = 11,		//单选
		//ui_slide = 12,		//滑动条
		//ui_actionimage = 13,   //动画控件
		//ui_verslide = 14,		//竖滑条
		//ui_skill = 15,        //技能CD控件
		//ui_progressBar = 16,  //进度条控件
		//ui_dropList = 17,		//下拉菜单
		//ui_horSlide = 18,		//水平滚动条
		//ui_comboBox = 19,		//下拉菜单
		//ui_menu = 20,			//菜单
		//ui_font = 21,
		//ui_tableWnd = 22,
		ui_animation = 23,		//动画控件
		//ui_all,
		selfwnd = 0x0FFFFFFE,
	};
}