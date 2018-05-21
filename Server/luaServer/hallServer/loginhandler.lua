module(...,package.seeall)

local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local Config = require "config"
local loginServer = nil

function init()
	-- Event.addEvent("onrecvdata")
	connectToLoginServer(Config.LoginHost,Config.LoginPort)
	print("init")
end

function connectToLoginServer(host,port)
	print("start connect to login",host,port)
	loginServer = ConnectServer(host,port)
	if loginServer ~= nil then
		loginServer.recvData = recvData
		loginServer.disConnect = disConnect
		print("connect to login success")
	else
		print("connect to login failed!")
	end
	print("result",loginServer)
end

function recvData(packet)
	-- if packet.luaGetCmd() == command.d2h_request then
	-- 	processRespond(UnSerialize(packet.luaRecvData()))
	-- end
	print(packet)
end

function disConnect(packet)
	print(packet)
end
