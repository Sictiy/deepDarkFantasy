Role = {}
Role.__index = Role


function Role:new()
	local self = {}
	setmetatable(self, Role)
	return self
end

function Role:create(id, connect)
	local self = Role:new()

	self.id = id
	self.connect = connect
	if not self.init() then
		self.dispose()
		return nil
	end
	return self
end

function Role:init()
	self.room = nil 
	self.battleId = nil
	return true
end

function Role:dispose()
	-- body
end

function Role:sendData(cmd, data)
	self.connect.packet.luaSendData(cmd,data)
end

function Role:bindBattle(battleId)
	if battleId ~= nil and self.battleId ~= nil then
		return false
	end
	self.battleId = battleId
	return true
end