Role = {}
Role.__index = Role

local Event = require "luaBase.eventdispatcher"
local command = require "luaBase.command"
local dbHandler = require "dbhandler"

function Role:new()
	local self = {}
	setmetatable(self,Role)

	return self
end

function Role:create(id, connect)
	local self = Role:new()

	self.roleId = id
	connect.roleId = id
	self.connect = connect
	self.roomId = nil
	self.nickName = "user"..tostring(id)

	if not self:init() then
		self:dispose()
		return nil
	end
	return self
end

function Role:init()
	self.gender = math.random(0,1)
	self.icon = math.random(1,20)
	self.level = 0
	self.gold = 0
	return true
end

function Role:dispose()
	self:save()
end
--------------------------------------------------
function Role:getName()
	return self.nickName
end

function Role:getIcon()
	return self.icon
end

function Role:getGender()
	return self.gender
end

function Role:getLevel()
	return self.level
end

function Role:getGold()
	return self.gold
end
--------------------------------------------------
function Role:save()
	local data = {}
	data.nickName = self.nickName
	data.gender = self.gender
	data.icon = self.icon
	data.level = self.level
	data.gold = self.gold
	dbHandler.request("role","save",self.roleId,Serialize(data))
end

function Role:load(data)
	local tempData = UnSerialize(data)
	self.nickName = tempData.nickName
	self.gender = tempData.gender
	self.icon = tempData.icon
	self.level = tempData.level
	self.gold = tempData.gold
end

function Role:postData()
	local response = Login_pb.LoginData()
	response.role.nickName = self.nickName
	response.role.gender = self.gender
	response.role.icon = self.icon
	response.role.level = self.level
	response.role.gold = self.gold

	local msg = response:SerializeToString()
	self:sendData(command.s2c_role_data, msg)
end

function Role:sendData(cmd, msg)
	self.connect.packet.luaSendData(cmd, msg)
end