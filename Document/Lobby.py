import asyncio
import json
import subprocess
import secrets
import string
import socket
import time
import random
from datetime import datetime
from pathlib import Path
from urllib.parse import urlencode
from urllib.request import urlopen

try:
    import websockets
except ImportError:
    raise SystemExit("缺少依赖，先执行: pip install websockets")

try:
    import aiomysql
except ImportError:
    raise SystemExit("缺少依赖，先执行: pip install aiomysql")

HOST = "0.0.0.0"
PORT = 8081


def get_local_ip() -> str:
    """取本机在局域网中的 IP（用于下发给 DS 连接匹配服）。
    UDP connect 不发包，只是让系统选出到外网所用的网卡地址；
    取不到时回落 127.0.0.1（DS 与本机同机部署时仍可用）。"""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(("8.8.8.8", 80))
        return s.getsockname()[0]
    except Exception:
        return "127.0.0.1"
    finally:
        s.close()


# 匹配服自身地址（下发给 DS，替代 C++ 里写死的 IP）
MATCH_SERVER_HOST = get_local_ip()
MATCH_SERVER_URL = f"ws://{MATCH_SERVER_HOST}:{PORT}"

DS_HOST = "127.0.0.1"      # 推给客户端的 DS 公网地址
DS_DIR = Path(__file__).resolve().parent / "WindowsServer"
DS_BAT = DS_DIR / "DServer.bat"
DS_EXE = DS_DIR / "BlackServer.exe"

MATCH_NEED = 1                 # 几人匹配（同时作为 -MaxPlayerNum 下发给 DS，两者必须一致）
DS_START_TIMEOUT = 30          # 等待 DS 就绪的最大秒数

# MySQL 配置
DB_CONFIG = {
    "host": "127.0.0.1",
    "port": 3306,
    "user": "root",
    "password": "gEgaEKQSjWS83q7",  # 改成你的 MySQL 密码
    "db": "black56",
    "charset": "utf8mb4",
    "autocommit": True,
    # 连接池参数（create_pool(**DB_CONFIG) 会透传给 aiomysql）
    "minsize": 10,          # 常驻连接数，启动即建好，避免突发请求时现场建连
    "maxsize": 50,          # 峰值上限；单进程 py + 本机 MySQL 的合理值
    "pool_recycle": 3600,   # 连接最长存活 1 小时，防止 MySQL wait_timeout 静默断连报 2006/2013
}

# ============================================================
# Steam 票据校验配置（必填）
# ============================================================
# Publisher Web API Key：Steamworks 后台 -> 用户与权限 -> 管理组 -> Web API Key
# 注意：这是 5418590 的 Publisher Key，只能用于校验 5418590 的票据。
STEAM_WEB_API_KEY = "7085E6231A690C9014306CCC785AC99B"

# ---- AppID：两套配置要一起切换，不能只改一边 ----
#   [本地快速自测]  STEAM_APP_ID = 480   —— 用 Steam 自带的 Spacewar 测试 App，
#                   任何 Steam 账号天生拥有它，无需申请 key，SteamAPI 直接初始化成功。
#                   但：480 的票据不能用上面的 5418590 Key 去验（key 与 appid 不匹配），
#                   所以此时必须把 VERIFY_STEAM_TICKET 设为 False（只收票、不验票）。
#                   客户端侧同步改成：steam_appid.txt = 480，
#                   以及 Config/DefaultEngine.ini 的 SteamDevAppId / SteamAppId = 480。
#
#   [正式联调/上线] STEAM_APP_ID = 5418590 —— 你自己的 AppId。
#                   需要先在 Steamworks 后台给测试账号发 Developer Autogrant key 并激活，
#                   让账号库里出现 5418590；客户端侧 steam_appid.txt 与 DefaultEngine.ini 也改回 5418590；
#                   然后把 VERIFY_STEAM_TICKET 设为 True 才会真正调用 Steam 验票。
STEAM_APP_ID = 5579860               # 已申请到的正式 AppId，与 DefaultEngine.ini 的 SteamDevAppId 一致

# 是否调用 Steam Web API 校验票据（上线务必 True）
#   - 用 480 自测时：必须 False（否则 5418590 的 Key 验不了 480 的票，会全失败）
#   - 用 5418590 联调/上线时：必须 True（否则等于没验票，任何人可伪造 steam_id）
VERIFY_STEAM_TICKET = False       # TODO: 正式联调/上线改为 True

# 技能卡池：C++ GetSkillCardBag()（action=learnedSkills）触发抽卡
#   普通池随机 8 个不重复 + 稀有池随机 2 个不重复，结果逐条写入 player_item 表
SKILL_POOL_NORMAL = [
    "10007", "10021","10003","10005","10009","10012","10046","10047","10014","10044",
    "10001", "10002","10022","10019","10008","10010","10026","10029","10024","10011",
    "10018", "10030","10004","10006","10023","10025","10027","10016","10017","10035",
]
SKILL_POOL_RARE = ["15045", "15046", "15049", "15057"]

# 端口池配置
DS_PORT_MIN = 7777
DS_PORT_MAX = 8777
_available_ports = set(range(DS_PORT_MIN, DS_PORT_MAX + 1))
_used_ports = {}

players = {}
match_queue = []
pending_matches = {}
ds_connections = {}
# 孤儿 DS：py 崩溃重启后，重启前启动的 DS 会重连并上报 match_id + port
# 结构: match_id -> {"ws": <连接>, "port": int, "since": float}
orphan_ds = {}

_match_lock = asyncio.Lock()
db_pool = None  # 全局数据库连接池


def now():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")


def generate_token() -> str:
    """生成 16 位随机会话 token（大小写字母+数字，约 8e28 种组合，无法预测/猜解）"""
    alphabet = string.ascii_letters + string.digits
    return "".join(secrets.choice(alphabet) for _ in range(16))


def get_client_ip(ws) -> str:
    """从 WebSocket 连接取客户端 IP（websockets 17.x 的 remote_address 可能是 tuple/list/str）"""
    peer = getattr(ws, "remote_address", None)
    if isinstance(peer, (tuple, list)) and peer:
        return str(peer[0])
    if isinstance(peer, str):
        return peer
    return ""


async def send_json(ws, obj):
    try:
        msg = json.dumps(obj, ensure_ascii=False)
        await ws.send(msg)
        print(f"[{now()}] 📤 发送: {msg}")
    except Exception as e:
        print(f"[{now()}] ❌ 发送失败: {e}")


def get_token_player(ws, data):
    """统一校验 C++ 消息里的 token 字段。

    约定：C++ 发来的所有 token 字段都是 login_ok 下发的会话 token，
    不是 Steam ticket；ticket 只在 login 阶段用于换 token。
    校验条件：token 存在于 players 中，且必须属于当前 WebSocket 连接。
    返回 (玩家信息字典, token)；失败返回 (None, token或空串)。
    """
    action = data.get("action", "?")
    token = data.get("token", "")
    if not isinstance(token, str) or not token:
        print(f"[{now()}] ⚠️ [{action}] 缺少/非法 token")
        return None, ""

    p = players.get(token)
    if not p or p.get("ws") is not ws:
        print(f"[{now()}] ⚠️ [{action}] token 无效"
              f"（只认 login_ok 下发的会话 token，且 token 必须属于当前连接）")
        return None, token
    return p, token


