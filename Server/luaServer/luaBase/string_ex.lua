local string = string
local setmetatable = setmetatable
local require = require
local type = type
local string1 = string
local table = table
--local core = require "md5.core"

module("luaBase.string_ex")

setmetatable(_M, {__index = string})

--字符串变16进制
toHex = function(str)
	local nLen = string.len(str)
	local szRet = "0x"
	for i = 1, nLen do
		local tmp =  string.format("%x", string.byte(string.sub(str, i, i)))
		if(string.len(tmp) == 1) then
			tmp = "0" .. tmp
		end
		szRet = szRet  .. tmp
	end
	return szRet
end

--16进制变字符串
HextoString = function(str)
	local nLen = string.len(str)
	local szRet = ""
	for i = 3, nLen, 2 do
		local tmp =  string.char("0x"..string.sub(str, i, i + 1))
		szRet = szRet..tmp
	end
	return szRet
end


md5 =function (k)
    if (k == nil or type(k) ~= "string") then
        return ""
    end
    k = core.sum(k)
    return (string1.gsub(k, ".", function (c)
        return string1.format("%02x", string1.byte(c))
         end))
end

--url编码函数
url_encode = function(str)
  if (str) then
    str = string.gsub (str, "\n", "\r\n")
    str = string.gsub (str, "([^%w ])",
        function (c) return string.format ("%%%02X", string.byte(c)) end)
    str = string.gsub (str, " ", "+")
  end
  return str
end

 --[[如果str全部由字母组成，则返回true，否则返回false。失败返回nil和失败信息]]
isalpha = function(str)
	if str == nil then
		return nil, "the string parameter is nil"
	end
	local len = string.len(str)
	for i = 1, len do
		local ch = string.sub(str, i, i)
		if not ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z')) then
			return false
		end
	end
	return true
end

--[[如果str全部由数字组成，则返回true，否则返回false。失败返回nil和失败信息]]
isdigit = function(str)
	if str == nil then
		return nil, "the string parameter is nil"
	end
	local len = string.len(str)
	for i = 1, len do
		local ch = string.sub(str, i, i)
		if ch < '0' or ch > '9' then
			return false
		end
	end
	return true
end


--火星文处理 
--去掉火星文中的会引出数据库出错的字符
trans_str = function(s) 
	s = string.gsub(s, "\\", " ") 
	s = string.gsub(s, "\"", " ") 
	s = string.gsub(s, "\'", " ") 
	s = string.gsub(s, "%)", " ") 
	s = string.gsub(s, "%(", " ") 
	s = string.gsub(s, "%%", " ") 
	s = string.gsub(s, "%?", " ") 
	s = string.gsub(s, "%*", " ") 
	s = string.gsub(s, "%[", " ") 
	s = string.gsub(s, "%]", " ") 
	s = string.gsub(s, "%+", " ") 
	s = string.gsub(s, "%^", " ") 
	s = string.gsub(s, "%$", " ") 
	s = string.gsub(s, ";", " ") 
	s = string.gsub(s, ",", " ") 
	s = string.gsub(s, "%-", " ") 
	s = string.gsub(s, "%.", " ") 
	return s 
end
--分割字符串, 用法 split("am,cc", ",")
function split(s, delim)
	if (not (type (delim) == "string" and string.len (delim) > 0)) then
		return {}
	end
	local start = 1  local t = {}
	while true do
		local pos = string.find (s, delim, start, true) -- plain find
		if not pos then
			break
		end
		table.insert (t, string.sub (s, start, pos - 1))
		start = pos + string.len (delim)
	end
	table.insert (t, string.sub (s, start))
	return t
end