module(...,package.seeall)

local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local dbHandler = require "dbhandler"
local hallHandler = require "hallhandler"

Roles = {}

function init()
	Event.addEvent("onuserlogin",_M,onUserLogin)
	print("init")
end

function login(id, connect, data)
	createUser(id,connect)
	dbHandler.request("user","login",id,name)
end

function createUser(id, connect, data)
	local user = {
		id = id, 
		connect = connect,
		name = name,
		passwd = passwd
	}
	Roles[id] = user
end

function getUser(id)
	return Roles[id]
end

function onUserLogin(id, result, data)
	if result == command.ok then
		local user = UnSerialize(data)
		local localuser = getUser(id)
		if localuser.name == user.name and localuser.passwd = user.passwd then
			print("login success")
		else
			print("login faild")
		end
	end
	if result == command.account_not_exists then
		-- dbHandler.request("user","login",id,)
		print("login name do not exist")
	end
end

function loginSuccess(id, name)
	ip, host = hallHandler.getHall()
	print("success login , to ", ip, host)
end