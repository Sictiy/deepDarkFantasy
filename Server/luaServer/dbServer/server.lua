module(..., package.seeall)

local server = nil
local connections = {}
local connect_count = 0
local frist = false

function startServer()
	SetStateFunc{load = load, update = update}
	init()
	start()
end

function init()
	print("init")
	-- body
end

function start()
	print("start")
	-- body
end

function load()
	createServer(Config.ServerHost,Config.ServerPort);
	print("load success")
	return true
	-- body
end

function update(frame)
	if not frist then
		frist = true
		local msg = Serialize(1,2,3,true,{1,2},{3,4},"fsfs","dddd",11,{key1 = 1, key2 = 2})
		print(msg)
		print(UnSerialiaze(msg))
		print("packer success")
	end
	return true
	-- body
end

function shutdown()
	print("shutdown")
	return true
	-- body
end

function createServer(host,port)
	server = CreateServer(host,port)
	if server then
		server.newConnect = newConnect
	else
		print("createServer failed!")
		os.exit(1)
	end
	-- body
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
	-- body
end

function recvData(packet)
	print("recvData",packet.luaGetCmd(),packet.luaRecvData(),packet)
	-- body
end