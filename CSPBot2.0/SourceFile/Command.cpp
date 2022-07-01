#pragma once
#include <string>
#include <vector>
#include <ctime>
#include <optional>
#include <sstream>

#include "cspbot20.h"
#include "helper.h"
#include "websocketClient.h"
//#include "CPython.h"
#include "helper.h"
#include "server.h"
#include "global.h"

using namespace std;

string fmtMotdBE(string msgJson, string returnMsg);
string fmtMotdJE(string msgJson, string returnMsg);

vector<string> CommandAPI::SplitCommand(const std::string& paras)
{
	if (paras.empty())
		return vector<string>();

	vector<string> res;
	string now, strInQuote = "";
	istringstream strIn(paras);
	while (strIn >> now)
	{
		if (!strInQuote.empty())
		{
			strInQuote = strInQuote + " " + now;
			if (now.back() == '\"')
			{
				strInQuote.pop_back();
				res.push_back(strInQuote.erase(0, 1));
				strInQuote = "";
			}
		}
		else
		{
			if (now.front() == '\"')
			{
				if (now.back() == '\"')
				{
					now = now.substr(1, now.size() - 2);
					res.push_back(now);
				}
				else
					strInQuote = now;
			}
			else
				res.push_back(now);
		}
	}
	if (!strInQuote.empty())
	{
		istringstream leftIn(strInQuote);
		while (leftIn >> now)
			res.push_back(now);
	}
	for (int i = 0; i < res.size(); i++) {
		res[i] = Helper::replace(res[i], "\"", "");
	}
	return res;
}

std::unordered_map<string, string> commands ={};

void CommandAPI::CustomCmd(string cmd, string group) {
	vector<string> sp = SplitCommand(cmd);
	string Action_Type = sp[0];
	if (Action_Type == "bind") {
		if (sp.size() > 2) {
			bind(sp[1], sp[2]);
		}
		else {
			mirai->sendGroupMsg(group, u8"��������!");
		}
	}
	else if (Action_Type == "unbind") {
		if (sp.size() > 1) {
			Bind::unbind(sp[1]);
		}
		else {
			mirai->sendGroupMsg(group, u8"��������!");
		}
	}
	else if (Action_Type == "motdbe") {
		if (sp.size() > 2) {
			QRegExp r("(\\w.+):(\\w+)");
			int r_pos = r.indexIn(Helper::stdString2QString(sp[1]));
			if (r_pos > -1) {
				string motd_respone = Motd::motdbe(sp[1]);
				string fmt_respone;
				fmt_respone = fmtMotdBE(motd_respone, sp[2]);
				mirai->sendGroupMsg(group, fmt_respone);
			}
			else {
				mirai->sendGroupMsg(group, u8"Motd��ַ����!");
			}
		}
		else {
			mirai->sendGroupMsg(group, u8"��������!");
		}
	}
	else if (Action_Type == "motdje") {
		if (sp.size() > 2) {
			QRegExp r("(\\w.+):(\\w+)");
			int r_pos = r.indexIn(Helper::stdString2QString(sp[1]));
			if (r_pos > -1) {
				string motd_respone = Motd::motdje(sp[1]);
				string fmt_respone;
				fmt_respone = fmtMotdJE(motd_respone, sp[2]);
				mirai->sendGroupMsg(group, fmt_respone);
			}
			else {
				mirai->sendGroupMsg(group, u8"Motd��ַ����!");
			}
		}
		else {
			mirai->sendGroupMsg(group, u8"��������!");
		}
	}
	else if (Action_Type == "start") {
		mirai->sendGroupMsg(group, u8"���ڿ���������...");
		emit signalStartServer();
	}
	else if (Action_Type == "stop") {
		if (server->started) {
			server->stopServer();
		}
		else if (server->started != false && group != "0") {
			mirai->sendGroupMsg(group, u8"����������������");
		}
	}
	else if (commands.find(Action_Type) != commands.end()) {
		vector<string> args;
		int num = 0;
		for (auto& i : sp) {
			if (num != 0) {
				args.push_back(i);
			}
			num++;
		}
		//window->CommandCallback(Action_Type, args);
	}
}

