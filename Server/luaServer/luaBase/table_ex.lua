local setmetatable	= setmetatable
local getmetatable	= getmetatable
local table		= table
local loadstring1 = loadstring
local pairs		= pairs
local ipairs	= ipairs
local print		= print
local type		= type
local rawset	= rawset
local rawget	= rawget
local math		= math
local tostring1 = tostring
local string	= string
local debug		= debug
local _G		= _G
local assert	= assert
local next		= next
local require	= require
local tonumber  = tonumber

module("luaBase.table_ex")

setmetatable(_M, { __index = table })

loadstring = function(strData)
	if strData == nil or strData == "" then
		--TraceError("loadstring参数为nil 或者空的字串:<"..tostring1(strData)..">"..debug.traceback())
		return
	end
	local f = loadstring1(strData)
	if f then
		return f()
	end
end

tostring = function(t)
	local mark={}
	local assign={}
	local ser_table
	if type(t) ~= "table" then
		--TraceError("tostring参数为nil 或者空的字串:<"..tostring1(t)..">"..debug.traceback())
		return "do local ret={} return ret end"
	end
    ser_table = function (tbl,parent)
		mark[tbl]=parent
		local tmp={}
		for k,v in pairs(tbl) do
			local key= type(k)=="number" and "["..k.."]" or "[".. string.format("%q", k) .."]"
			if type(v)=="table" then
				local dotkey= parent.. key
				if mark[v] then
					table.insert(assign,dotkey.."='"..mark[v] .."'")
				else
					table.insert(tmp, key.."="..ser_table(v,dotkey))
				end
			elseif type(v) == "string" then
				table.insert(tmp, key.."=".. string.format('%q', v))
			elseif type(v) == "number" or type(v) == "boolean" then
				table.insert(tmp, key.."=".. tostring1(v))
			end
		end
		return "{"..table.concat(tmp,",").."}"
	end
	return "do local ret="..ser_table(t,"ret")..table.concat(assign," ").." return ret end"
end

clone = function(srctable)
	if (srctable == nil) then
		return nil
	else
		return loadstring(tostring(srctable))
	end
end

--注意只是简单复制对象
cloneex = function(srctable)
    local new_table = {}
    for k, v in pairs(srctable) do
        new_table[k] = v;
    end
    return new_table;
end

--打乱一个数组, 注意,没有返回值,直接对目标数组进行操作
disarrange = function(ref_array)
	local ret = {}
	while #ref_array > 0 do
		local n = math.random(1, #ref_array)
		table.insert(ret, ref_array[n])
		table.remove(ref_array, n)
	end
	for k, v in pairs(ret) do
		ref_array[k] = v
	end
end

--打乱一个数组, 注意,没有返回值,直接对目标数组进行操作, 随机算法用大数随机, 避免LUA小数随机出连续小数问题
disarrange_ex = function(ref_array)
	local ret = {}
	while #ref_array > 0 do
		local Count = #ref_array
		local n = math.random(1, 1000000000) % Count
		if 0 == n then
			n = Count
		end
		table.insert(ret, ref_array[n])
		table.remove(ref_array, n)
	end
	for k, v in pairs(ret) do
		ref_array[k] = v
	end
end

--交换table中的两个key
swap = function(ref_table, key1, key2)
	local tmp = ref_table[key1]
	ref_table[key1] = ref_table[key2]
	ref_table[key2] = tmp
end

--判断是否包含某个值,包含则返回key
finditemkey = function(tbl, value)
	for k, v in pairs(tbl) do
		if v == value then
			return k
		end
	end
	return
end

--找出一个table中的最值
findtop = function(ref_tbl, rulefunc)
	local topitem
	if not rulefunc then
		rulefunc = function(a, b) return a < b end
	end
	for k, v in pairs(ref_tbl) do
		if not topitem then
			topitem = v
		else
			if rulefunc(v, topitem) then
				topitem = v
			end
		end
	end
	return topitem
end

--合并数组
mergearray = function(...)
	local ret = {}
	for k, v in pairs({...}) do
		for k1, v1 in pairs(v) do
			table.insert(ret, v1)
		end
	end
	return ret
end

-- 树型打印
tree = function(x, header, path, mem)
	header = header or ""
	path = path or "/"
	mem = mem or {}

	local tp = type(x);
	if tp == "string" then
		return string.format("%q", x);
	end

	if tp == "table" then
		if not next(x) then
			return "{}";
		end

		local m = mem[x];
		if m then
			return "("..m..")";
		end
		mem[x] = path;

		local str_tab = {};
		for k,v in pairs(x) do
			local islast = not next(x, k);
			local next_header;

			table.insert(str_tab, header);
			if islast then
				table.insert(str_tab, "`");
				next_header = header .. "  ";
			else
				table.insert(str_tab, "|");
				next_header = header .. "| ";
			end

			local key = tostring1(k);
			if type(k) == "table" then
				key = "(*)\n" .. tree(k, next_header, path .. "[*]", mem);
			end

			if type(v) == "table" then
				table.insert(str_tab, "~");
				table.insert(str_tab, key);
				if mem[v] then
					table.insert(str_tab, "=");
				else
					table.insert(str_tab, "/\n");
				end
			else
				table.insert(str_tab, "-");
				table.insert(str_tab, key);
				table.insert(str_tab, "=");
			end

			table.insert(str_tab, tree(v, next_header, path .. tostring1(k).."/", mem));

			if not islast then
				table.insert(str_tab, "\n");
			end
		end

		return table.concat(str_tab);
	end

	return tostring1(x);
end

createEnum = function(tbl, index) 
    local enumtbl = {} 
    local enumindex = index or 0 
    for i, v in ipairs(tbl) do 
        enumtbl[v] = enumindex + i 
    end 
    return enumtbl 
end 

elementToNumber = function(mtable)
	local elements = nil

	for i, v in pairs(mtable) do
		local num = tonumber(v)
		if num ~= nil then
			if not elements then
				elements = {}
			end
			table.insert(elements, num)
		end
	end

	return elements
end
