local command = require "luaBase.command"
module(...,package.seeall)

function init()
	mysqlQuery([[
	CREATE TABLE IF NOT EXISTS `account` (
		`id` int NOT NULL primary key auto_increment,
  		`name` varchar(60) NOT NULL,
  		`passwd` varchar(60) NOT NULL
	) ENGINE=MyISAM DEFAULT CHARSET=utf8
	]])
end

function login(id, name)
	local cmd = string.format("select * from account where name = %s",name)
	local result = mysqlQuery(cmd)
	if #result.rows == 0 then
		print("login failed get null")
		return command.account_not_exists
	end

	local user = {}
	user.id = tonumber(result.rows[1][1])
	user.name = result.rows[1][2]
	user.passwd = result.rows[1][3]
	print("login user:",user)
	return id, command.ok, Serialize(user)
end

function create(id, name, passwd)
	local sql = {}
	table.insert(sql,string.format("insert into account(name, score) values("))
	table.insert(sql,string.format("'%s', '%s')",name, passwd))
	local cmd = table.concat(sql)
	print("1.insert into account:",cmd)
	mysqlQuery(cmd)
	cmd = string.format("select * from account where name = %s",name)
	local result = mysqlQuery(cmd)
	if #result.rows == 0 then
		print("insert failed")
		return command.account_not_exists 
	end
	local roleId = tonumber(result.rows[1][1])
	return id, command.ok, roleId
end