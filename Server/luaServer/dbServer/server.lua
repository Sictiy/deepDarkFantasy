module(..., package.seeall)

local command = require "luaBase.command"
local Event = require "luaBase.eventdispatcher"


local server = nil
local connections = {}
local connect_count = 0
local threads = {}
local requestQueue = {}
local requestId = 0
local nameMap = {}
local frist = false

function startServer()
	SetStateFunc{load = load, update = update}
	init()
	start()
end

function init()
	createServer(Config.ServerHost,Config.ServerPort);
	local thread = createThread("role")
	thread.connectMysql(Config.DbHost, Config.DbPort, Config.DbUser, Config.DbPassword, Config.DbName)
	thread.loadModule("role","role.role")
	print("init")
end

function start()
	print("start")
end

function load()
	print("load success")
	return true
end

function update(frame)
	if not frist then
		frist = true
	end
	return true
end

function shutdown()
	print("shutdown")
	return true
end

function createThread(name)
	local thread = CreateThread("thread.lua")
	thread.name = name
	thread.onError = onError
	thread.onResult = onResult
	thread.connectMysql = function( ... )
		thread.luaPushRequest(0,Serialize("connectMysql",...))
	end
	thread.loadModule = function(name, modName)
		thread.luaPushRequest(0,Serialize("loadMoule",modName))
		nameMap[name] = {thread.name,modName}
	end
	threads[name] = thread
	return thread
end

function onResult(id, result)
	print("get result",id)
	local request = requestQueue[id]
	if request then
		requestQueue[id] = nil
		if result then
			request.packet.luaSendData(command.d2h_request,Serialize(request.name, request.method, UnSerialize(result)))
		end
	end
end

function onError(id)
	requestQueue[id] = nil
end

function createServer(host,port)
	server = CreateServer(host,port)
	if server then
		server.newConnect = newConnect
	else
		print("createServer failed!")
		os.exit(1)
	end
end

function newConnect(packet)
	print("newConnect",packet)
	connect_count = 1+ connect_count
	local connect = {
		id = connect_count,
		packet = packet
	}
	connections[connect_count] = connect
	packet.connect = connect

	packet.recvData = recvData
	packet.disConnect = disConnect
end

function disConnect(packet)
	print("disConnect",packet)
end

function recvData(packet)
	if packet.luaGetCmd() == command.h2d_request then
		processData(packet,UnSerialize(packet.luaRecvData()))
	end
	print("recvData",packet.luaGetCmd(),packet.luaRecvData(),packet)
end

function processData(packet, name, method, ...)
	print("get request: ", name, method)
	requestId = requestId + 1
	requestQueue[requestId] = {
		packet = packet,
		name = name,
		method = method
	}
	pushRequest(requestId, name, method, ...)
end

function pushRequest(id, name, ...)
	local map =nameMap[name]
	if not map then
		print("not map")
		return
	end
	local thread = threads[map[1]]
	if not thread then
		print("not thread",map[1])
		return
	end
	thread.luaPushRequest(id,Serialize("callFunc", map[2], ...))
end