def is_port_free(port: int) -> bool:
    """探测 UDP 端口是否空闲。
    DS 用 UDP 监听游戏端口，py 重启后即使孤儿 DS 还没来得及上报，
    这里也能发现端口仍被占用，避免分配出去导致两个 DS 抢同一端口。"""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.bind((HOST, port))
        return True
    except OSError:
        return False
    finally:
        s.close()


def alloc_ds_port() -> int | None:
    while _available_ports:
        port = min(_available_ports)
        _available_ports.remove(port)
        if is_port_free(port):
            return port
        # 系统层面仍被占用：多半是 py 崩溃前启动、还没重连上报的 DS，跳过并标记占用
        _used_ports[port] = "orphan"
        print(f"[{now()}] ⚠️ 端口 {port} 仍被系统占用（残留 DS？），跳过")
    print(f"[{now()}] ❌ 端口池已耗尽 ({DS_PORT_MIN}-{DS_PORT_MAX})")
    return None


def release_ds_port(port: int):
    if DS_PORT_MIN <= port <= DS_PORT_MAX:
        _available_ports.add(port)
        _used_ports.pop(port, None)
        print(f"[{now()}] ♻️ 端口 {port} 已回收，当前可用: {len(_available_ports)}/{DS_PORT_MAX - DS_PORT_MIN + 1}")
    else:
        print(f"[{now()}] ⚠️ 端口 {port} 不在范围内，跳过回收")


def start_ds_for_match(match_id: str, port: int) -> subprocess.Popen | None:
    if not DS_EXE.exists():
        print(f"[{now()}] ❌ 找不到 BlackServer.exe: {DS_EXE}")
        return None

    try:
        proc = subprocess.Popen(
            [
                str(DS_EXE),
                "/Game/UI/main/Maps/GameReady?listen",
                "-server",
                "-log",
                "-nosteam",
                f"-port={port}",
                f"-match_id={match_id}",
                f"-match_server={MATCH_SERVER_URL}",   # 把 py 自己的 IP 告诉 DS，C++ 里不再写死
                f"-MaxPlayerNum={MATCH_NEED}",         # 最大人数（与 MATCH_NEED 一致）
            ],
            cwd=str(DS_DIR),
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            encoding="utf-8",
            errors="ignore",
        )
        _used_ports[port] = match_id
        print(f"[{now()}] 🚀 已启动 DS match_id={match_id} pid={proc.pid} port={port}")
        print(f"[{now()}] 📊 端口占用: {len(_used_ports)}/{DS_PORT_MAX - DS_PORT_MIN + 1}")
        return proc
    except Exception as e:
        print(f"[{now()}] ❌ 启动 DS 失败: {e}")
        release_ds_port(port)
        return None


def stop_ds_by_match(match_id: str):
    """关闭指定 match_id 的 DS 进程并回收端口"""
    info = pending_matches.get(match_id)
    if not info:
        print(f"[{now()}] ⚠️ 找不到待处理的 match_id={match_id}")
        return

    port = info.get("port", 0)
    proc = info.get("proc")
    
    if proc and proc.poll() is None:
        pid = proc.pid
        print(f"[{now()}] 🛑 正在关闭 DS match_id={match_id} pid={pid} port={port}")
        
        # Windows 上用 taskkill 强制杀进程树
        try:
            subprocess.run(
                ["taskkill", "/F", "/T", "/PID", str(pid)],
                capture_output=True,
                timeout=5
            )
            print(f"[{now()}] ✅ DS 已关闭 match_id={match_id}")
        except Exception as e:
            print(f"[{now()}] ⚠️ taskkill 失败: {e}")
            # 备用方案
            try:
                proc.kill()
                proc.wait(timeout=3)
            except:
                pass

    if port:
        release_ds_port(port)

    pending_matches.pop(match_id, None)
    ds_connections.pop(match_id, None)


async def try_start_match():
    """尝试开始匹配：检查队列是否足够，够了就启动 DS"""
    async with _match_lock:
        # 清理队列里断线的
        alive_queue = []
        for t in match_queue:
            p = players.get(t)
            if p and p.get("ws"):
                try:
                    if p["ws"].state.name == "CLOSED":
                        print(f"[{now()}] 🧹 队列清理断线 token={t}")
                        continue
                except:
                    pass
                alive_queue.append(t)
            else:
                print(f"[{now()}] 🧹 队列清理断线 token={t}")
        match_queue[:] = alive_queue

        print(f"[{now()}] 👥 当前队列: {len(match_queue)}/{MATCH_NEED}")

        if len(match_queue) < MATCH_NEED:
            return

        batch = match_queue[:MATCH_NEED]
        match_queue[:] = match_queue[MATCH_NEED:]

        player_sockets = []
        steam_ids = []
        for t in batch:
            p = players.get(t)
            if p and p.get("ws"):
                player_sockets.append(p["ws"])
                steam_ids.append(p["steam_id"])

        if len(player_sockets) < MATCH_NEED:
            print(f"[{now()}] ⚠️ 匹配失败: 部分玩家已断线")
            return

        port = alloc_ds_port()
        if port is None:
            print(f"[{now()}] ❌ 无法分配端口，匹配取消")
            for ws in player_sockets:
                try:
                    await send_json(ws, {
                        "type": "match_cancelled",
                        "message": "服务器繁忙，请稍后重试"
                    })
                except:
                    pass
            return

        match_id = f"match_{int(time.time())}_{random.randint(1000, 9999)}"

        pending_matches[match_id] = {
            "player_sockets": player_sockets,
            "steam_ids": steam_ids,
            "ds_ws": None,
            "ready": False,
            "port": port,
            "proc": None,
            "confirmed": set(),    # 已确认进入游戏（joinGame isGame=true）的 steam_id 集合
            "created_at": time.time()
        }

        print(f"[{now()}] 🎯 匹配成功: {steam_ids} match_id={match_id} port={port}"
              f"（等待全员 SendJoinGame(true) 确认后再启动 DS）")

        # 匹配成功：向所有参战玩家广播 joinResult result=0
        # DS 不在这里启动，等全员 joinGame isGame=true 后由 handle_join_game 启动；
        # DS 就绪后仍按原流程下发 match_found 带连接地址，客户端收到后 ClientTravel 进战局
        for pws in player_sockets:
            try:
                await send_json(pws, {
                    "type": "joinResult",
                    "result": 0,           # 枚举: 0=匹配成功
                    "match_id": match_id,
                    "total_needed": MATCH_NEED,
                    "message": "匹配成功"
                })
            except:
                pass


