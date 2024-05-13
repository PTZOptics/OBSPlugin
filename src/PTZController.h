#ifndef PTZCONTROLLER_H
#define PTZCONTROLLER_H

#include <obs-module.h>
#include <obs-frontend-api.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

extern const char *PLUGIN_NAME;
extern const char *PLUGIN_VERSION;

void obs_log(int log_level, const char *format, ...);
extern void blogva(int log_level, const char *format, va_list args);

obs_hotkey_id upHotkeyId, downHotkeyId, leftHotkeyId, rightHotkeyId, zoomInHotkeyId, zoomOutHotkeyId, presetCall1HotkeyId, presetCall2HotkeyId,
presetCall3HotkeyId, presetCall4HotkeyId, presetCall5HotkeyId, presetCall6HotkeyId;

void registerPTZHotkeys(obs_data_t *save_data, bool saving, void *private_data);
void savePTZHotkeys(obs_data_t *save_data, bool saving, void *private_data);

void ptzHotkeyCallback(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);
#endif
