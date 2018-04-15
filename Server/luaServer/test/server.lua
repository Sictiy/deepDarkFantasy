module(..., package.seeall)

function startServer()
	init()
	start()
end

function init()
	print("init")
	CreateServer("127.0.0.1",5050);
	ConnectServer("127.0.0.1",5151);
	-- body
end

function start( ... )
	print("start")
	-- body
end

function disConnect(fd)
	print(fd)
	-- body
end

function recvData(fd,cmd,data)
	print("recvData",fd,cmd,data)
	-- body
end