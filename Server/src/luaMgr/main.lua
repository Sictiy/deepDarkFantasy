module(..., package.seeall)

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


function Init()
    print("init ")
end

function Start()
    print("start ")
end

print("loadScript main.lua success!")
local server = require "server"
server.startServer()