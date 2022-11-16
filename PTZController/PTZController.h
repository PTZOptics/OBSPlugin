// PTZController.h : Include file for standard system include files,
// or project specific include files.
#ifndef PTZCONTROLLER_H
#define PTZCONTROLLER_H

#include <obs-module.h>
#include "obs-frontend-api.h"
#include <util/platform.h>
#include <util/threading.h>

obs_hotkey_id upHotkeyId, downHotkeyId, leftHotkeyId, rightHotkeyId, zoomInHotkeyId, zoomOutHotkeyId, presetCall1HotkeyId, presetCall2HotkeyId,
presetCall3HotkeyId, presetCall4HotkeyId, presetCall5HotkeyId, presetCall6HotkeyId;

void registerPTZHotkeys(obs_data_t *save_data, bool saving, void *private_data);
void savePTZHotkeys(obs_data_t *save_data, bool saving, void *private_data);

void ptzHotkeyCallback(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);
#endif
