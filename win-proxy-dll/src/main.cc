#include <Windows.h>

#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


//#include "patches/patches.h"

void VersionDllInit();

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpReserved*/)
{
  std::filesystem::path game_path;

  // open log in append mode
  FILE *flog = fopen("patchlog.txt", "a");
  // Output current time to log
  if (!flog) {
    return FALSE; // Failed to open log file
  } 
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);  
  fprintf(flog, "Log opened at %02d:%02d:%02d on %02d/%02d/%04d\n", 
          timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
          timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
  fprintf(flog, "(version.dll entered with reason %d)\n", fdwReason);
  fflush(flog);

  switch (fdwReason) {
    case DLL_PROCESS_ATTACH:

      fprintf(flog, "(DisableThreadLibraryCalls)\n");
      fflush(flog); 
      DisableThreadLibraryCalls(hinstDLL);

      fprintf(flog, "(GetModuleFileName)\n");
      fflush(flog); 
      TCHAR szFileName[MAX_PATH];
      GetModuleFileName(NULL, szFileName, MAX_PATH);

      game_path = szFileName;

      fprintf(flog, "(Game Path: %s)\n", game_path.string().c_str());
      fflush(flog);
      if (!game_path.filename().generic_wstring().starts_with(L"prime")) {
        return TRUE;
      }

      // Since we are replacing version.dll, need the proper forwards
      fprintf(flog, "(VersionDllInit)\n");
      fflush(flog); 
      VersionDllInit();

      fprintf(flog, "(Patch Initialization Starting)\n");
      fflush(flog);

      spdlog::info("(spdlog) Game path: {}", game_path.string());
      //fprintf(stderr, "Game path: %s\n", game_path.string().c_str());

      fprintf(flog, "(Patch Initialization Complete)\n");

      //ApplyPatches();
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      break;
  }
  fclose(flog); 
  return TRUE;
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
