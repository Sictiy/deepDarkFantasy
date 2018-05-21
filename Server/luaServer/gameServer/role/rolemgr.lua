module(...,package.seeall)

local command = require "luaBase.command"
local Event = require "luaBase.eventdispatcher"
require "role.role"

Roles = {}

function login(id, connect)
	Roles[id] = Role:create(id, connect)
	return Roles[id] ~= nil and 1 or 0
end

function sendData(id, cmd, data)
	local role = Roles[id]
	if not role then
		print("role do not exist")
		return
	end
	role:sendData(cmd, data)
end

function broadcast(cmd, data)
	for k, v in pairs(Roles) do
		v:sendData()
	end
end

function getRole(roleId)
	if Roles[roleId] ~= nil then
		return Roles[roleId]
	end
end

function disposeRole(id)
	local role = Roles[id]
	if role ~= nil then
		role:dispose
	end
	Roles[id] = nilS
end

function init()
	Event.addEvent("recvData",_M,"recvData")
end

function recvData(id, cmd, data)
	
end