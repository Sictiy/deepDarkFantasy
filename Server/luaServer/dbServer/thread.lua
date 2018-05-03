package.path = package.path .. ';../protobuf/?.lua;./?.lua;../?.lua'
package.cpath = package.cpath .. ';../protobuf/?.so'
local table = require "luaBase.table_ex"

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
	Log("[LUA PRINT]" .. str);
end

function connectMysql(host,prot,user,passwd,db)
	mysqlConnect(host,prot,user,passwd)
	mysqlQuery(("create database if not exists %s;"):format(db))
	mysqlSelectDb(db)
	print("connectMysql success",db)
end

function loadModule(modName)
	local mod = require(modName)
	if mod.init then
		mod.init()
	end
	print("loadModule success",modName)
end

function callFunc(modName, funcName, ...)
	print("start call function : ", modName, funcName)
	local mod = require(modName)
	local func = mod[funcName]
	return func(...)
end