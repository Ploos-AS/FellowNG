#include <time.h>
#include <cstdarg>
#include <string>
#include <list>

#include "Service/Log.h"
#include "VirtualHost/Core.h"

constexpr auto WRITE_LOG_BUF_SIZE = 512;

using namespace std;

namespace Service
{
  Log::Log()
    : _new_line(true),
      _first_time(true),
      _enabled(true),
#ifdef _DEBUG
      _level(LogLevelDebug)
#else
      _level(LogLevelInformation)
#endif
  {
  }

  Log::~Log() = default;

  void Log::AddLogDebug(const char *format, ...)
  {
    if (!_enabled)
    {
      return;
    }

    if (_level >= LogLevelDebug)
    {
      va_list parms;
      char buffer[WRITE_LOG_BUF_SIZE];
      char *buffer2 = LogTime(buffer);

      va_start(parms, format);
      vsnprintf(buffer2, WRITE_LOG_BUF_SIZE - 1 - strlen(buffer), format, parms);
      FILE *F = OpenLogFile();
      if (F != nullptr)
      {
        AddLogInternal(F, buffer);
        CloseLogFile(F);
      }
      va_end(parms);
    }
  }

  void Log::AddLog(const char *format, ...)
  {
    if (!_enabled)
    {
      return;
    }

    va_list parms;
    char buffer[WRITE_LOG_BUF_SIZE];
    char *buffer2 = LogTime(buffer);

    va_start(parms, format);
    vsnprintf(buffer2, WRITE_LOG_BUF_SIZE - 1 - strlen(buffer), format, parms);
    FILE *F = OpenLogFile();
    if (F != nullptr)
    {
      AddLogInternal(F, buffer);
      CloseLogFile(F);
    }
    va_end(parms);
  }

  void Log::AddLogList(const list<string> &messages)
  {
    if (!_enabled)
    {
      return;
    }

    char buffer[WRITE_LOG_BUF_SIZE];
    FILE *F = OpenLogFile();

    for (const string &msg : messages)
    {
      char *buffer2 = LogTime(buffer);
      snprintf(buffer2, WRITE_LOG_BUF_SIZE - 1 - strlen(buffer), "%s\n", msg.c_str());
      AddLogInternal(F, buffer);
    }

    CloseLogFile(F);
  }

  void Log::AddLog2(const char *msg)
  {
    if (!_enabled)
    {
      return;
    }

    FILE *F = OpenLogFile();
    if (F != nullptr)
    {
      AddLogInternal(F, msg);
      CloseLogFile(F);
    }
  }

  void Log::AddTimelessLog(const char *format, ...)
  {
    char buffer[WRITE_LOG_BUF_SIZE];
    va_list parms;

    va_start(parms, format);
    vsnprintf(buffer, WRITE_LOG_BUF_SIZE - 1, format, parms);
    buffer[WRITE_LOG_BUF_SIZE - 1] = '\0';
    va_end(parms);

    AddLog2(buffer);
  }

  void Log::AddLogInternal(FILE *F, const char *msg)
  {
    fprintf(F, "%s", msg);
    _new_line = (msg[strlen(msg) - 1] == '\n');
  }

  char *Log::LogTime(char *buffer)
  {
    if (_new_line)
    {
      // log date/time into buffer
      time_t thetime = time(nullptr);
      struct tm timedata;
#ifdef _WIN32
      localtime_s(&timedata, &thetime);
#else
      localtime_r(&thetime, &timedata);
#endif
      strftime(buffer, 255, "%c: ", &timedata);
      // move buffer pointer ahead to log additional text after date/time
      return buffer + strlen(buffer);
    }

    // skip date/time, log to beginning of buffer
    buffer[0] = '\0';
    return buffer;
  }

  FILE *Log::OpenLogFile()
  {
    FILE *F = nullptr;

    if (_first_time)
    {
      char logfilename[FILEOPS_MAX_FILE_PATH];
      _core.Fileops->GetFellowLogfileName(logfilename);
      _logfilename = logfilename;
#ifdef _WIN32
      fopen_s(&F, _logfilename.c_str(), "w");
#else
      F = fopen(_logfilename.c_str(), "w");
#endif
      _first_time = false;
    }
    else
    {
#ifdef _WIN32
      fopen_s(&F, _logfilename.c_str(), "a");
#else
      F = fopen(_logfilename.c_str(), "a");
#endif
    }
    return F;
  }

  void Log::CloseLogFile(FILE *F)
  {
    fflush(F);
    fclose(F);
  }
}