async def handle_join_game(ws, data):
    """C++ SendJoinGame(bOk) 发来: action=joinGame, token=<py 会话 token>, isGame=bool
    isGame=true  -> 确认进入游戏；本组全部 MATCH_NEED 名玩家都确认后启动 DS
    isGame=false -> 本组所有匹配成功的玩家自动取消，向全组广播 joinGameFailed"""
    p, _ = get_token_player(ws, data)
    if not p:
        await send_json(ws, {"type": "error", "message": "joinGame: 无效 token（只接受 login_ok 下发的 token）"})
        return

    is_game = bool(data.get("isGame", False))
    steam_id = p["steam_id"]

    # 找到该玩家所在且未失败的匹配组
    target_match_id = None
    for mid, info in pending_matches.items():
        if steam_id in info.get("steam_ids", []):
            target_match_id = mid
            break
    if not target_match_id:
        await send_json(ws, {"type": "error", "message": "joinGame: 不在任何匹配组中"})
        return

    info = pending_matches[target_match_id]
    print(f"[{now()}] 🎮 joinGame: steam_id={steam_id} isGame={is_game} match_id={target_match_id} "
          f"已确认={len(info['confirmed'])}/{len(info['steam_ids'])}")

    if not is_game:
        # 一票否决：整组取消，回收端口，全组广播 joinGameFailed
        info["failed"] = True
        sockets = info.get("player_sockets", [])
        port = info.get("port")
        pending_matches.pop(target_match_id, None)
        if port:
            release_ds_port(port)
        print(f"[{now()}] ❌ joinGame 拒绝: match_id={target_match_id} 整组解散（{len(sockets)}名玩家）")
        for pws in sockets:
            try:
                await send_json(pws, {
                    "type": "joinGameFailed",
                    "match_id": target_match_id,
                    "reason": 1,               # 1=玩家取消
                    "message": "有玩家取消进入游戏，本组匹配已解散"
                })
            except:
                pass
        return

    # isGame=true：记录确认（set 自动去重，重复确认无副作用）
    info["confirmed"].add(steam_id)
    if len(info["confirmed"]) >= len(info["steam_ids"]):
        print(f"[{now()}] 🚀 全员确认进入游戏 match_id={target_match_id}，启动 DS")
        proc = start_ds_for_match(target_match_id, info["port"])
        if not proc:
            print(f"[{now()}] ❌ DS 启动失败 match_id={target_match_id}，整组解散")
            sockets = info.get("player_sockets", [])
            port = info.get("port")
            pending_matches.pop(target_match_id, None)
            if port:
                release_ds_port(port)
            for pws in sockets:
                try:
                    await send_json(pws, {
                        "type": "joinGameFailed",
                        "match_id": target_match_id,
                        "reason": 2,           # 2=DS 启动失败
                        "message": "服务器启动失败，请稍后重试"
                    })
                except:
                    pass
            return
        info["proc"] = proc
        # DS 启动后才进入"等 DS 就绪"阶段（超时由 wait_ds_ready_timeout 负责）
        asyncio.create_task(wait_ds_ready_timeout(target_match_id))


async def handle_ds_ready(ws, data):
    """处理 DS 发来的就绪/注册消息：action=ds_ready, match_id=xxx, port=7777"""
    match_id = data.get("match_id", "")
    port = data.get("port", 0)
    if not isinstance(port, int):
        try:
            port = int(port)
        except (TypeError, ValueError):
            port = 0

    if match_id not in pending_matches:
        # py 崩溃重启后，重启前启动的 DS 会重连并上报自己正在使用的端口。
        # 这里登记为"孤儿 DS"，把端口从可用池里划走，防止再分配给新比赛。
        if DS_PORT_MIN <= port <= DS_PORT_MAX:
            _available_ports.discard(port)
            _used_ports[port] = match_id
            orphan_ds[match_id] = {"ws": ws, "port": port, "since": time.time()}
            print(f"[{now()}] 🔗 孤儿 DS 上报 match_id={match_id} port={port}，已登记占用"
                  f"（当前占用 {len(_used_ports)}/{DS_PORT_MAX - DS_PORT_MIN + 1}）")
            await send_json(ws, {
                "type": "ds_ready_ack",
                "match_id": match_id,
                "adopted": True,
                "message": "已登记端口占用"
            })
        else:
            print(f"[{now()}] ⚠️ 收到未知 match_id 的就绪消息: {match_id} port={port}")
            await send_json(ws, {"type": "error", "message": "未知 match_id"})
        return

    info = pending_matches[match_id]
    info["ds_ws"] = ws
    info["ready"] = True
    ds_connections[match_id] = ws

    # 以 py 分配的端口为准，DS 上报值不一致时提示（正常不会出现，出现了说明启动参数没生效）
    if port and port != info["port"]:
        print(f"[{now()}] ⚠️ DS 上报端口({port}) 与分配端口({info['port']}) 不一致，以分配值为准")

    port = info["port"]
    print(f"[{now()}] 🟢 DS 就绪 match_id={match_id} port={port}")

    player_sockets = info.get("player_sockets", [])
    for player_ws in player_sockets:
        try:
            await send_json(player_ws, {
                "type": "match_found",
                "ds_address": DS_HOST,
                "ds_port": port,
                "match_id": match_id,
                "map": "/Game/UI/main/Maps/GameReady",
                "message": f"匹配成功，连接 DS: {DS_HOST}:{port}"
            })
            print(f"[{now()}] ✅ 已通知玩家连接 DS match_id={match_id}")
        except Exception as e:
            print(f"[{now()}] ⚠️ 通知玩家失败: {e}")

    await send_json(ws, {
        "type": "ds_ready_ack",
        "match_id": match_id,
        "message": "DS 已注册，等待玩家连接"
    })


async def handle_game_end(ws, data):
    """处理 DS 发来的游戏结束消息"""
    match_id = data.get("match_id", "")
    if not match_id:
        await send_json(ws, {"type": "error", "message": "缺少 match_id"})
        return

    print(f"[{now()}] 🏁 游戏结束 match_id={match_id}")

    # 孤儿 DS（py 重启前的比赛）没有 proc 句柄，直接回收它上报的端口
    orphan = orphan_ds.pop(match_id, None)
    if orphan:
        orphan_port = orphan.get("port", 0)
        print(f"[{now()}] 🧹 孤儿 DS 结束 match_id={match_id} port={orphan_port}，回收端口")
        if orphan_port:
            release_ds_port(orphan_port)

    stop_ds_by_match(match_id)

    await send_json(ws, {
        "type": "game_end_ack",
        "match_id": match_id,
        "message": "游戏结束，DS 已关闭"
    })


