module(..., package.seeall)

local room = require "match.room"
local roleMgr = require "role.roleMgr"
local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local gameHandler = require "gamehandler"

require "Battle_pb"

function distributeBattle(connect, data)
	local response = Battle_pb.BattleInfo()
	response.ip = gameHandler.getIp()
	response.port = gameHandler.getPort()

	local msg = response:SerializeToString()

	local roleData = {}
	for k, v in pairs(data) do 
		local role = roleMgr.getRole(v)
		if not role then
			print("can't find role",v)
		end
		roleData[v] = role:getRoleBase()
	end
	connect.packet.luaSendData(command.h2g_role_data,Serialize(roleData))
	for k, v in pairs(data) do 
		local role = roleMgr.getRole(v)
		if not role then
			print("can't find role",v)
		end
		role:sendData(command.s2c_battle_search, msg)
	end
end

------------------------------------------------------------init
function init()
	Event.addEvent("recvData", _M, "onRecvData")
end

function onRecvData(connect, cmd, data)
	if cmd == command.c2s_battle_search then
		room.addRole(connect.roleId)
	end
end