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
  //auto console = spdlog::stdout_color_mt("console");

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
    // use spdlog to open a file logger
  try {
    // only create the logger if it doesn't exist
    if (!spdlog::get("logger")) {
      auto slog = spdlog::basic_logger_mt("logger", "spdlog.txt");
      spdlog::set_default_logger(slog);
      spdlog::set_pattern("[%H:%M:%S] [%l] %v");
      spdlog::flush_on(spdlog::level::info);
      slog->info("spdlog initialized");
    }
  } catch (const std::exception& e) {
    fprintf(flog, "(spdlog initialization failed: %s)\n", e.what());
    fflush(flog);
  }
    //console->info("(spdlog) Game path: {}", game_path.string());

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

      fprintf(stdout, "(stdout) Game path: %s\n", game_path.string().c_str());
      fflush(stdout);

      spdlog::info("Game path: {}", game_path.string());


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
