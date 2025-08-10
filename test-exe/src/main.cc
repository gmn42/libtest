#include <Windows.h>
#include <filesystem>

int main() {
    // Set dll directory to current working directory
    std::filesystem::path currentPath = std::filesystem::current_path();

    printf("Current path: %s\n", currentPath.string().c_str());
    SetDllDirectoryA(currentPath.string().c_str());
    // Load library from current directory
    HMODULE hModule = LoadLibraryA("version.dll");
    if (hModule) {
        printf("Successfully loaded version.dll\n");
    } else {
        printf("Failed to load version.dll\n");
    }

    // print the path of the loaded library
    char path[MAX_PATH];
    if (GetModuleFileNameA(hModule, path, sizeof(path))) {
        printf("Loaded version.dll from: %s\n", path);
    } else {
        printf("Failed to get module file name\n");
    }

    // print version of loaded library
    typedef DWORD(WINAPI* GetFileVersionInfoSizeA_t)(LPCSTR, LPDWORD);
    typedef BOOL(WINAPI* GetFileVersionInfoA_t)(LPCSTR, DWORD, DWORD, LPVOID);
    typedef BOOL(WINAPI* VerQueryValueA_t)(LPCVOID, LPCSTR, LPVOID*, PUINT);

    GetFileVersionInfoSizeA_t pGetFileVersionInfoSizeA = (GetFileVersionInfoSizeA_t)GetProcAddress(hModule, "GetFileVersionInfoSizeA");
    GetFileVersionInfoA_t pGetFileVersionInfoA = (GetFileVersionInfoA_t)GetProcAddress(hModule, "GetFileVersionInfoA");
    VerQueryValueA_t pVerQueryValueA = (VerQueryValueA_t)GetProcAddress(hModule, "VerQueryValueA");

    if (pGetFileVersionInfoSizeA && pGetFileVersionInfoA && pVerQueryValueA) {
        DWORD handle;
        DWORD size = pGetFileVersionInfoSizeA(path, &handle);
        if (size) {
            LPVOID versionInfo = GlobalAlloc(GPTR, size);
            if (versionInfo) {
                if (pGetFileVersionInfoA(path, 0, size, versionInfo)) {
                    VS_FIXEDFILEINFO* fileInfo;
                    UINT fileInfoSize;
                    if (pVerQueryValueA(versionInfo, "\\", (LPVOID*)&fileInfo, &fileInfoSize)) {
                        printf("Version of loaded library: %d.%d.%d.%d\n",
                            HIWORD(fileInfo->dwFileVersionMS),
                            LOWORD(fileInfo->dwFileVersionMS),
                            HIWORD(fileInfo->dwFileVersionLS),
                            LOWORD(fileInfo->dwFileVersionLS));
                    }
                }
                GlobalFree(versionInfo);
            }
        }
    }

    return 0;
}
