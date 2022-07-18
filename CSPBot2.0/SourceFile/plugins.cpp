#include <plugins.h>
#include <global.h>
#include <server.h>
#include <websocketClient.h>
#include <qmessagebox.h>
#include <tojson.hpp>
#include <helper.h>

using namespace std;
using namespace tojson;
using namespace luabridge;
int versionPacket = 1; // api�汾

// Buttons
enum SelfStandardButton {
	NoButton = 0,
	Ok = 1024,
	Save,
	SaveAll,
	Open,
	Yes,
	YesToAll,
	No,
	NoToAll,
	Abort,
	Retry,
	Ignore,
	Close,
	Cancel,
	Discard,
	Help,
	Apply,
	Reset,
	RestoreDefaults,
};

//######################### Server #########################

bool runcmd(const string& cmd, lua_State* L) {
	return server->sendCmd(cmd + '\n');
}

bool queryServerStarted(lua_State* L) {
	return server->getStarted();
}

//######################### Mirai #########################
// Mirai����Ϣ��API
bool ThreadMirai(string cbe, StringMap qm) {
	string type = cbe;
	if (type == "sendGroup") {
		string group = qm.at("group");
		string msg = qm.at("msg");
		mirai->sendGroupMsg(group, msg, false);
	}
	else if (type == "sendAllGroup") {
		string msg = qm.at("msg");
		mirai->sendAllGroupMsg(msg, false);
	}
	else if (type == "recallMsg") {
		string target = qm.at("target");
		mirai->recallMsg(target, false);
	}
	else if (type == "App") {
		string group = qm.at("group");
		string code = qm.at("code");
		mirai->send_app(group, code);
	}
	else if (type == "sendPacket") {
		string code = qm.at("packet");
		mirai->SendPacket(code);
	}
	return true;
}
// Mirai API
void sendGroup(const string& group, const string& msg, lua_State* L) {
	std::unordered_map<string, string> data;
	data.emplace("group", group);
	data.emplace("msg", msg);
	ThreadMirai("sendGroup", data);
}

void sendAllGroup(const string& msg, lua_State* L) {
	std::unordered_map<string, string> data;
	data.emplace("msg", msg);
	ThreadMirai("sendAllGroup", data);
}

void recallMsg(const string& target, lua_State* L) {
	std::unordered_map<string, string> data;
	data.emplace("target", target);
	ThreadMirai("recallMsg", data);
}

void sendApp(const string& group, const string& code, lua_State* L) {
	std::unordered_map<string, string> data;
	data.emplace("group", group);
	data.emplace("code", code);
	ThreadMirai("App", data);
}

void sendPacket(const string& packet, lua_State* L) {
	std::unordered_map<string, string> data;
	data.emplace("packet", packet);
	ThreadMirai("sendPacket", data);
}

//######################### Listener #########################

bool setListener(const string& eventName, const LuaRef& func, lua_State* L) {
	auto event_code = magic_enum::enum_cast<EventCode>(eventName);
	if (!event_code)
		throw std::invalid_argument("Invalid event name " + eventName);

	//��ӻص�����
	g_cb_functions[event_code.value()].push_back(func);
	return true;
}

//######################### Motd #########################

string MotdJE(const string& host, lua_State* L) {
	QRegExp r("(\\w.+):(\\w+)");
	int r_pos = r.indexIn(Helper::stdString2QString(host));
	if (r_pos > -1) {
		return Motd::motdje(host);
	}
	else {
		return "{}";
	}
}
string MotdBE(const string& host, lua_State* L) {
	QRegExp r("(\\w.+):(\\w+)");
	int r_pos = r.indexIn(Helper::stdString2QString(host));
	if (r_pos > -1) {
		return Motd::motdbe(host);
	}
	else {
		return "{}";
	}
}

//######################### Window  #########################

