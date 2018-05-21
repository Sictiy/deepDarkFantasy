local command = require "luaBase.command"
module(...,package.seeall)

function init()
	mysqlQuery([[
	CREATE TABLE IF NOT EXISTS `role` (
		`id` int NOT NULL primary key auto_increment,
  		`name` varchar(60) NOT NULL,
  		`gender` tinyint unsigned NOT NULL default 0,
  		`icon` smallint unsigned not null default 0,
  		`level` smallint unsigned not null default 0,
  		`gold` int unsigned not null default 0,
	) ENGINE=MyISAM DEFAULT CHARSET=utf8
	]])
end

function load(id)
	local cmd = string.format("select * from role where id = %d",id)
	local result mysqlQuery(cmd)
	if #result.rows == then
		return id, command.account_not_exists
	end

	local data = {}
	data.nickName = result.rows[1][2]
	data.gender = tonumber(result.rows[1][3])
	data.icon = tonumber(result.rows[1][4])
	data.level = tonumber(result.rows[1][5])
	data.gold = tonumber(result.rows[1][6])
	return id, command.ok, Serialize(data)
end

function create(id, data)
	local cmd = string.format("select * from role where id = %d",id)
	local result = mysqlQuery(cmd)
	if #result.rows ~= then
		return id, command.account_exists
	end
	local tempdata = UnSerialize(data)
	cmd = string.format("insert into role(name,gender,icon,gold,level) values('%s',%d.%d,%d,%d);",tempdata.nickName,tempdata.gender,tempdata.icon,tempdata.gold,tempdata.level)
	mysqlQuery(cmd)
	print("create role:",cmd)
	return id, command.ok
end

function save(id, data)
	local cmd = string.format("select * from role where id = %d",id)
	local result mysqlQuery(cmd)
	if #result.rows == 0 then
		return id, command.account_not_exists
	end
	local tempdata = UnSerialize(data)
	local sql = {}
	table.insert(sql,"update role set ")
	table.insert(sql,string.format("name = '%s',",tempdata.nickName))
	table.insert(sql,string.format("gender = %d, ",tempdata.gender))
	table.insert(sql,string.format("icon = %d, ",tempdata.icon))
	table.insert(sql,string.format("level = %d, ",tempdata.level))
	table.insert(sql,string.format("gold = %d, ",tempdata.gold))
	table.insert(sql,string.format("where id = %d;",id))
	cmd = table.concat(sql)
	print("save role:",cmd)
	mysqlQuery(cmd)
	return id, command.ok
end