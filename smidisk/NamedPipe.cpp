#include "NamedPipe.h"

NamedPipe::NamedPipe(const string& pipeName, int time) : hPipe(INVALID_HANDLE_VALUE) {
	wstring name(pipeName.begin(), pipeName.end());
	hPipe = CreateNamedPipe(
		name.c_str(),
		PIPE_ACCESS_DUPLEX,            //允许双向通信
		PIPE_TYPE_MESSAGE | PIPE_WAIT, //消息模式，调用会阻塞直到有客户端连接
		255,                           //允许连接的进程数
		266 * 1024,                    //输入缓冲区
		266 * 1024,                    //输出缓冲区
		time,
		NULL
	);
	if (hPipe == INVALID_HANDLE_VALUE) {
		throw runtime_error("failed to create named pipe");
	}
	cout << "Waiting for client to connect..." << endl;
	if (!ConnectNamedPipe(hPipe, NULL)) {
		CloseHandle(hPipe);
		throw string("failed connecting to named pipe");
	}
}

void NamedPipe::write(string message) {
	if (message.size() >= 272384) {
		message = message.substr(0, 272383);
		message += '\n';
	}
	const char* s = message.c_str();
	WriteFile(hPipe, s, strlen(s) + 1, &bytesWritten, NULL);
}

string NamedPipe::read() {
	char c[1024];
	if (ReadFile(hPipe, c, sizeof(c), &bytesRead, NULL)) {
		buffer = c;
	}
	else {
		buffer = "";
	}
	return buffer;
}

NamedPipe::~NamedPipe() {
	if (hPipe != INVALID_HANDLE_VALUE) {
		CloseHandle(hPipe);
	}
}