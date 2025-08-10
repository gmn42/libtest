#include <Windows.h>

#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

//#include "patches/patches.h"

void VersionDllInit();
void flog(const char* message);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpReserved*/)
{
  std::filesystem::path game_path;

  char logbuf[128];

  snprintf(logbuf, sizeof(logbuf), "DllMain called with reason %d", fdwReason);
  flog(logbuf);

  flog("testing lock_guard mutex\n");

  std::mutex mtx;
  std::lock_guard<std::mutex> lock(mtx);
  flog("got lock_guard mutex");

  try {
    // only create the logger if it doesn't exist
    flog("Checking if spdlog logger exists");
    if (!spdlog::get("logger")) {
      flog("Creating spdlog logger");
      auto slog = spdlog::basic_logger_mt("logger", "spdlog.txt");
      spdlog::set_default_logger(slog);
      spdlog::set_pattern("[%H:%M:%S] [%l] %v");
      spdlog::flush_on(spdlog::level::info);
      slog->info("spdlog initialized");
    }
  } catch (const std::exception& e) {
    snprintf(logbuf, sizeof(logbuf), "spdlog initialization failed: %s", e.what());
    flog(logbuf);
  }


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

// void* operator new[](size_t size, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/,
//                      int /*line*/)
// {
//   return malloc(size);
// }

// void* operator new[](size_t size, size_t /*alignment*/, size_t /*alignmentOffset*/, const char* /*name*/, int /*flags*/,
//                      unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
// {
//   return malloc(size);
// }
