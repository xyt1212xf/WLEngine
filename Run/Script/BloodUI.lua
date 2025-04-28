require("script/frame/MainFrameInc")
require("script/frame/MsgInc")	
require("script/MainPlayer")

Blood = CGuiEntity.new()


function Blood:init()
	Blood:setLuaEntityName("Blood")
	Blood:initialise("resource/Gui/BloodWnd.gui")
	Blood:setVisible(true)
end

Blood:init()
return Blood

