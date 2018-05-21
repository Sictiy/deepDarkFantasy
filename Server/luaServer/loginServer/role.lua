module(...,package.seeall)

local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local dbHandler = require "dbhandler"
local hallHandler = require "hallhandler"
local LoginPb = request "Login_pb"

Roles = {}

function init()
	Event.addEvent("onuserlogin",_M,"onUserLogin")
	Event.addEvent("onuserregist",_M,"onUserRegist")
	Event.addEvent("disConnect",_M,"disConnect")
	Event.addEvent("recvData",_M "recvData")
	print("init")
end

function recvData(connect, cmd, data)
	if cmd == c2s_login then
		login(connect.id, connect, data)
	elseif cmd == c2s_regist then
		regist(connect.id, connect, data)
	end
end

function disConnect(id)
	Roles[id] = nil
end

function login(id, connect, data)
	local request = LoginPb.AccountInfo()
	request:ParseFromString(data)
	createUser(id, connect, request.name, request.passwd)
	dbHandler.request("user","login",id, request.name)
end

function regist(id, connect, data)
	local request = LoginPb.AccountInfo()
	request:ParseFromString(data)
	createUser(id, connect, request.name, request.passwd)
	dbHandler.request("user","create",id, request.name, request.passwd)
end

function createUser(id, connect, name, passwd)
	local user = Roles[id]
	if not user then
		user = {
			id = id, 
			connect = connect,
			name = name,
			passwd = passwd
		}
	else
		user.name = name
		user.passwd = passwd
	end
	Roles[id] = user
end

function getUser(id)
	return Roles[id]
end

function onUserRegist(id, result, data)
	if result == command.ok then
		loginSuccess(id, user.name)
	else
		loginFailed(id)
	end
end

function onUserLogin(id, result, data)
	if result == command.ok then
		local user = UnSerialize(data)
		local localuser = getUser(id)
		if localuser.name == user.name and localuser.passwd = user.passwd then
			loginSuccess(id, user.id, user.name)
			print("login success")
		else
			loginFailed(id)
			print("login faild")
		end
	end
	if result == command.account_not_exists then
		loginFailed(id)
		print("login name do not exist")
	end
end

function loginFailed(id)
	local response = LoginPb.LoginRes()
	response.result = false
	local data = response:SerializeToString()
	sendData(id, command.s2c_login, data)
end

function loginSuccess(id, roleId, name)
	ip, host = hallHandler.getHall()
	print("success login , to ", ip, host)
	local response = LoginPb.LoginRes()
	response.result = true
	response.ip = ip
	response.host = host
	response.id = roleId

	local data = response:SerializeToString()
	sendData(id, command.s2c_login, data)
end

function sendData(id, cmd, data)
	Roles[id].connect.packet.luaSendData(cmd, data)	
end