string QButtonToString(QMessageBox::StandardButton c) {
	if (c == QMessageBox::NoButton) {
		return "NoButton";
	}
	else if (c == QMessageBox::Ok) {
		return "Ok";
	}
	else if (c == QMessageBox::Save) {
		return "Save";
	}
	else if (c == QMessageBox::SaveAll) {
		return "SaveAll";
	}
	else if (c == QMessageBox::Open) {
		return "Open";
	}
	else if (c == QMessageBox::Yes) {
		return "Yes";
	}
	else if (c == QMessageBox::YesToAll) {
		return "YesToAll";
	}
	else if (c == QMessageBox::No) {
		return "No";
	}
	else if (c == QMessageBox::NoToAll) {
		return "NoToAll";
	}
	else if (c == QMessageBox::Abort) {
		return "Abort";
	}
	else if (c == QMessageBox::Retry) {
		return "Retry";
	}
	else if (c == QMessageBox::Ignore) {
		return "Ignore";
	}
	else if (c == QMessageBox::Close) {
		return "Close";
	}
	else if (c == QMessageBox::Cancel) {
		return "Cancel";
	}
	else if (c == QMessageBox::Discard) {
		return "Discard";
	}
	else if (c == QMessageBox::Help) {
		return "Help";
	}
	else if (c == QMessageBox::Apply) {
		return "Apply";
	}
	else if (c == QMessageBox::Reset) {
		return "Reset";
	}
	else if (c == QMessageBox::RestoreDefaults) {
		return "RestoreDefaults";
	}
	else {
		return "";
	}
}

QMessageBox::StandardButton StringToQButton(string c) {
	if (c == "NoButton") {
		return QMessageBox::NoButton;
	}
	else if (c == "Ok") {
		return QMessageBox::Ok;
	}
	else if (c == "Save") {
		return QMessageBox::Save;
	}
	else if (c == "SaveAll") {
		return QMessageBox::SaveAll;
	}
	else if (c == "Open") {
		return QMessageBox::Open;
	}
	else if (c == "Yes") {
		return QMessageBox::Yes;
	}
	else if (c == "YesToAll") {
		return QMessageBox::YesToAll;
	}
	else if (c == "No") {
		return QMessageBox::No;
	}
	else if (c == "NoToAll") {
		return QMessageBox::NoToAll;
	}
	else if (c == "Abort") {
		return QMessageBox::Abort;
	}
	else if (c == "Retry") {
		return QMessageBox::Retry;
	}
	else if (c == "Ignore") {
		return QMessageBox::Ignore;
	}
	else if (c == "Close") {
		return QMessageBox::Close;
	}
	else if (c == "Cancel") {
		return QMessageBox::Cancel;
	}
	else if (c == "Discard") {
		return QMessageBox::Discard;
	}
	else if (c == "Help") {
		return QMessageBox::Help;
	}
	else if (c == "Apply") {
		return QMessageBox::Apply;
	}
	else if (c == "Reset") {
		return QMessageBox::Reset;
	}
	else if (c == "RestoreDefaults") {
		return QMessageBox::RestoreDefaults;
	}
	else {
		return QMessageBox::FlagMask;
	}
}

//���쵯��
string ShowTipWindow(
	const string& type,
	const string& title,
	const string& content,
	const LuaRef& buttonType,
	lua_State* L) {
	QFlags<QMessageBox::StandardButton> btn;
	for (int i = 0; i < buttonType.length(); ++i) {
		string Btype = buttonType[i];
		auto event_code = StringToQButton(Btype);

		if (event_code == QMessageBox::FlagMask) {
			throw invalid_argument("Invalid StandardButton name " + Btype);
		}

		btn = btn | event_code;
	}
	//չʾ����
	QMessageBox::StandardButton Choosedbtn;
	if (type == "information") {
		Choosedbtn = QMessageBox::information(
			window,
			Helper::stdString2QString(title),
			Helper::stdString2QString(content),
			btn);
	}
	//ѯ��
	else if (type == "question") {
		Choosedbtn = QMessageBox::question(
			window,
			Helper::stdString2QString(title),
			Helper::stdString2QString(content),
			btn);
	}
	//����
	else if (type == "warning") {
		Choosedbtn = QMessageBox::warning(
			window,
			Helper::stdString2QString(title),
			Helper::stdString2QString(content),
			btn);
	}
	//����
	else if (type == "critical") {
		Choosedbtn = QMessageBox::critical(
			window,
			Helper::stdString2QString(title),
			Helper::stdString2QString(content),
			btn);
	}
	//δ֪
	else {
		throw invalid_argument("Invalid TipWindowType name " + type);
		return "";
	}

	//����ֵ
	return QButtonToString(Choosedbtn);
}

