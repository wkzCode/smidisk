#include "CommonUse.h"

class NamedPipe {
private:
	HANDLE hPipe;
	wstring pipeName;
	string buffer;
	DWORD bytesRead;
	DWORD bytesWritten;
public:
	NamedPipe(const string& pipeName, int time = 0);

	void write(string message);
	string read();

	~NamedPipe();
};
