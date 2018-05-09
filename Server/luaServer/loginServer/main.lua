package.path = package.path .. ';../protobuf/?.lua;../?.lua;./?.lua'
package.cpath = package.cpath .. ';../protobuf/?.so;../bin/?.so'

local table = require "luaBase.table_ex"
local dbg = require "luaBase.debugger"

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

SetErrorFunc(function(err)
		print("Debugger stopped on error:");
		print(debug.traceback(err));
		dbg(false, 1);
	end)

local server = require "server"
server.startServer()