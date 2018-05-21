module(..., package.seeall)

local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local Config = require "config"

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
	-- body
end

function disConnect(packet)
	
end

function processData(connect, cmd, data)
	-- body
end