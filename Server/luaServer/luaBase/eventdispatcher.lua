module(...,package.seeall)

events = {}

function addEvent(eventName, modName, funcName)
	local event = events[eventName]
	if not event then
		event = {}
		events[eventName] = event
	end

	for i,v in ipairs(event) do
		if v.mod == modName and v.func == funcName then
			return
		end
	end 

	table.insert(event,{mod = modName, func = funcName})
end

function removeEvent(eventName, modName, funcName)
	local event = events[eventName]
	if not event then
		return 
	end

	for i,v in ipairs(event) do
		if v.mod == modName and v.func == funcName then
			table.remove(event,i)
		end
	end
end

function disPatcher(eventName,...)
	local event = events[eventName]
	if not event then
		return 
	end

	for i,v in ipairs(event) do
		local func = v.mod[v.func]
		if func then
			Call(eventName,...)
		end
	end
end