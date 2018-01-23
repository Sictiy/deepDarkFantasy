#include "http_handler.h"
#include "stdafx.h"
#include "db_handler.h"
#include <time.h>
#include <regex>
#include <vector>

static char month[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static char week[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void splitstr(const std::string& s, const std::string seperator, std::vector<std::string>& vec){
	std::string::size_type pos1, pos2;
	pos2 = s.find(seperator);
	pos1 = 0;
	while(std::string::npos != pos2){
		vec.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + seperator.size();
		pos2 = s.find(seperator, pos1);
	}

	if(pos1 != s.length()){
		vec.push_back(s.substr(pos1));
	}
}

HTTP_Handler::HTTP_Handler(int fd):
		fd_(fd),
		content_length_(0)
{
	memset(ip_,0, INET_ADDRSTRLEN);
	init();
}

HTTP_Handler::~HTTP_Handler(){
	LogFormat(eLogInfo, "http_handler disposed! fd: %d", fd_);	
}

int HTTP_Handler::process(){
	Log(eLogInfo, "---------------------------");
	Log(eLogInfo, buff_);
	Log(eLogInfo, "---------------------------");
	std::string str = decode();
	//request("1234565");

	return 0;
}

void HTTP_Handler::disconnected(){
	LogFormat(eLogInfo, "SOCKET : %s[%d] disconnected!!!", ip_, port_);
}

void HTTP_Handler::init(){
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	if(getpeername(fd_, (struct sockaddr*)&addr, &addrlen) == -1){
		return;
	}

	char ip_addr[INET_ADDRSTRLEN];
	const char* ip = inet_ntop(addr.sin_family, &addr.sin_addr, ip_addr, INET_ADDRSTRLEN);
	memcpy(ip_, ip, INET_ADDRSTRLEN);
	port_ = ntohs(addr.sin_port);
}

void HTTP_Handler::request(char* data){
	std::string header = formate_header(strlen(data));
	std::string str(header.c_str());
	str.append(data);

	write(fd_, str.c_str(), str.length());
}

std::string HTTP_Handler::formate_header(int len){
	std::string str = "";
	str.append("HTTP/1.1 200 OK\r\n");
	str.append("Data: ");
	str.append(formate_date().c_str());
	str.append("\r\n");
	str.append("Server:  LoginServer@7road ZhichaoZheng\r\n");
	str.append("Access-Control-Allow-Origin: *\r\n");
	str.append("Access-Control-Allow-Methods: GET, OPTIONS, POST\r\n");
	str.append("Access-Control-Allow-Headers: X-Custom-Header\r\n");
	str.append("Content-Length: ");
	str.append(std::to_string(len));
	str.append("\r\n");
	str.append("Content-Type: text/plain\r\n\r\n");
	
	return str;
}

std::string HTTP_Handler::formate_date(){
	struct tm *t;

	time_t ltime_cur;
	ltime_cur = time(NULL);
	t = gmtime(&ltime_cur);

	char sz_time[100] = {0};
	sprintf(sz_time, "%s, %d %s %d %d:%d:%d GMT",
		week[t->tm_wday], t->tm_mday, month[t->tm_mon],
		t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);

	return sz_time;
}

std::string HTTP_Handler::decode(){
	std::string str(buff_);

	if(content_length_ > 0){
		decodeContent();
	}
	else if(str.find("POST") != std::string::npos){
		decodePost();
	}
	else if(str.find("GET") != std::string::npos){
		return decodeGet();
	}

	return "";
}

void HTTP_Handler::decodePost(){
	Log(eLogInfo, "2.Reach Post");
	std::string buf(buff_);

	/*std::regex patten1("Content-Length: \\d[0-9]");
	std::regex patten2("\\d[0-9]");
	std::smatch match1, match2;
	std::regex_search(buf, match1 , patten1);
	std::regex_search(match1.str(), match2, patten2);

	content_length_ = atoi(match2.str().c_str());*/
	int pos = buf.find("Content-Length:");
	if(pos != std::string::npos){
		Log(eLogInfo, "1. find content_length_ here!!");
		content_length_ = atoi(&buff_[pos+16]);
	}

	pos = buf.find("\r\n\r\n");
	std::string body = buf.substr(pos+4);
	LogFormat(eLogInfo, "=============decode post: %s, %d", body.c_str(), body.size());

	if(body.size() > 0){
		Log(eLogInfo,"1111111111111111");
		memset(buff_, 0, 2048);
		Log(eLogInfo, "2222222222222222");
		memcpy(buff_, body.c_str(), body.size());
		Log(eLogInfo, "3333333333333333");

		decodeContent();
	}
}

std::string HTTP_Handler::decodeGet(){
	Log(eLogInfo, "3.Reach GET");
	close(fd_);
	return "";
}

void HTTP_Handler::decodeContent(){
	content_length_ = 0;
	LogFormat(eLogInfo, "4. Decode Content : %s", buff_);
	std::vector<std::string> str;
	splitstr(buff_, ",", str);

	for(auto it : str){
		Log(eLogInfo, it.c_str());
	}

	if(!str.empty() && atoi(str[0].c_str()) == 1){
		request_login(atoi(str[2].c_str()), str[1].c_str());
	}
}

void HTTP_Handler::on_database_callback(int queryid, std::vector<std::vector<std::string>>&  ret){
	LogFormat(eLogInfo, "2.fd: %d", fd_);
	switch(queryid){
		case login_account:
			handle_login(ret);
			break;
		case create_account:
			handle_create(ret);
			break;
		default:
			LogFormat(eLogInfo, "Callback Query id : %d not founded!!", queryid);
			break;
	}
}

void HTTP_Handler::request_login(int ser_id, const char* usrname){
	std::string cmd_str = "select id from account where name = '";
	cmd_str += usrname;
	cmd_str += "';" ;

	LogFormat(eLogInfo, "1.Request Login: %s", cmd_str.c_str());

	db_command cmd;
	cmd.cmd = cmd_str;
	cmd.fd = fd_;
	cmd.query_id = login_account;
	DB_HANDLER->pushRequest(cmd);

	m_server_id = ser_id;
	m_usrname = usrname;
}

void HTTP_Handler::request_create(int ser_id, const char* usrname){
	std::string cmd_str = "insert into account(name) values('";
	cmd_str += usrname;
	cmd_str += "');";

	LogFormat(eLogInfo, "2.Request Create: %s", cmd_str.c_str());

	db_command cmd;
	cmd.cmd = cmd_str;
	cmd.fd = fd_;
	cmd.query_id = create_account;
	DB_HANDLER->pushRequest(cmd);

	m_server_id = ser_id;
	m_usrname = usrname;
}

void HTTP_Handler::handle_login(std::vector<std::vector<std::string>>& v){
	if(v.empty()){
		request_create(m_server_id, m_usrname.c_str());
	}
	else{
		int id = atoi(v[0][0].c_str());
		ServerAddr addr;
		DB_HANDLER->get_server(m_server_id, addr);

		char buf[50] = {0};
		sprintf(buf, "%d,%d,%s,%d", 0, id, addr.addr, addr.nPort);

		request(buf);
	}
}

void HTTP_Handler::handle_create(std::vector<std::vector<std::string>>& v){
	request_login(m_server_id, m_usrname.c_str());
}