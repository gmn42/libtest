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

    return 0;
}
