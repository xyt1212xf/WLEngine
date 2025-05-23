#pragma once
#include "Common.h"

#define MSG_MOUSEFIRST                   0x0200
#define MSG_MOUSEMOVE                    0x0200
#define MSG_LBUTTONDOWN                  0x0201
#define MSG_LBUTTONUP                    0x0202
#define MSG_LBUTTONDBLCLK                0x0203
#define MSG_RBUTTONDOWN                  0x0204
#define MSG_RBUTTONUP                    0x0205
#define MSG_RBUTTONDBLCLK                0x0206
#define MSG_MBUTTONDOWN                  0x0207
#define MSG_MBUTTONUP                    0x0208
#define MSG_MBUTTONDBLCLK                0x0209
#define MSG_MOUSEWHEEL                   0x020A


#define MSG_KEYFIRST                     0x0100
#define MSG_KEYDOWN                      0x0100
#define MSG_KEYUP                        0x0101
#define MSG_CHAR                         0x0102
#define MSG_DEADCHAR                     0x0103
#define MSG_SYSKEYDOWN                   0x0104
#define MSG_SYSKEYUP                     0x0105
#define MSG_SYSCHAR                      0x0106
#define MSG_SYSDEADCHAR                  0x0107
#define MSG_UNICHAR                      0x0109
#define MSG_KEYLAST                      0x0109

namespace WL
{
	//消息的响应事件
	struct SEvent
	{
		bool bProcess = false;
		int message = 0;
		int mouseX = 0;
		int mouseY = 0;
		INT64 wParam = 0;
		INT64 lParam = 0;
	};
}
