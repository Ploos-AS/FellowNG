/* Portable configuration state for FellowNG frontends.
 *
 * The Windows implementation also owns WinFellow.ini persistence and GUI
 * history. Portable frontends only require a deterministic current/default
 * configuration state; frontend-specific persistence can be layered on later.
 */
#include <cstdlib>
#include <cstring>

#include "IniFile.h"
#include "VirtualHost/Core.h"

iniManager ini_manager{};

namespace {
void copyField(char *destination, const char *source)
{
  if (source == nullptr) source = "";
  std::strncpy(destination, source, CFG_FILENAME_LENGTH - 1);
  destination[CFG_FILENAME_LENGTH - 1] = '\0';
}

ini *newIni()
{
  auto *state = static_cast<ini *>(std::calloc(1, sizeof(ini)));
  if (state == nullptr) return nullptr;

  char defaultConfig[CFG_FILENAME_LENGTH]{};
  if (_core.Fileops != nullptr)
    _core.Fileops->GetDefaultConfigFileName(defaultConfig);
  copyField(state->m_current_configuration, defaultConfig);
  state->m_pauseemulationwhenwindowlosesfocus = TRUE;
  return state;
}
}

int iniGetMainWindowXPos(ini *state) { return state ? state->m_mainwindowxposition : 0; }
int iniGetMainWindowYPos(ini *state) { return state ? state->m_mainwindowyposition : 0; }
int iniGetEmulationWindowXPos(ini *state) { return state ? state->m_emulationwindowxposition : 0; }
int iniGetEmulationWindowYPos(ini *state) { return state ? state->m_emulationwindowyposition : 0; }

void iniSetMainWindowPosition(ini *state, uint32_t x, uint32_t y)
{
  if (state) { state->m_mainwindowxposition = static_cast<int>(x); state->m_mainwindowyposition = static_cast<int>(y); }
}
void iniSetEmulationWindowPosition(ini *state, uint32_t x, uint32_t y)
{
  if (state) { state->m_emulationwindowxposition = static_cast<int>(x); state->m_emulationwindowyposition = static_cast<int>(y); }
}

const char *iniGetConfigurationHistoryFilename(ini *state, uint32_t position)
{
  return state && position < 4 ? state->m_configuration_history[position] : "";
}
void iniSetConfigurationHistoryFilename(ini *state, uint32_t position, const char *value)
{
  if (state && position < 4) copyField(state->m_configuration_history[position], value);
}
const char *iniGetCurrentConfigurationFilename(ini *state) { return state ? state->m_current_configuration : ""; }
void iniSetCurrentConfigurationFilename(ini *state, const char *value) { if (state) copyField(state->m_current_configuration, value); }

#define INI_STRING_ACCESSORS(SetName, GetName, Field) \
void SetName(ini *state, const char *value) { if (state) copyField(state->Field, value); } \
const char *GetName(ini *state) { return state ? state->Field : ""; }

INI_STRING_ACCESSORS(iniSetLastUsedCfgDir, iniGetLastUsedCfgDir, m_lastusedconfigurationdir)
INI_STRING_ACCESSORS(iniSetLastUsedKickImageDir, iniGetLastUsedKickImageDir, m_lastusedkickimagedir)
INI_STRING_ACCESSORS(iniSetLastUsedKeyDir, iniGetLastUsedKeyDir, m_lastusedkeydir)
INI_STRING_ACCESSORS(iniSetLastUsedGlobalDiskDir, iniGetLastUsedGlobalDiskDir, m_lastusedglobaldiskdir)
INI_STRING_ACCESSORS(iniSetLastUsedHdfDir, iniGetLastUsedHdfDir, m_lastusedhdfdir)
INI_STRING_ACCESSORS(iniSetLastUsedModDir, iniGetLastUsedModDir, m_lastusedmoddir)
INI_STRING_ACCESSORS(iniSetLastUsedStateFileDir, iniGetLastUsedStateFileDir, m_lastusedstatefiledir)
INI_STRING_ACCESSORS(iniSetLastUsedPresetROMDir, iniGetLastUsedPresetROMDir, m_lastusedpresetromdir)

void iniSetLastUsedCfgTab(ini *state, uint32_t tab) { if (state) state->m_lastusedconfigurationtab = tab; }
uint32_t iniGetLastUsedCfgTab(ini *state) { return state ? state->m_lastusedconfigurationtab : 0; }
BOOLE iniGetPauseEmulationWhenWindowLosesFocus(ini *state) { return state ? state->m_pauseemulationwhenwindowlosesfocus : FALSE; }
void iniSetPauseEmulationWhenWindowLosesFocus(ini *state, BOOLE pause) { if (state) state->m_pauseemulationwhenwindowlosesfocus = pause; }

void iniManagerSetCurrentInitdata(iniManager *manager, ini *state) { if (manager) manager->m_current_ini = state; }
ini *iniManagerGetCurrentInitdata(iniManager *manager) { return manager ? manager->m_current_ini : nullptr; }
void iniManagerSetDefaultInitdata(iniManager *manager, ini *state) { if (manager) manager->m_default_ini = state; }
ini *iniManagerGetDefaultInitdata(iniManager *manager) { return manager ? manager->m_default_ini : nullptr; }

void iniManagerStartup(iniManager *manager)
{
  if (manager == nullptr) return;
  manager->m_current_ini = newIni();
  manager->m_default_ini = newIni();
}
void iniManagerShutdown(iniManager *manager)
{
  if (manager == nullptr) return;
  std::free(manager->m_default_ini);
  std::free(manager->m_current_ini);
  manager->m_default_ini = nullptr;
  manager->m_current_ini = nullptr;
}

BOOLE iniSetOption(ini *, char *) { return FALSE; }
BOOLE iniSaveOptions(ini *, FILE *) { return TRUE; }

void iniStartup() { iniManagerStartup(&ini_manager); }
void iniShutdown() { iniManagerShutdown(&ini_manager); }
void iniEmulationStart() {}
void iniEmulationStop() {}
