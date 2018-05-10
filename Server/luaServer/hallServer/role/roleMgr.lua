module(...,package.seeall)

local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local dbHandler = require "dbhandler"

require "role.role"

Roles = {}

function login(id, connect)
	Roles[id] = Role:create(id, connect)
	dbHandler.request("role","load",id)
	return Roles[id] ~= nil and 1 or 0
end

function getRole(roleId)
	return Roles[roileid]
end

function sendData(roleId, cmd, data)
	local role = getRole(roleId)
	if not role then
		print("can't find role",roleId)
		return
	end
	role:sendData(cmd, data)
end

function broadcast(cmd, data)
	for k, v in ipairs(Roles) do
		v:sendData(cmd, data)
	end
end
----------------------------------------------------dbresponse
function onroleload(roleId, result, data)
	local role = getRole(roleId)
	if result == command.account_not_exists then
		local tempData = {}
		tempData.nickName = role:getName()
		tempData.gender = role:getGender()
		tempData.icon = role:getIcon()
		tempData.gold = role:getGold()
		tempData.level = role:getLevel()

		dbHandler.request("role", "create", roleId, Serialize(tempData))
	end
	if result == command.ok then
		role:load(data)
		print("load role data success")
	end
end

function onrolecreate(roleId, result, data)
	local role = getRole(roleId)
	if result == command.ok then
		role:postData()
	end
end
----------------------------------------------------init
function init()
	Event.addEvent("recvData", _M, "onRecvData")
	Event.addEvent()
end

function onRecvData(connect, cmd, data)
end
