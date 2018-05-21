module(..., package.seeall)

local command = require "luaBase.command"

Halls = {}
local index = 0
local min = 0
local minCount = 0

function processData(connect, cmd, data)
	if cmd == h2l_regist then
		print("regist game server")
		local connectNum, ip, port = UnSerialize(data)
		if not connect.index then
			connect.index = index
			Halls[index] = {
				connect = connect,
				connectNum = connectNum,
				ip = ip,
				port = port
			}
			index = index + 1
		else
			Halls[index].connectNum = connectNum
		end
		if connectNum <= minCount or minCount = 0 then
			minCount = connectNum
			min = index
		end
	end
end

function getHall()
	local hall = Halls[min]
	return hall.ip, hall.port
end