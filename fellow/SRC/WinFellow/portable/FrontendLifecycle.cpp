/* Portable frontend lifecycle adapter.
 *
 * SDL and other portable frontends own their event loop and presentation.
 * These legacy WGUI entry points remain part of the common module lifecycle,
 * so provide frontend-neutral behavior rather than linking the Windows UI.
 */
#include <cstdio>

#include "FellowMain.h"

void wguiShowRequester(const char *message, FELLOW_REQUESTER_TYPE requesterType)
{
  const char *kind = "info";
  if (requesterType == FELLOW_REQUESTER_TYPE::FELLOW_REQUESTER_TYPE_WARN) kind = "warning";
  else if (requesterType == FELLOW_REQUESTER_TYPE::FELLOW_REQUESTER_TYPE_ERROR) kind = "error";

  std::fprintf(stderr, "FellowNG %s: %s\n", kind, message != nullptr ? message : "");
}

void wguiStartup()
{
}

void wguiShutdown()
{
}

void wguiSetProcessDPIAwareness(const char *)
{
  /* DPI awareness is a Windows process setting; portable frontends own DPI. */
}

BOOLE wguiEnter()
{
  /* The portable frontend owns the event loop. */
  return TRUE;
}
