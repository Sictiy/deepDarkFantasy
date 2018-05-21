module(..., package.seeall)

local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local Config = require "config"

local battleMgr = require "battle.battlemgr"

local hallServer = nil

function init()

	print("init")
end

function connectToHall(host, port)
	print("connect to hall", host, port)
	hallServer = ConnectServer(host, port)
	if hallServer ~= nil then
		hallServer.recvData = recvData
		hallServer.disConnect = disConnect
		print("connect to hall success")
	else
		print("connect to hall failed")
	end
end

function recvData(packet, data)
	print("recvData-----",packet.luaGetCmd())
	processData(packet.connect, packet.luaGetCmd(), packet.luaRecvData())
end

function disConnect(packet)
	Event.dispatcher("disConnect",packet.connect)
end

function processData(connect, cmd, data)
	if cmd == command.h2g_battle then
		SendData(command.g2h_battle, data)
	elseif cmd == command.h2g_roledata then
		battleMgr.createBattle(data)
	end
	Event.dispatcher("recvData", connect, cmd, data)
end