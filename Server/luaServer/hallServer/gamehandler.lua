module(..., package.seeall)

local command = require "luaBase.command"
local matchMgr = require "match.matchMgr"

GameSvr = {}
local index = 0

function processData(connect, cmd, data)
	if cmd = g2h_regist then
		print("regist game server")
		local connectNum, ip, port = UnSerialize(data)
		if not connect.index then
			connect.index = index
			GameSvr[index] = {
				connect = connect,
				connectNum = connectNum,
				ip = ip,
				port = port
			}
			index = index + 1
		else
			GameSvr[index].connectNum = connectNum
		end
	elseif cmd = g2h_battle then
		print("distribute battle")
		matchMgr.distributeBattle(connect, data)
	end
endthen

function disposeServer(index)
	if GameSvr[index].connect ~= nil then
		print("dispose Server",index)
	end
	GameSvr[index] = nil
end