require("script/RoleHeader")

Roles = 
{
    ["BaWangWan"] = 
    {     
        state = 
        {
            [RoleStateEnum.stand]=
            {
                id = RoleStateEnum.stand,   --状态值
                name = "站立",			    --名称
                frameName = "resource/Role/BaWangWan/0_",           --动作起始帧的名称
                frameCount = 10,
                speed = 0.26,		        --播放速度
                isLoop = true,              --是否循环播放动作
                interrupt = 999,	        --主动状态多少帧之前可以打断
                beAttacked = 999,	        --被动状态多少帧之前可以打断
                moveSpeed = {x=0,y=0},      --移动速度
            },
            [RoleStateEnum.walk]=
            {
                id = RoleStateEnum.walk,   --状态值
                name = "前移",			    --名称
                frameName = "resource/Role/BaWangWan/12_",           --动作起始帧的名称
                frameCount = 8,
                speed = 0.26,		        --播放速度
                isLoop = true,              --是否循环播放动作
                interrupt = 999,	        --主动状态多少帧之前可以打断
                beAttacked = 999,	        --被动状态多少帧之前可以打断    
                moveSpeed = {x=4,y=0},      --移动速度
            },
            [RoleStateEnum.back]=
            {
                id = RoleStateEnum.back,    --状态值
                name = "后退",			    --名称
                frameName = "resource/Role/BaWangWan/14_",          --动作起始帧的名称
                frameCount = 8,
                speed = 0.26,		        --播放速度
                isLoop = true,              --是否循环播放动作
                interrupt = 999,	        --主动状态多少帧之前可以打断
                beAttacked = 999,	        --被动状态多少帧之前可以打断
                moveSpeed = {x=-3,y=0},      --移动速度
            },

            [RoleStateEnum.jump]=
            {
                id = RoleStateEnum.jump,    --状态值
                name = "跳",  			    --名称
                frameName = "resource/Role/BaWangWan/16_",          --动作起始帧的名称
                frameCount = 4,
                speed = 0.62,		        --播放速度
                isLoop = false,             --是否循环播放动作
                interrupt = 999,	        --主动状态多少帧之前可以打断
                beAttacked = 999,	        --被动状态多少帧之前可以打断
                moveSpeed = {x=0,y=-1},     --移动速度
            },

            [RoleStateEnum.down]=
            {
                id = RoleStateEnum.down,    --状态值
                name = "下落",			    --名称
                frameName = "resource/Role/BaWangWan/18_",          --动作起始帧的名称
                frameCount = 3,
                speed = 0.26,		        --播放速度
                isLoop = false,             --是否循环播放动作
                interrupt = 999,	        --主动状态多少帧之前可以打断
                beAttacked = 999,	        --被动状态多少帧之前可以打断
                moveSpeed = {x=0,y=3},      --移动速度
            },

            [RoleStateEnum.crouch]=
            {
                id = RoleStateEnum.crouch,    --状态值
                name = "下落",			    --名称
                frameName = "resource/Role/BaWangWan/36_",          --动作起始帧的名称
                frameCount = 1,
                speed = 0.26,		        --播放速度
                isLoop = false,             --是否循环播放动作
                interrupt = 999,	        --主动状态多少帧之前可以打断
                beAttacked = 999,	        --被动状态多少帧之前可以打断
                moveSpeed = {x=0,y=0},      --移动速度
            },

            [RoleStateEnum.attack1]=
            {
                id = RoleStateEnum.attack1,    --状态值
                name = "轻攻击",			    --名称
                frameName = "resource/Role/BaWangWan/161_",          --动作起始帧的名称
                frameCount = 9,
                speed = 1,	   	            --播放速度
                isLoop = false,             --是否循环播放动作
                interrupt = -1,	            --主动状态多少帧之前可以打断
                beAttacked = 999,	        --被动状态多少帧之前可以打断
                moveSpeed = {x=0,y=0},      --移动速度
            },
            [RoleStateEnum.attack2]=
            {
                id = RoleStateEnum.attack2,    --状态值
                name = "中攻击",			    --名称
                frameName = "resource/Role/BaWangWan/163_",          --动作起始帧的名称
                frameCount = 9,
                speed = 1,	   	            --播放速度
                isLoop = false,             --是否循环播放动作
                interrupt = -1,	            --主动状态多少帧之前可以打断
                beAttacked = 999,	        --被动状态多少帧之前可以打断
                moveSpeed = {x=0,y=0},      --移动速度
            },
        },
        blood = 100,                        --血量
        attack = 2,                         --攻击力
        defense = 15,                       --防御力        
    },
    ["YaShenHuanShiLang"] = 
    {     
        state = 
        {
            [RoleStateEnum.stand]=
            {
                stateName = "",		        --
                name = "",			        --名称
                nFrameCount = 0,	        --一共多少帧
                speed = 0.26,		        --播放速度
                interrupt = 999,	        --主动状态多少帧之前可以打断
                beAttacked = 990,	        --被动状态多少帧之前可以打断
            },
        },
        blood = 100,                        --血量
        attack = 2,                         --攻击力
        defense = 15,                       --防御力        
    },
    
}