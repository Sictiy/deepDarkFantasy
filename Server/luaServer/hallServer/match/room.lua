module(..., package.seeall)

local command = require "luaBase.command"
local gameHandler = require "gamehandler"

local index = 1

Rooms = {}

function addRole(roleId)
	local room = findRoom()
	if not room then
		createRoom(roleId)
		return
	end
	table.insert(Rooms[room],roleId)
	if #Rooms[room] >= 20 then
		print("removeroom",room)
		removeRoom(room)
	end
end

function findRoom()
	for k, v in pairs(Rooms) do
		if #v < 20 then
			return k
		end
	end	
	return nil
end

function createRoom(roleId)
	print("create room by",roleId)
	Rooms[index] = {}
	table.insert(Rooms[index],roleId)
	index = index +	1
end

function removeRoom(index)
	local gameServer = gameHandler.getServer()
	if not gameServer then
		print("do not have gameServer")
		return
	end
	gameServer.connect.packet.luaSendData(command.h2g_battle, Serialize(Rooms[index]))
	Rooms[index] = nil
end