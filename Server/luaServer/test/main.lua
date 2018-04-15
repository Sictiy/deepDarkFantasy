oldprint = print
function print(...)
	local str = "";
	for i,v in ipairs{...} do
		if i > 1 then
			str = str .. "\t";
		end
		if type(v) == "table" then
			str = str .. table.tostring(v);
		else
			str = str .. tostring(v);
		end
	end
	Log("[LUA PRINT]:" .. str);
end

print("loadScript main.lua success!")
local server = require "server"
server.startServer()

-- function test()
-- 	print("test success!")
-- 	return "test return "
-- 	-- body
-- end

-- -- string = "test string"
-- function recvData(int fd, int cmd, string data)
-- 	print("recvData:",fd,cmd,data)
-- 	-- body
-- end

-- function disConnect(int fd)
-- 	print("disConnect:",fd)
-- 	-- body
-- end