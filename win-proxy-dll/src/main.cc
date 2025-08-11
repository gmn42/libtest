#include <Windows.h>

#include <filesystem>
#include <mutex>

//#include "patches/patches.h"

void VersionDllInit();
void flog(const char* message);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpReserved*/)
{
  std::filesystem::path game_path;

  char logbuf[128];

  flog("");
  snprintf(logbuf, sizeof(logbuf), "DllMain called with reason %d", fdwReason);
  flog(logbuf);

  flog("testing lock_guard mutex");

  try {
    std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
  } catch (const std::exception& e) {
    snprintf(logbuf, sizeof(logbuf), "Exception in lock_guard: %s", e.what());
    flog(logbuf);
  }
  flog("got lock_guard mutex");

  switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
      //flog("(DisableThreadLibraryCalls)");

      //DisableThreadLibraryCalls(hinstDLL);

      TCHAR szFileName[MAX_PATH];
      GetModuleFileName(NULL, szFileName, MAX_PATH);

      game_path = szFileName;

      snprintf(logbuf, sizeof(logbuf), "Game path: %s", game_path.string().c_str());
      flog(logbuf);
      if (!game_path.filename().generic_wstring().starts_with(L"prime")) {
        return TRUE;
      }

      // Since we are replacing version.dll, need the proper forwards
      flog("(VersionDllInit)");
      VersionDllInit();

      flog("(Patch Initialization Starting)");

      snprintf(logbuf, sizeof(logbuf), "(stdout) Game path: %s", game_path.string().c_str());
      flog(logbuf);

      flog("(Patch Initialization Complete)");

      //ApplyPatches();
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}

// function to handle file logging
void flog(const char* message)
{

  // Create template for current time
  std::time_t now = std::time(nullptr);
  std::tm* timeinfo = std::localtime(&now);
  char timebuf[80];
  std::strftime(timebuf, sizeof(timebuf), "[%H:%M:%S] ", timeinfo);

  FILE* flog = fopen("patchlog.txt", "a");
  if (flog) {
    fprintf(flog, "%s%s\n", timebuf, message);
    fclose(flog);
  }
}

void* operator new[](size_t size, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/,
                     int /*line*/)
{
  return malloc(size);
}

void* operator new[](size_t size, size_t /*alignment*/, size_t /*alignmentOffset*/, const char* /*name*/, int /*flags*/,
                     unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
  return malloc(size);
}
