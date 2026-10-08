// AceTR Voice 2.0 -- lightweight local IPC client, Win32/x64.
// Prototype ONLY. Program.cs currently accepts JOIN_PARTY and JOIN_GUILD.
// This does not transmit audio or authorize a real voice room.
#ifndef ACETR_VOICE_BRIDGE_H
#define ACETR_VOICE_BRIDGE_H
#include <windows.h>
#include <cstring>
namespace AceTRVoiceBridge {
inline bool SendPrototypeCommand(const char* command) {
    if (!command) return false;
    if (strcmp(command, "JOIN_PARTY") != 0 &&
        strcmp(command, "JOIN_GUILD") != 0) return false;
    const char* pipe = "\\\\.\\pipe\\AceTRVoiceV2";
    if (!WaitNamedPipeA(pipe, 100)) return false;
    HANDLE handle = CreateFileA(pipe, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) return false;
    char payload[32] = {0};
    const size_t length = strlen(command);
    if (length + 1 >= sizeof(payload)) { CloseHandle(handle); return false; }
    memcpy(payload, command, length);
    payload[length] = '\n';
    DWORD written = 0;
    const BOOL ok = WriteFile(handle, payload, (DWORD)(length + 1), &written, NULL);
    CloseHandle(handle);
    return ok != FALSE && written == length + 1;
}
}
#endif
