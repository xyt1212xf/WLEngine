#pragma once
namespace WL
{
	enum WndType
	{
		ui_wnd = 0,				//����
		ui_button = 1,			//��ť
		ui_image = 5,			//ͼƬ
		//ui_checkbox = 2,		//��ѡ
		//ui_progress = 3,		//������
		//ui_list = 4,			//�б�
		//ui_dynamicimage = 6,	//��̬ͼƬ
		//ui_editBox = 7,		//�ı�
		ui_label = 8,			//��̬�ı�
		//ui_minwnd = 9,		//С����
		//ui_mouse = 10,		//���
		//ui_radio = 11,		//��ѡ
		//ui_slide = 12,		//������
		//ui_actionimage = 13,   //�����ؼ�
		//ui_verslide = 14,		//������
		//ui_skill = 15,        //����CD�ؼ�
		//ui_progressBar = 16,  //�������ؼ�
		//ui_dropList = 17,		//�����˵�
		//ui_horSlide = 18,		//ˮƽ������
		//ui_comboBox = 19,		//�����˵�
		//ui_menu = 20,			//�˵�
		//ui_font = 21,
		//ui_tableWnd = 22,
		ui_animation = 23,		//�����ؼ�
		//ui_all,
		selfwnd = 0x0FFFFFFE,
	};
}