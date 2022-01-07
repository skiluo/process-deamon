#include <iostream>
#include <iterator>
#include <filesystem>
#include <Windows.h>
#include <io.h>

using namespace std;

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

int main(int argc, char* argv[]) {
    if(argc <= 1) {
        cout << "Allowed options: " << "process-deamon.exe xx.exe '{args}'";
        return 0;
    }

    char* propath = argv[1];
    char* arguments = "";
    if(argc >== 3) {
        arguments = argv[2];
    }

    string cmd = string(propath);
    string workspace = std::filesystem::path(cmd.c_str()).parent_path().string();
    cmd.append(" ");
    cmd.append(arguments);
    
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    memset(&si, 0, sizeof(STARTUPINFO));
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.mShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.hStdError = NULL;
    si.hStdOutput = NULL;

    cout << cmd << endl;
    do {
        if (_access(propath, 0) != -1 ) {
            if (!CreateProcessA(NULL, 
                    (LPSTR)cmd.c_str(),
                    NULL,
                    NULL,
                    TRUE,
                    CREATE_NO_WINDOW,
                    NULL,
                    workspace.empty() ? NULL : (LPSTR)workspace.c_str(),
                    &si,
                    &pi)){
                cout << "CreateProcess failed: %d" << GetLastError() << endl;
                return -1;
            }

            cout << "launch process, id: "<< pi.dwProcessId << endl;
            WaitForSingleObject(pi.hProcess, INFINITE);
            cout << propath << " exited, restart it" << endl;

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            cout << propath << " not exists" << endl; 
        }

        Sleep(1000);
    } while (true)

    return 0;
}