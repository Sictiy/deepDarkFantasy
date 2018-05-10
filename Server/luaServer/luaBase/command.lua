return{
	--db command
	ok = 0x1000,
	account_not_exists = 0x1001,
	--cs command
	c2s_rank_get  = 1,
	s2c_rank_get  = 2,

	c2s_login = 0x3001,
	s2c_role_data = 0x3002,
	c2s_battle_search = 0x3101,
	s2c_battle_search = 0x3102,

	--dh command
	h2d_request = 0x2001,
	d2h_request = 0x2002,

	--gh command
	g2h_regist = 0x4001,
	g2h_battle = 0x4002,
	h2g_battle = 0x4003,
}