async def save_player_login(steam_id: str, ticket: str, ip: str = "", login_time: datetime | None = None, user_name: str = ""):
    """每个 steam_id 只保留一行：首次插入，之后每次登录都更新为最新票据。
    依赖 player_logins.steam_id 上的 UNIQUE 索引。
    ticket 仅作为登录记录入库，不参与任何业务消息的身份校验；
    身份校验一律使用 login_ok 下发的会话 token。
    """
    global db_pool
    if db_pool is None:
        print(f"[{now()}] ⚠️ 数据库未连接，跳过保存登录记录")
        return

    # 登录时间由服务端统一生成，保证与日志时间戳一致、不依赖数据库时区/默认值
    login_at = login_time or datetime.now()

    try:
        async with db_pool.acquire() as conn:
            async with conn.cursor() as cur:
                # MySQL 8.0.19+ 行别名写法；ON DUPLICATE KEY UPDATE 由 UNIQUE(steam_id) 触发
                sql = """
                    INSERT INTO player_logins (steam_id, user_name, ticket, ip_address, login_time)
                    VALUES (%s, %s, %s, %s, %s) AS new
                    ON DUPLICATE KEY UPDATE
                        ticket      = new.ticket,
                        ip_address  = new.ip_address,
                        login_time  = new.login_time
                """
                await cur.execute(sql, (steam_id, user_name, ticket, ip, login_at))
                await conn.commit()
                # rowcount: 1=新增, 2=更新（MySQL 默认 CLIENT_FOUND_ROWS 关闭时）
                action = "新增" if cur.rowcount == 1 else "更新"
                print(f"[{now()}] 💾 {action}登录记录: steam_id={steam_id} ip={ip} 登录时间={login_at.strftime('%Y-%m-%d %H:%M:%S')}")
    except Exception as e:
        print(f"[{now()}] ❌ 保存登录记录失败: {e}（确认 steam_id 已建 UNIQUE 索引）")


# ============================================================
# 物品存取（player_item 表）
# ============================================================
# 表结构（与数据库实际结构保持一致）:
#   uid         BIGINT 自增主键   物品唯一id（数据库自动分配，全局唯一）
#   steam_id    VARCHAR(32)      玩家
#   item_id     INT              物品id（关联 UE DataTable 物品模板，库里不存名称/属性）
#   quantity    INT              物品数量
#   create_time DATETIME         入库时间（DEFAULT CURRENT_TIMESTAMP，数据库自动填）

# UninID 字段已删除：uid（player_item 自增主键）本身就是全局唯一 id，直接用它即可


async def add_item_db(steam_id: str, item_id: int, quantity: int = 1,
                      is_tradable: bool = True, use_count: int = 1,
                      userid: int = 0, golden: int = 0):
    """插入一条物品实例，返回数据库生成的物品唯一id(uid)；失败返回 None
    extra: is_tradable=是否可交易（对应 player_item.is_tradable 列），
           use_count=使用次数（默认 1）, userid=UE侧用户id（skillGift 传入）, golden=金币价值（默认 0）"""
    global db_pool
    if db_pool is None:
        print(f"[{now()}] ⚠️ 数据库未连接，跳过保存物品")
        return None

    try:
        async with db_pool.acquire() as conn:
            async with conn.cursor() as cur:
                await cur.execute(
                    "INSERT INTO player_item (steam_id, item_id, quantity, is_tradable, use_count, userid, golden) "
                    "VALUES (%s, %s, %s, %s, %s, %s, %s)",
                    (steam_id, item_id, quantity, 1 if is_tradable else 0, use_count, userid, golden)
                )
                await conn.commit()
                uid = cur.lastrowid
                print(f"[{now()}] 💾 新增物品: steam_id={steam_id} item_id={item_id} "
                      f"数量={quantity} tradable={is_tradable} uid={uid}")
                return uid
    except Exception as e:
        print(f"[{now()}] ❌ 保存物品失败: {e}")
        return None


async def add_backbag_db(userid, item_id: int, unin_id, count: int = 1,
                         is_tradable: bool = False):
    """向 player_backbag 表插入一条背包物品（action=add_item_backbag 用）。
    表里没有 steam_id 列，归属者用 userid（player_logins.id）标识。
    uuid 列存 UE 侧传来的 UninID（客户端保证全局唯一，表上有 UNIQUE 索引），
    后续 backpackRemove 用 userid + uuid 删除对应记录。
    返回 (id, uuid)；数据库不可用/失败返回 (None, None)"""
    global db_pool
    if db_pool is None:
        print(f"[{now()}] ⚠️ 数据库未连接，跳过保存背包物品")
        return (None, None)

    item_uuid = str(unin_id)
    try:
        async with db_pool.acquire() as conn:
            async with conn.cursor() as cur:
                await cur.execute(
                    "INSERT INTO player_backbag (userid, itemid, uuid, itemCount, quantity, isTradable) "
                    "VALUES (%s, %s, %s, %s, %s, %s)",
                    (str(userid), item_id, item_uuid, count, count, 1 if is_tradable else 0)
                )
                await conn.commit()
                new_id = cur.lastrowid
                print(f"[{now()}] 💾 新增背包物品: userid={userid} "
                      f"item_id={item_id} UninID={item_uuid} itemCount={count} quantity={count} "
                      f"tradable={is_tradable} id={new_id}")
                return (new_id, item_uuid)
    except Exception as e:
        print(f"[{now()}] ❌ 保存背包物品失败: {e}")
        return (None, None)


async def get_items_db(steam_id: str):
    """查询玩家全部物品（按 uid 倒序=最新在前）；数据库不可用返回 None"""
    global db_pool
    if db_pool is None:
        print(f"[{now()}] ⚠️ 数据库未连接，无法查询物品")
        return None

    try:
        async with db_pool.acquire() as conn:
            async with conn.cursor(aiomysql.DictCursor) as cur:
                await cur.execute(
                    "SELECT uid, item_id, quantity, is_tradable, use_count, golden, create_time "
                    "FROM player_item WHERE steam_id = %s ORDER BY uid DESC",
                    (steam_id,)
                )
                rows = await cur.fetchall()
                # datetime 转字符串，方便 json 序列化和客户端阅读
                for r in rows:
                    if isinstance(r.get("create_time"), datetime):
                        r["create_time"] = r["create_time"].strftime("%Y-%m-%d %H:%M:%S")
                    # is_tradable 列在库里可能是 BIT/BOOL，统一转成 true/false
                    if "is_tradable" in r:
                        r["is_tradable"] = bool(r["is_tradable"])
                return rows
    except Exception as e:
        print(f"[{now()}] ❌ 查询物品失败: {e}")
        return None


