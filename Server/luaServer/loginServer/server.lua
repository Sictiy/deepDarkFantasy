module(..., package.seeall)

-- local rank = require "rank"
local dbHandler = require "dbhandler"
local hallHandler = require "hallhandler"
local command = require "luaBase.command"
local Event = require "luaBase.eventdispatcher"
local role = require "role"


local server = nil
local connections = {}
local connect_count = 0
local times = 0

function startServer()
	print("startServer")
	SetStateFunc{load = load, update = update}
	init()
	start()
end

function init()
	createServer(Config.ServerHost,Config.ServerPort);
	dbHandler.init()
	-- hallHandler.init()
	role.init()
	print("init")
end

function start()
	-- rank.init()
	print("start")
end

function load()
	print("load success")
	return true
end

function update(frame)
	times = times + 1
	if times > 600 then
		dbHandler.breathe()
		times = 0
	end
	return true
end

function shutdown()
	print("shutdown")
	return true
end

function createServer(host,port)
	server = CreateServer(host,port)
	if server then
		server.newConnect = newConnect
	else
		print("createServer failed!")
		os.exit(1)
	end
	print(server)
end

function newConnect(packet)
	print("newConnect",packet)
	connect_count = 1+ connect_count
	local connect = {
		id = connect_count,
		packet = packet,
		newFlag = true,
	}
	connections[connect_count] = connect
	packet.connect = connect

	packet.recvData = recvData
	packet.disConnect = disConnect

	--from client
	-- rank.sendRank(connect)
end

function disConnect(packet)
	print("disConnect",packet)
end

function recvData(packet)
	print("recvData--------------",packet.luaGetCmd())
	processData(packet.connect,packet.luaGetCmd(),packet.luaRecvData())
end

function processData(connect, cmd, data)
	if cmd == command.c2s_login and connect.id ~= nil then
		role.login(id, connect, data)
		print "login"
	end
	hallHandler.processData(connect, cmd, data)
	-- Event.dispatcher("recvData", connect, cmd, data)
end