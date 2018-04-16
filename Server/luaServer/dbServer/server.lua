module(..., package.seeall)

local server = nil
local connections = {}
local connect_count = 0

function startServer()
	init()
	start()
end

function init()
	print("init")
	-- body
end

function start()
	print("start")
	createServer(Config.ServerHost,Config.ServerPort);
	-- ConnectServer(Config.DbHost,Config.DbPort);
	-- body
end

function load()
	-- body
end

function update()
	-- body
end

function shutdown()
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

function recvData(cmd, data, packet)
	print("recvData",cmd,data,packet)
	-- body
end