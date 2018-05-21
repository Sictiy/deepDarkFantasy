module(...,package.seeall)

local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local dbServer = nil

function init()
	connectToDbServer(Config.DbHost,Config.DbPort);
end

function connectToDbServer(host,port)
	print("start connect to db",host,port)
	dbServer = ConnectServer(host,port)
	if dbServer ~= nil then
		dbServer.recvData = recvData
		dbServer.disConnect = disConnect
		print("connect to db success")
	else
		print("connect to db failed!")
	end
	print("result",dbServer)
end

function recvData(packet)
	if packet.luaGetCmd() == command.d2h_request then
		processRespond(UnSerialize(packet.luaRecvData()))
	end
end

function processRespond(name, method, ...)
	print("get respond:", name, method)
	if not dbServer then
		return 
	end
	Event.disPatcher("on"..name..method,...)
end

function request(name,method,...)
	if not dbServer then
		return 
	end
	dbServer.luaSendData(command.h2d_request,Serialize(name,method,...))
end

function breathe()
	dbServer.luaSendData(7)
end