async def remove_item_and_add_gold(steam_id: str, item_id: int, count: int):
    """删除玩家 count 条 item_id 物品记录，并把被删记录的 golden 列之和累加到 player_logins.gold。
    golden 以数据库 player_item.golden 为准（客户端不传，防伪造）。
    返回 (删除条数, 金币增量)；数据库不可用返回 (-1, -1)；没有匹配记录返回 (0, 0)"""
    global db_pool
    if db_pool is None:
        print(f"[{now()}] ⚠️ 数据库未连接，跳过背包移除")
        return (-1, -1)

    try:
        async with db_pool.acquire() as conn:
            async with conn.cursor() as cur:
                # 1) 找出该玩家该物品最早的 count 条记录，同时取每条的 golden
                await cur.execute(
                    "SELECT uid, golden FROM player_item WHERE steam_id=%s AND item_id=%s ORDER BY uid ASC LIMIT %s",
                    (steam_id, item_id, count)
                )
                rows = await cur.fetchall()
                if not rows:
                    return (0, 0)

                uids = tuple(r[0] for r in rows)
                placeholders = ",".join(["%s"] * len(uids))

                # 2) 删除这些记录
                await cur.execute(f"DELETE FROM player_item WHERE uid IN ({placeholders})", uids)
                deleted = cur.rowcount

                # 3) 被删记录的 golden 之和累加到 player_logins.gold（golden 为 NULL 时按 0）
                gold_added = sum((r[1] or 0) for r in rows[:deleted])
                await cur.execute(
                    "UPDATE player_logins SET gold = COALESCE(gold, 0) + %s WHERE steam_id=%s",
                    (gold_added, steam_id)
                )
                await conn.commit()
                return (deleted, gold_added)
    except Exception as e:
        print(f"[{now()}] ❌ 背包移除/加金币失败: {e}")
        return (-1, -1)


# ============================================================
# 技能学习记录（player_logins.learned_skills，JSON 数组，id 为字符串）
# ============================================================

async def get_learned_skills(steam_id: str):
    """读取玩家已学习技能id列表（字符串数组）；无记录/解析失败返回 []"""
    global db_pool
    if db_pool is None:
        print(f"[{now()}] ⚠️ 数据库未连接，无法读取技能记录")
        return []
    try:
        async with db_pool.acquire() as conn:
            async with conn.cursor() as cur:
                await cur.execute(
                    "SELECT learned_skills FROM player_logins WHERE steam_id = %s",
                    (steam_id,)
                )
                row = await cur.fetchone()
                if not row or not row[0]:
                    return []
                data = json.loads(row[0])
                return data if isinstance(data, list) else []
    except Exception as e:
        print(f"[{now()}] ❌ 读取技能记录失败: {e}")
        return []


async def save_learned_skills(steam_id: str, skill_ids):
    """整体覆写玩家已学习技能列表（列表小，整包读-改-写最简单可靠）"""
    global db_pool
    if db_pool is None:
        print(f"[{now()}] ⚠️ 数据库未连接，无法保存技能记录")
        return False
    try:
        async with db_pool.acquire() as conn:
            async with conn.cursor() as cur:
                await cur.execute(
                    "UPDATE player_logins SET learned_skills = %s WHERE steam_id = %s",
                    (json.dumps(skill_ids, ensure_ascii=False), steam_id)
                )
                await conn.commit()
                print(f"[{now()}] 💾 技能记录: steam_id={steam_id} 共{len(skill_ids)}个")
                return True
    except Exception as e:
        print(f"[{now()}] ❌ 保存技能记录失败: {e}")
        return False


async def handle_learn_skill(ws, data):
    """学习技能：action=learn_skill, token=xxx, steam_id=xxx, skill_id="1001"
    幂等（已学过直接返回全量），回包 {"type":"skill_learned","skills":[...]}"""
    p, _ = get_token_player(ws, data)
    if not p:
        await send_json(ws, {"type": "error", "message": "未登录/无效 token（只接受 login_ok 下发的 token）"})
        return

    # steam_id 作为数据库查询/更新的 key，但必须与 token 反查结果一致，防止伪造写他人记录
    steam_id = data.get("steam_id", "")
    if steam_id != p["steam_id"]:
        print(f"[{now()}] ⚠️ learn_skill steam_id 不一致: token反查={p['steam_id']} 客户端={steam_id}")
        await send_json(ws, {"type": "error", "message": "steam_id 不匹配"})
        return

    skill_id = data.get("skill_id", "")
    if not isinstance(skill_id, str) or not skill_id:
        await send_json(ws, {"type": "error", "message": "缺少/非法 skill_id"})
        return

    skills = await get_learned_skills(steam_id)
    if skill_id not in skills:            # 已学过则跳过写入，直接回全量
        skills.append(skill_id)
        if not await save_learned_skills(steam_id, skills):
            await send_json(ws, {"type": "error", "message": "保存技能记录失败"})
            return
    print(f"[{now()}] 📘 学习技能: steam_id={steam_id} skill_id={skill_id}")
    await send_json(ws, {"type": "skill_learned", "skills": skills})


async def handle_get_skills(ws, data):
    """查询已学技能：action=get_skills, token=xxx, steam_id=xxx → {"type":"skills","skills":[...]}"""
    p, _ = get_token_player(ws, data)
    if not p:
        await send_json(ws, {"type": "error", "message": "未登录/无效 token（只接受 login_ok 下发的 token）"})
        return

    steam_id = data.get("steam_id", "")
    if steam_id != p["steam_id"]:
        print(f"[{now()}] ⚠️ get_skills steam_id 不一致: token反查={p['steam_id']} 客户端={steam_id}")
        await send_json(ws, {"type": "error", "message": "steam_id 不匹配"})
        return

    skills = await get_learned_skills(steam_id)
    print(f"[{now()}] 📖 查询技能: steam_id={steam_id} 共{len(skills)}个")
    await send_json(ws, {"type": "skills", "skills": skills})


async def handle_get_learnedSkillsResult(ws, data):
    """技能卡包：action=skillGift, token=xxx（C++ GetSkillCardBag 发出）
    普通池随机 8 个 + 稀有池随机 2 个 + 固定 6/7/8，逐条写入 player_item
    （item_id=技能id, quantity=1, use_count=1, is_tradable=false, golden=0），
    回包 {"type":"learnedSkillsResult","skills":[13个id]}"""
    p, _ = get_token_player(ws, data)
    if not p:
        await send_json(ws, {"type": "error", "message": "未登录/无效 token（只接受 login_ok 下发的 token）"})
        return

    # steam_id 必须与 token 反查结果一致，防止伪造写他人记录
    steam_id = data.get("steam_id", "")
    if steam_id and steam_id != p["steam_id"]:
        print(f"[{now()}] ⚠️ learnedSkills steam_id 不一致: token反查={p['steam_id']} 客户端={steam_id}")
        await send_json(ws, {"type": "error", "message": "steam_id 不匹配"})
        return
    steam_id = p["steam_id"]

    # 普通池随机 8 个不重复 + 稀有池随机 2 个不重复
    drawn = random.sample(SKILL_POOL_NORMAL, 8) + random.sample(SKILL_POOL_RARE, 2)
    # 固定追加 6/7/8：与抽到的技能一样入库（不可交易, golden=0），数量 6→1、7→2、8→2
    drawn += ["6", "7", "8"]
    fixed_quantity = {"6": 1, "7": 2, "8": 2}

    # C++ 消息里的 userId（SetNumberField），写入 player_item.userid
    user_id = data.get("userId", 0)
    try:
        user_id = int(user_id)
    except (TypeError, ValueError):
        await send_json(ws, {"type": "error", "message": "缺少/非法 userId"})
        return

    # 每个抽到的技能写一条 player_item：golden=0, use_count=1, is_tradable=false,
    # 数量：6→1、7→2、8→2，其余=1；userid=消息里的 userId；uid 由数据库自增产生
    failed = 0
    for skill_id in drawn:
        uid = await add_item_db(steam_id, int(skill_id), quantity=fixed_quantity.get(skill_id, 1),
                                is_tradable=False, use_count=1,
                                userid=user_id, golden=0)
        if uid is None:
            failed += 1
    if failed:
        await send_json(ws, {"type": "error", "message": f"技能卡包入库失败（{failed} 条）"})
        return

    print(f"[{now()}] 🎴 技能卡包: steam_id={steam_id} 抽到={drawn}")
    await send_json(ws, {"type": "learnedSkillsResult", "skills": drawn})


