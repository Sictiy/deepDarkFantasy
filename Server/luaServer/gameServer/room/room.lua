module(...,package.seeall)

local command = require "luaBase.command"
local Event = require "luaBase.eventdispatcher"
local roleMgr = require "role.rolemgr"

Rooms = {}
local index = 0 

function init()
	Event.addEvent("recvData",_M,"recvData")
end

function recvData(id, cmd, data)
	if cmd == c2s_room_crt then
		create(id, data)
	elseif cmd == c2s_room_join then
		join(id, data)
	elseif cmd == c2s_room_quit then
		quit(id, data)
	elseif cmd == c2s_room_close then
		close(id, data)
	end
end

function broadcast(index, cmd, data)
	local room = Rooms[index]
	if not room then
		print("room do not exist!")
		return
	end
	for k,v in pairs(room.roles) do
		roleMgr.sendData(v,cmd,data)
	end
end
------------------------------------------------
function createRoom(roles)
	index = index + 1
	if index > 65535 then
		index = 1
	end
	Rooms[index] = {
		lock = false,
		roles = roles
	}
	return index
end

function joinRoom(index, roleId)
	local room = Rooms[index]
	if not room then
		return false
	end
	table.insert(room.roles, roleId)
	return true
end

function quitRoom(index, roleId)
	local room = Rooms[index]
	if not room then
		return false
	end
	for k,v in pairs(room.roles) do
		if v == roleId then
			print("quit success",roleId,index)
			table.remove(room.roles,roleId)
			return true
		end
	end
	return false
end

function closeRoom(index)
	Rooms[index] = nil
end

function create(id, data)
	-- body
end

function join(id, data)
	-- body
end

function quit(id, data)
	-- body
end

function close(id, data)
	-- body
end