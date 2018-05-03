module(...,package.seeall)

local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local dbHandler = require "dbhandler"
local userInfo = require "protos.Protos_pb"

local roledata = {}

local function getRank(connect,data)
	local user = userInfo.UserInfo()
	user:ParseFromString(data)
	local role = {}
	role.name = user.name
	role.score = user.score
	insertRank(role)
	sendRank(connect)
end

function sendRank(connect)
	local users = userInfo.DataResp()
	local count = #table
	if count > 10 then
		count = 10
	end
	local index = 0
	for k,v in ipairs(roledata) do
		index = index +1
		local u = userInfo.UserInfo()
		u.name = v.name
		u.score = v.score
		table.insert(users.users,u)
		if index >= 10 then
			break
		end
	end
	local data = users:SerializeToString()
	sendData(connect,data)
end

local function compFunc(a,b)
	return a.score > b.score
end

local function insertRank(user)
	table.insert(roledata, user)
	table.sort(roledata, compFunc)
	dbHandler.require(role,insert,Serialize(user))
end

local function sendData(connect,data)
	local packet = connect.packet
	if not packet then
		print("packet not exists!")
		return
	end
	packet.luaSendData(command.s2c_rank_get,data)
end

local function loadRank()
	print("start load rank")
	dbHandler.request("role","loadall")
end

function onloadall(cmd, data)
	if not cmd == command.ok then
		print("loadRank failed")
		return 
	end
	roledata = UnSerialize(data)
	table.sort(roledata, compFunc)
	print("loadRank success:",roledata,string.len(data))
end

--init dispatcher-----------------------------------------------
function init()
	Event.addEvent("recvData", _M, "onRecvData")
	Event.addEvent("onroleloadall", _M, "onloadall")
	loadRank()
end

function onRecvData(connect,cmd,data)
	if cmd == command.c2s_rank_get then
		print("get rankdata from client")
		getRank(connect,data)
	end
	-- if cmd == command.c2s_rank_insert then
	-- 	print("get rankdata from client")
	-- 	insertRank(connect,data)
	-- end
end