async def verify_steam_ticket(ticket: str):
    """
    调用 Steam Web API 校验会话票据（ISteamUserAuth/AuthenticateUserTicket）。
    校验成功返回 SteamID 字符串；失败返回 None。
    """
    if not STEAM_WEB_API_KEY or STEAM_WEB_API_KEY.startswith("在此填入"):
        print(f"[{now()}] ❌ 未配置 STEAM_WEB_API_KEY，无法验票")
        return None
    if not STEAM_APP_ID:
        print(f"[{now()}] ❌ 未配置 STEAM_APP_ID，无法验票")
        return None

    url = "https://api.steampowered.com/ISteamUserAuth/AuthenticateUserTicket/v1/"
    full = url + "?" + urlencode({
        "key": STEAM_WEB_API_KEY,
        "appid": STEAM_APP_ID,
        "ticket": ticket,
    })

    def _request():
        with urlopen(full, timeout=10) as resp:
            return json.loads(resp.read().decode("utf-8"))

    try:
        # urlopen 是阻塞 IO，丢到线程池避免卡住事件循环
        data = await asyncio.to_thread(_request)
    except Exception as e:
        print(f"[{now()}] ❌ Steam 验票请求失败: {e}")
        return None

    params = data.get("response", {}).get("params", {})
    result = params.get("result")
    if result == "OK":
        return params.get("steamid")

    print(f"[{now()}] ❌ Steam 验票未通过: result={result} raw={data}")
    return None


async def handle_login(ws, data):
    ticket = data.get("ticket", "")
    client_steam_id = data.get("steam_id", "")
    if not ticket or not client_steam_id:
        await send_json(ws, {"type": "error", "message": "缺少 ticket/steam_id"})
        return

    print(f"[{now()}] 🎫 收到票据: steam_id={client_steam_id} 长度={len(ticket)} 前缀={ticket[:8]}")
    if ticket == "DummyAuthTicket":
        print(f"[{now()}] ⚠️ 收到假票据 DummyAuthTicket，说明客户端 SteamAPI_Init 没成功")

    verified_steam_id = client_steam_id
    if VERIFY_STEAM_TICKET:
        verified_steam_id = await verify_steam_ticket(ticket)
        if not verified_steam_id:
            await send_json(ws, {"type": "error", "message": "Steam 票据校验失败"})
            return
        # 以后端校验结果为准；客户端传来的只用于比对，防止冒用他人 steam_id
        if verified_steam_id != client_steam_id:
            print(f"[{now()}] ⚠️ steam_id 不一致: 客户端={client_steam_id} 校验={verified_steam_id}")
            await send_json(ws, {"type": "error", "message": "steam_id 不匹配"})
            return

    # 先查库取已注册的 userName 和玩家id（必须在 save_player_login 之前查；新玩家/无记录 → 空字符串/id=0）
    db_user_name = ""
    db_userid = 0
    if db_pool is not None:
        try:
            async with db_pool.acquire() as conn:
                async with conn.cursor() as cur:
                    await cur.execute(
                        "SELECT id, user_name FROM player_logins WHERE steam_id = %s",
                        (verified_steam_id,)
                    )
                    row = await cur.fetchone()
                    if row:
                        db_userid = int(row[0] or 0)
                        if row[1]:
                            db_user_name = row[1]
        except Exception as e:
            print(f"[{now()}] ❌ 查询 user_name/id 失败: {e}")

    # 新玩家（库里无记录）同步建档，立刻拿到自增 id，login_ok 才能带回真实 userid
    if db_userid == 0 and db_pool is not None:
        try:
            async with db_pool.acquire() as conn:
                async with conn.cursor() as cur:
                    await cur.execute(
                        "INSERT INTO player_logins (steam_id, user_name, ticket, ip_address, login_time) "
                        "VALUES (%s, %s, %s, %s, %s)",
                        (verified_steam_id, "", ticket, get_client_ip(ws), datetime.now())
                    )
                    await conn.commit()
                    db_userid = cur.lastrowid
                    print(f"[{now()}] 🆕 新玩家建档: steam_id={verified_steam_id} id={db_userid}")
        except Exception as e:
            print(f"[{now()}] ❌ 新玩家建档失败: {e}")

    token = generate_token()
    players[token] = {
        "steam_id": verified_steam_id,
        "userid": db_userid,       # player_logins.id，写 player_backbag.userid 时使用
        "user_name": db_user_name,
        "ws": ws,
        "logged_in": True,
        "in_queue": False
    }

    print(f"[{now()}] 🔑 登录成功(验票={VERIFY_STEAM_TICKET}): steam_id={verified_steam_id} token={token}")

    # 异步保存到 MySQL（不阻塞响应）：steam_id / ticket / ip / 登录时间
    client_ip = get_client_ip(ws)
    # 新玩家首次插入 user_name 默认空字符串；老玩家不覆盖（save_player_login 的 ON DUPLICATE 已去掉 user_name）
    asyncio.create_task(save_player_login(verified_steam_id, ticket, client_ip, datetime.now(), ""))

    # 登录回包顺带下发已学习技能id列表（字符串数组，如 ["10001","10002"]），省一次 get_skills 请求
    learned_skills = await get_learned_skills(verified_steam_id)

    await send_json(ws, {
        "type": "login_ok",
        "token": token,
        "ticket": ticket,          # 兼容旧客户端缓存；py 端身份校验不使用该字段
        "userName": db_user_name,  # 已注册玩家名；新玩家为空字符串（C++ CachedUserName）
        "userid": db_userid,       # player_logins.id（C++ UserID，skillGift 等消息要用）
        "learn_skill": learned_skills,
        "message": "登录成功"
    })


