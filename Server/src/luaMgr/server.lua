module(..., package.seeall)

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