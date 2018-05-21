module(...,package.seeall)

local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local Config = require "config"
local loginServer = nil

function init()
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

function registHall(num,ip,port)
	localServer.lusSendData(command.h2l_regist,Serialize(num,ip,port))
end

function recvData(packet)
	print("recvData-----",packet.luaGetCmd())
	processData(packet.connect, packet.luaGetCmd(), packet.luaRecvData())
end

function processData(connect, cmd, data)
end

function disConnect(packet)
	Event.dispatcher("disConnect",packet.connect)
end
