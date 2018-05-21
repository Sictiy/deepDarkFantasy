local command = require "luaBase.command"
module(...,package.seeall)

function init()
	mysqlQuery([[
	CREATE TABLE IF NOT EXISTS `user` (
  		`name` varchar(60) NOT NULL,
  		`score` varchar(60) NOT NULL
	) ENGINE=MyISAM DEFAULT CHARSET=utf8
	]])
end

function loadall()
	local cmd = string.format("select * from user")
	local result = mysqlQuery(cmd)
	if #result.rows == 0 then
		print("loadall failed get null")
		return command.account_not_exists
	end

	local data = {}
	for k,v in ipairs(result.rows) do
		local user = {}
		user.name = v[1]
		user.score = tonumber(v[2])
		table.insert(data,user)
		print(user.name,user.score)
	end
	return command.ok,Serialize(data)
end

function insert(data)
	local user = UnSerialiaze(data)
	local sql = {}
	table.insert(sql,string.format("insert into user(name,score) value("))
	table.insert(sql,string.format("'%s',%d)",user.name,user.score))
	local cmd = table.concat(sql)
	print("1. insert into user: ",cmd)
	mysqlQuery(cmd)
	return command.ok
end