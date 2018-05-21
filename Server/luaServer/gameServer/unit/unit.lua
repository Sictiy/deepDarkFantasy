Unit = {}
Unit.__index = Unit

local roleMgr = require "role.rolemgr"

function Unit:new()
	local self = {}
	setmetatable(self,Unit)
	return self
end

function Unit:create(id)
	local self = Unit:new()
	self.id = id
	if not self.init() then
		self.dispose()
		return nil
	end
	return self
end

function Unit:init()
	return true
end

function Unit:dispose()
	
end