//######################### Command #########################

bool registerCommand(const string& cmd, const LuaRef& cbf, lua_State* L) {
	if (command.find(cmd) != command.end() &&
		cmd != "bind" &&
		cmd != "unbind" &&
		cmd != "motdbe" &&
		cmd != "motdje" &&
		cmd != "start" &&
		cmd != "stop") {
		return false;
		throw invalid_argument("Invalid command:" + cmd);
	}
	command.emplace(cmd, cbf);
	return true;
}

//######################### Player #########################
LuaRef queryInfo(const string& type, const string& arg, lua_State* L) {
	if (type != "qq" && type != "xuid" && type != "player") {
		throw std::invalid_argument("Invalid type:" + type);
	}

	auto queryData = Bind::queryXboxID(type, arg);
	string j = yaml2json(YAML::Dump(queryData)).dump();
	return {L, j};
}

bool unbindXbox(string qq, lua_State* L) {
	return Bind::unbind(qq);
}

bool bindXbox(string name, string qq, lua_State* L) {
	return Bind::bind(qq, name);
}

//######################### Info #########################
LuaRef getGroup(lua_State* L) {
	LuaRef groupList = newTable(L);
	std::ifstream fin("config/config.yml");
	YAML::Node config = YAML::Load(fin);
	bool inGroup = false;
	for (auto i : config["group"]) {
		groupList.append(i.as<string>());
	}
	return groupList;
}

LuaRef getAdmin(lua_State* L) {
	LuaRef groupList = newTable(L);
	std::ifstream fin("config/config.yml");
	YAML::Node config = YAML::Load(fin);
	bool inGroup = false;
	for (auto i : config["admin"]) {
		groupList.append(i.as<string>());
	}
	return groupList;
}

void EnableListener(EventCode evc) {
	enableEvent.emplace(evc, true);
}

lua_State* InitLua() {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	getGlobalNamespace(L)
		.beginClass<Logger>("Logger")
		.addConstructor<void (*)(string)>()
		.addFunction("info", std::function([](Logger* thiz, const string& msg, lua_State* L) { thiz->info(msg); }))
		.addFunction("debug", std::function([](Logger* thiz, const string& msg, lua_State* L) { thiz->debug(msg); }))
		.addFunction("error", std::function([](Logger* thiz, const string& msg, lua_State* L) { thiz->error(msg); }))
		.addFunction("error", std::function([](Logger* thiz, const string& msg, lua_State* L) { thiz->warn(msg); }))
		.endClass()
		.addFunction("getVersion", std::function([](lua_State* L) { return version; }))
		.addFunction("sendGroupMsg", &sendGroup)
		.addFunction("sendAllGroupMsg", &sendAllGroup)
		.addFunction("recallMsg", &recallMsg)
		.addFunction("sendApp", &sendApp)
		.addFunction("sendPacket", &sendPacket)
		.addFunction("setListener", &setListener)
		.addFunction("MotdBE", &MotdBE)
		.addFunction("MotdJE", &MotdJE)
		.addFunction("tip", &ShowTipWindow)
		.addFunction("getAPIVersion", std::function([](lua_State* L) { return versionPacket; }))
		.addFunction("registerCommand", &registerCommand)

		.addFunction("runCommand", &runcmd)
		.addFunction("getServerStatus", &queryServerStarted)

		.addFunction("queryData", &queryInfo)
		.addFunction("unbind", &unbindXbox)
		.addFunction("bind", &bindXbox)

		.addFunction("getAdmin", &getAdmin)
		.addFunction("getGroup", &getGroup);

	return L;
}

void DeinitLua(lua_State* L) {
	lua_close(L);
}
