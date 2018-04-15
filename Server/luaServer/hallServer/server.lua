module(..., package.seeall)

local server = nil

function startServer()
	init()
	start()
	-- body
end

function init()
	print("init")
	-- body
end

function start( ... )
	print("start")
	-- body
end