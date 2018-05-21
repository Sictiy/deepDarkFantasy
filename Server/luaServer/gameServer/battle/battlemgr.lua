module(..., package.seeall)

require "battle.battle"

local roomMgr = require "room.room"
local roleMgr = require "role.rolemgr"
local command = require "luaBase.command"
local Event = require "luaBase.eventdispatcher"

Battles = {}
battleId = 1

--------------------------------------------------------------------
function broadcast(id, data)
	local battleId = getBattleIdByRole(id)
	if not battleId then
		print("not found battle")
		return
	end
	local battle = Battles[battleId]
	battle:updateBattle(id, data)
end

function setState(roleid, data)
	local battleId = getBattleIdByRole(roleid)
	if not battleId then
		print("not found battle")
		return
	end
	local battle = Battles[battleId]
	battle:updateState(roleid, data)
end

function getBattleIdByRole(roleid)
	local role = roleMgr.getRole(roleid)
	if not role then
		return nil 
	end
	return role.battleId
end

function init()
	Event.addEvent("recvData", _M, "recvData")
end

function recvData(id, cmd, data)
	if cmd == c2s_battle_broadcast then
		broadcast(roleid, data)
	elseif cmd == c2s_battle_state then
		setState(roleid, data)
	end
end
-----------------------------------
function createBattle(data)
	local roles = {}
	for k,v in pairs(data) do
		table.insert(roles, k)
	end
	local roomId = roomMgr.createRoom(roles)
	local battle = Battle:create(roomId,roles)
	Battle[roomId] = battle
	for k,v in pairs(roles) do
		roleMgr.login(v)
		bindRole(v, battle:getBattleId())
	end
end

function joinBattle(connect, data)
	
end