async def handle_register_user_name(ws, data):
    """注册/更新玩家名：action=registerUName, token=xxx, userName=xxx（C++ SendRegisterUserName 发出）
    按 token 反查 steam_id，更新 player_logins.user_name，并同步内存会话"""
    p, _ = get_token_player(ws, data)
    if not p:
        await send_json(ws, {"type": "error", "message": "未登录/无效 token（只接受 login_ok 下发的 token）"})
        return

    user_name = (data.get("userName") or "").strip()
    if len(user_name) < 6:
        await send_json(ws, {"type": "error", "message": "userName 长度不能少于 6 个字符"})
        return
    if len(user_name) > 64:
        user_name = user_name[:64]      # 与 user_name 列 VARCHAR(64) 对齐

    steam_id = p["steam_id"]
    ok = False
    if db_pool is None:
        print(f"[{now()}] ⚠️ 数据库未连接，无法更新 user_name")
    else:
        try:
            async with db_pool.acquire() as conn:
                async with conn.cursor() as cur:
                    await cur.execute(
                        "UPDATE player_logins SET user_name = %s WHERE steam_id = %s",
                        (user_name, steam_id)
                    )
                    await conn.commit()
                    ok = cur.rowcount > 0
        except Exception as e:
            print(f"[{now()}] ❌ 更新 user_name 失败: {e}")

    if ok:
        p["user_name"] = user_name      # 同步内存会话
        print(f"[{now()}] 📝 更新玩家名: steam_id={steam_id} user_name={user_name}")
        await send_json(ws, {"type": "registerUNameResult", "ok": True, "user_name": user_name})
    else:
        await send_json(ws, {"type": "error", "message": "更新玩家名失败"})


async def handle_get_items(ws, data):
    """查询玩家背包：action=getItems, token=xxx"""
    p, _ = get_token_player(ws, data)
    if not p:
        await send_json(ws, {"type": "error", "message": "未登录/无效 token（只接受 login_ok 下发的 token）"})
        return

    items = await get_items_db(p["steam_id"])
    if items is None:
        await send_json(ws, {"type": "error", "message": "查询物品失败（数据库不可用）"})
        return

    print(f"[{now()}] 🎒 查询背包: steam_id={p['steam_id']} 共{len(items)}件物品")
    await send_json(ws, {"type": "items", "items": items})


async def handle_add_item(ws, data):
    """新增背包物品：action=add_item_backbag, token=xxx, item_id=xxx, unin_id=<UE侧UninID>,
    quantity=1, is_tradable=false（均可省略）
    写入 player_backbag 表（归属者按 token 反查的 userid）；uuid 列存 UninID 并回传"""
    p, _ = get_token_player(ws, data)
    if not p:
        await send_json(ws, {"type": "error", "message": "未登录/无效 token（只接受 login_ok 下发的 token）"})
        return

    item_id = data.get("item_id")
    if not isinstance(item_id, int) or item_id <= 0:
        await send_json(ws, {"type": "error", "message": "缺少/非法 item_id"})
        return

    # UE 侧唯一物品 id（UninID），player_backbag.uuid 列就存它；缺少则拒绝（UNIQUE 索引靠它保证）
    unin_id = data.get("unin_id", 0)
    if not isinstance(unin_id, int) or unin_id <= 0:
        await send_json(ws, {"type": "error", "message": "add_item_backbag: 缺少/非法 unin_id"})
        return

    quantity = data.get("quantity", 1)
    if not isinstance(quantity, int) or quantity <= 0:
        quantity = 1

    is_tradable = bool(data.get("is_tradable", False))

    new_id, item_uuid = await add_backbag_db(p.get("userid", 0), item_id,
                                             unin_id, quantity, is_tradable)
    if new_id is None:
        await send_json(ws, {"type": "error", "message": "保存背包物品失败（数据库不可用）"})
        return

    await send_json(ws, {
        "type": "item_added",
        "id": new_id,              # player_backbag.id（自增主键）
        "unin_id": unin_id,        # 原样回传，客户端缓存供 backpackRemove 使用
        "item_uuid": item_uuid,    # = str(unin_id)，与 backpackRemove 的 item_uuid 对应
        "item_id": item_id,
        "quantity": quantity,
        "is_tradable": is_tradable
    })


async def handle_backpack_remove(ws, data):
    """背包移除物品：action=backpackRemove, token=<py 会话 token>, item_id=xxx, item_uuid=xxx, count=xxx
    （C++ SendBackpackRemove 发出）身份一律按 token 反查，用 userid + uuid 定位删除记录"""
    p, _ = get_token_player(ws, data)
    if not p:
        await send_json(ws, {"type": "error", "message": "backpackRemove: 无效 token（只接受 login_ok 下发的 token）"})
        return
    userid = str(p.get("userid", 0))

    item_id = data.get("item_id")
    item_uuid = data.get("item_uuid")
    count = data.get("count", 1)
    if not isinstance(item_id, int) or item_id <= 0:
        await send_json(ws, {"type": "error", "message": "backpackRemove: 缺少/非法 item_id"})
        return
    if item_uuid is None or (isinstance(item_uuid, str) and not item_uuid.strip()):
        await send_json(ws, {"type": "error", "message": "backpackRemove: 缺少 item_uuid"})
        return
    if not isinstance(count, int) or count <= 0:
        count = 1

    uuid_str = str(item_uuid).strip()

    if db_pool is None:
        await send_json(ws, {"type": "error", "message": "backpackRemove: 数据库未连接"})
        return

    deleted = 0
    try:
        async with db_pool.acquire() as conn:
            async with conn.cursor() as cur:
                # token 反查 userid，userid + uuid 双条件防冒删他人记录
                await cur.execute(
                    "DELETE FROM player_backbag WHERE userid = %s AND uuid = %s",
                    (userid, uuid_str)
                )
                await conn.commit()
                deleted = cur.rowcount
    except Exception as e:
        print(f"[{now()}] ❌ 删除 player_backbag 记录失败: {e}")
        await send_json(ws, {"type": "error", "message": "backpackRemove: 数据库操作失败"})
        return

    print(f"[{now()}] 🗑️ 背包移除: userid={userid} item_id={item_id} uuid={uuid_str} "
          f"请求={count} 删除={deleted}")
    await send_json(ws, {
        "type": "backpack_remove_ok",
        "item_id": item_id,
        "item_uuid": uuid_str,
        "deleted": deleted
    })


async def handle_join(ws, data):
    """C++ SendJoinMatch(bOk) 发来: action=join, token=<py 会话 token>, isMatch=bool
    isMatch=true  -> 记录请求并把玩家加入匹配池，立即尝试一次匹配；
                     凑齐人数马上广播 joinResult result=0，人数不足则什么都不做
    isMatch=false -> 把玩家从匹配池移除即可，不回消息"""
    p, token = get_token_player(ws, data)
    if not p:
        await send_json(ws, {"type": "error", "message": "join: 无效 token（只接受 login_ok 下发的 token）"})
        return

    is_match = bool(data.get("isMatch", False))
    # 记录客户端请求
    print(f"[{now()}] 🎮 join请求: steam_id={p['steam_id']} isMatch={is_match} 当前队列={len(match_queue)}/{MATCH_NEED}")

    if not is_match:
        # isMatch=false：从匹配池移除即可
        if token in match_queue:
            match_queue.remove(token)
        p["in_queue"] = False
        print(f"[{now()}] 🚪 退出匹配: steam_id={p['steam_id']} 剩余队列={len(match_queue)}/{MATCH_NEED}")
        return

    # isMatch=true：加入匹配池
    p["in_queue"] = True
    if token not in match_queue:
        match_queue.append(token)
    print(f"[{now()}] 🎮 加入匹配: steam_id={p['steam_id']} 队列位置={len(match_queue)}/{MATCH_NEED}")

    # 立即尝试一次匹配：凑齐就广播 joinResult result=0，凑不齐 py 什么都不做
    await try_start_match()


