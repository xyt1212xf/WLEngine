-- UnLua 启动入口
-- 由 UnLua 设置中的 StartupModuleName 指定（Config/DefaultUnLuaSettings.ini）
-- Lua 环境创建时会自动 require 本模块

print("[Main] UnLua startup module loaded")

-- ===== LuaPanda 调试开关 =====
-- 调试时确保为 true；正式打包前改成 false 或删除 StartupModuleName 配置
local ENABLE_LUAPANDA = true


local function StartDebugger()
    if not ENABLE_LUAPANDA then
        return
    end

    local ok, debugger = pcall(require, "Debug.LuaPanda")
    if not ok then
        print("[Main] LuaPanda load failed: " .. tostring(debugger))
        return
    end

    -- UE 作为客户端主动连接 VSCode 里 LuaPanda 监听的端口
    -- VSCode 没启动调试时连接会失败，LuaPanda 会自动重试，不阻塞游戏
    debugger.start("127.0.0.1", 8818)
end

StartDebugger()
