Battle = {}
Battle.__index = Battle

local roomMgr = require "room.room"

function Battle:new()
	local self = {}
	setmetatable(self, Battle)
	return self
end

function Battle:create(id, roles)
	local self = Battle:new()
	self.id = id
	self.roles = roles
	if not self:init then
		self.dispose 
		return nil
	end
	return self
end

function Battle:init()
	self.frame = 0
	self.units = {}
	return true 
end

function Battle:dispose()
	roomMgr.closeRoom(self.id)
	for k,v in pairs(self.units) do 
		v:dispose
	end
	self = nil
end

function Battle:startBattle()
	-- body
end

function Battle:createUnit(roleid)
	-- body
end

function Battle:quitBattle(roleid, data)
	-- body
end

function Battle:checkBattleEnd()
	-- body
end

function Battle:update()
	-- body
end

---------------------------------
function Battle:checkBattleReady(roleid)
	-- body
end

function Battle:updateBattle(roleid, data)
	-- body
end