async def wait_ds_ready_timeout(match_id: str):
    """等待 DS 就绪，超时则取消匹配"""
    await asyncio.sleep(DS_START_TIMEOUT)

    info = pending_matches.get(match_id)
    if info and not info["ready"]:
        print(f"[{now()}] ⏰ DS 就绪超时 match_id={match_id}，取消匹配")
        for ws in info.get("player_sockets", []):
            try:
                await send_json(ws, {
                    "type": "match_cancelled",
                    "match_id": match_id,
                    "message": "DS 启动超时，请重新匹配"
                })
            except:
                pass
        stop_ds_by_match(match_id)


async def handler(ws):
    peer = getattr(ws, "remote_address", ("?", 0))
    print(f"[{now()}] ✅ 新连接: {peer}")

    try:
        async for raw in ws:
            print(f"[{now()}] 📩 收到: {raw}")
            try:
                data = json.loads(raw)
            except Exception:
                await send_json(ws, {"type": "error", "message": "JSON格式错误"})
                continue

            action = data.get("action")
            if action == "login":
                await handle_login(ws, data)
            elif action == "registerUName":
                await handle_register_user_name(ws, data)
            elif action == "join":
                await handle_join(ws, data)
            elif action == "joinGame":
                await handle_join_game(ws, data)
            elif action == "ds_ready":
                await handle_ds_ready(ws, data)
            elif action == "game_end":
                await handle_game_end(ws, data)
            elif action == "getItems":
                await handle_get_items(ws, data)
            elif action == "add_item_backbag":
                await handle_add_item(ws, data)
            elif action == "learn_skill":
                await handle_learn_skill(ws, data)
            elif action == "get_skills":
                await handle_get_skills(ws, data)
            elif action == "skillGift":
                await handle_get_learnedSkillsResult(ws, data)
            elif action == "backpackRemove":
                await handle_backpack_remove(ws, data)
            elif action == "ping":
                await send_json(ws, {"type": "pong"})
            else:
                await send_json(ws, {"type": "error", "message": f"未知action: {action}"})

    except websockets.exceptions.ConnectionClosed as e:
        # websockets 12+ 已移除 e.clean 属性，只打印 code/reason
        print(f"[{now()}] ⚠️ [WS连接关闭] peer={peer} code={e.code} reason={e.reason}")
    except Exception as e:
        print(f"[{now()}] ⚠️ [连接异常] peer={peer} error={e}")
    finally:
        # 先判断是否是 DS 连接断开
        ds_found = False
        for match_id, ds_ws in list(ds_connections.items()):
            if ds_ws is ws:
                print(f"[{now()}] 🔴 [DS断开] match_id={match_id} peer={peer}")
                ds_connections.pop(match_id, None)
                
                info = pending_matches.get(match_id)
                if info and info["ready"]:
                    print(f"[{now()}] ⏸️ [DS断开] 已就绪，标记等待 game_end 或超时 match_id={match_id}")
                    info["ds_disconnected"] = True
                elif info and not info["ready"]:
                    print(f"[{now()}] ⏳ [DS断开] 未就绪，等待超时处理 match_id={match_id}")
                ds_found = True
                break

        # 孤儿 DS 断开：保留端口占用记录（DS 可能只是重连），只清掉失效的连接引用
        if not ds_found:
            for match_id, orphan in list(orphan_ds.items()):
                if orphan.get("ws") is ws:
                    orphan["ws"] = None
                    print(f"[{now()}] 🔴 [孤儿DS断开] match_id={match_id} port={orphan.get('port')}，"
                          f"端口保留占用等待重连上报")
                    ds_found = True
                    break

        # 再判断是否是玩家连接断开
        player_found = False
        for token, p in list(players.items()):
            if p.get("ws") is ws:
                print(f"[{now()}] 🔵 [玩家断开] steam_id={p.get('steam_id')} token={token} peer={peer}")
                if token in match_queue:
                    match_queue.remove(token)
                players.pop(token, None)
                player_found = True
                break

        # 如果既不是 DS 也不是玩家，打印未知断开
        if not ds_found and not player_found:
            print(f"[{now()}] ⚪ [未知连接断开] peer={peer}")

        print(f"[{now()}] 📊 状态: 队列={len(match_queue)}/{MATCH_NEED} 玩家在线={len(players)} DS活跃={len(ds_connections)}")


async def init_db_pool():
    """创建数据库连接池"""
    global db_pool
    try:
        db_pool = await aiomysql.create_pool(**DB_CONFIG)
        print(f"[{now()}] ✅ MySQL 连接池已建立 (127.0.0.1:3306/black56)")
        return True
    except Exception as e:
        print(f"[{now()}] ❌ MySQL 连接失败: {e}")
        print(f"[{now()}] ⚠️ 匹配服将继续运行，但不会保存登录记录")
        return False


async def main():
    # 初始化数据库连接池
    db_ok = await init_db_pool()
    
    print(f"\n{'='*60}")
    print(f"  Black56 Lobby/匹配服务器")
    print(f"  WS监听: ws://{HOST}:{PORT}")
    print(f"  下发给DS的匹配服地址: {MATCH_SERVER_URL}")
    print(f"  DS目标: {DS_HOST}")
    print(f"  DS目录: {DS_DIR}")
    print(f"  匹配人数: {MATCH_NEED}")
    print(f"  DS就绪超时: {DS_START_TIMEOUT}s")
    print(f"  端口范围: {DS_PORT_MIN}-{DS_PORT_MAX} ({DS_PORT_MAX - DS_PORT_MIN + 1}个)")
    print(f"  匹配方式: 事件驱动（玩家加入时触发）")
    print(f"  数据库: MySQL (127.0.0.1:3306/black56)")
    print(f"  Steam验票: {'开启' if VERIFY_STEAM_TICKET else '关闭'}  AppId={STEAM_APP_ID}")
    
    if db_ok:
        print(f"  ✅ 数据库连接成功！登录记录->black56.player_logins 物品数据->black56.player_item")
    else:
        print(f"  ⚠️ 数据库连接失败，匹配服将以无数据库模式运行")
    
    print(f"  启动时间: {now()}")
    print(f"{'='*60}\n")

    async with websockets.serve(handler, HOST, PORT):
        await asyncio.Future()


if __name__ == "__main__":
    asyncio.run(main())