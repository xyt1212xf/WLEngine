require("script/frame/MainFrameInc")
require("script/frame/MsgInc")	
require("script/MainPlayer")

Fight1 = CGuiEntity.New()


function Fight1:Init()
	Fight1:setLuaEntityName("Fight1")
	Fight1:initialise("resource/Gui/Fight1Wnd.gui")
	Fight1:setVisible(true)
end

Fight1:Init()
return Fight1

