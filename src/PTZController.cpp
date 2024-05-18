#include <QMainWindow>
#include <QAction>
#include <QMessageBox>

#include "PTZController.h"
#include "mainwindow.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

MainWindow *ptzMainWindow;

bool obs_module_load(void)
{


	QMainWindow* main_window = (QMainWindow*)obs_frontend_get_main_window();
	QAction *action = (QAction*)obs_frontend_add_tools_menu_qaction(
		obs_module_text("PTZOptics Camera Controller"));

	obs_frontend_push_ui_translation(obs_module_get_string); 
	ptzMainWindow = new MainWindow(main_window);
	obs_frontend_pop_ui_translation();

	auto menu_cb = []
	{
		ptzMainWindow->setVisible(!ptzMainWindow->isVisible());
	};

	action->connect(action, &QAction::triggered, menu_cb);

	obs_frontend_add_preload_callback(registerPTZHotkeys, NULL);
	obs_frontend_add_save_callback(savePTZHotkeys, NULL);

	obs_log(LOG_INFO, "plugin loaded successfully (version %s)",
	PLUGIN_VERSION);
	
	return true;
}

void registerPTZHotkeys(obs_data_t *save_data, bool saving, void *private_data)
{
	upHotkeyId = obs_hotkey_register_frontend("PTZController.Move_Up", "Move Up", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array = obs_data_get_array(save_data, "PTZController.Move_Up");
	obs_hotkey_load(upHotkeyId, hotkey_save_array);

	downHotkeyId = obs_hotkey_register_frontend("PTZController.Move_Down", "Move Down", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array1 = obs_data_get_array(save_data, "PTZController.Move_Down");
	obs_hotkey_load(downHotkeyId, hotkey_save_array1);

	leftHotkeyId = obs_hotkey_register_frontend("PTZController.Move_Left", "Move Left", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array2 = obs_data_get_array(save_data, "PTZController.Move_Left");
	obs_hotkey_load(leftHotkeyId, hotkey_save_array2);

	rightHotkeyId = obs_hotkey_register_frontend("PTZController.Move_Right", "Move Right", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array3 = obs_data_get_array(save_data, "PTZController.Move_Right");
	obs_hotkey_load(rightHotkeyId, hotkey_save_array3);

	presetCall1HotkeyId = obs_hotkey_register_frontend("PTZController.Call_Preset1", "Call Preset 1", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array4 = obs_data_get_array(save_data, "PTZController.Call_Preset1");
	obs_hotkey_load(presetCall1HotkeyId, hotkey_save_array4);

	presetCall2HotkeyId = obs_hotkey_register_frontend("PTZController.Call_Preset2", "Call Preset 2", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array5 = obs_data_get_array(save_data, "PTZController.Call_Preset2");
	obs_hotkey_load(presetCall2HotkeyId, hotkey_save_array5);

	presetCall3HotkeyId = obs_hotkey_register_frontend("PTZController.Call_Preset3", "Call Preset 3", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array6 = obs_data_get_array(save_data, "PTZController.Call_Preset3");
	obs_hotkey_load(presetCall3HotkeyId, hotkey_save_array6);

	presetCall4HotkeyId = obs_hotkey_register_frontend("PTZController.Call_Preset4", "Call Preset 4", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array11 = obs_data_get_array(save_data, "PTZController.Call_Preset4");
	obs_hotkey_load(presetCall4HotkeyId, hotkey_save_array11);

	presetCall5HotkeyId = obs_hotkey_register_frontend("PTZController.Call_Preset5", "Call Preset 5", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array7 = obs_data_get_array(save_data, "PTZController.Call_Preset5");
	obs_hotkey_load(presetCall5HotkeyId, hotkey_save_array7);

	presetCall6HotkeyId = obs_hotkey_register_frontend("PTZController.Call_Preset6", "Call Preset 6", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array8 = obs_data_get_array(save_data, "PTZController.Call_Preset6");
	obs_hotkey_load(presetCall6HotkeyId, hotkey_save_array8);

	zoomInHotkeyId = obs_hotkey_register_frontend("PTZController.Zoom_In", "Zoom In", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array9 = obs_data_get_array(save_data, "PTZController.Zoom_In");
	obs_hotkey_load(zoomInHotkeyId, hotkey_save_array9);

	zoomOutHotkeyId = obs_hotkey_register_frontend("PTZController.Zoom_Out", "Zoom Out", ptzHotkeyCallback, NULL);
	obs_data_array_t *hotkey_save_array10 = obs_data_get_array(save_data, "PTZController.Zoom_Out");
	obs_hotkey_load(zoomOutHotkeyId, hotkey_save_array10);

	obs_data_array_release(hotkey_save_array);
}

void savePTZHotkeys(obs_data_t *save_data, bool saving, void *private_data)
{
	obs_data_array_t *hotkey_save_array = obs_hotkey_save(upHotkeyId);
	obs_data_set_array(save_data, "PTZController.Move_Up", hotkey_save_array);
	obs_data_array_release(hotkey_save_array);

	obs_data_array_t *hotkey_save_array1 = obs_hotkey_save(downHotkeyId);
	obs_data_set_array(save_data, "PTZController.Move_Down", hotkey_save_array1);
	obs_data_array_release(hotkey_save_array1);

	obs_data_array_t *hotkey_save_array2 = obs_hotkey_save(leftHotkeyId);
	obs_data_set_array(save_data, "PTZController.Move_Left", hotkey_save_array2);
	obs_data_array_release(hotkey_save_array2);

	obs_data_array_t *hotkey_save_array3 = obs_hotkey_save(rightHotkeyId);
	obs_data_set_array(save_data, "PTZController.Move_Right", hotkey_save_array3);
	obs_data_array_release(hotkey_save_array3);

	obs_data_array_t *hotkey_save_array4 = obs_hotkey_save(zoomInHotkeyId);
	obs_data_set_array(save_data, "PTZController.Zoom_In", hotkey_save_array4);
	obs_data_array_release(hotkey_save_array4);

	obs_data_array_t *hotkey_save_array5 = obs_hotkey_save(zoomOutHotkeyId);
	obs_data_set_array(save_data, "PTZController.Zoom_Out", hotkey_save_array5);
	obs_data_array_release(hotkey_save_array5);

	obs_data_array_t *hotkey_save_array6 = obs_hotkey_save(presetCall1HotkeyId);
	obs_data_set_array(save_data, "PTZController.Call_Preset1", hotkey_save_array6);
	obs_data_array_release(hotkey_save_array6);

	obs_data_array_t *hotkey_save_array7 = obs_hotkey_save(presetCall2HotkeyId);
	obs_data_set_array(save_data, "PTZController.Call_Preset2", hotkey_save_array7);
	obs_data_array_release(hotkey_save_array7);

	obs_data_array_t *hotkey_save_array8 = obs_hotkey_save(presetCall3HotkeyId);
	obs_data_set_array(save_data, "PTZController.Call_Preset3", hotkey_save_array8);
	obs_data_array_release(hotkey_save_array8);

	obs_data_array_t *hotkey_save_array9 = obs_hotkey_save(presetCall4HotkeyId);
	obs_data_set_array(save_data, "PTZController.Call_Preset4", hotkey_save_array9);
	obs_data_array_release(hotkey_save_array9);

	obs_data_array_t *hotkey_save_array10 = obs_hotkey_save(presetCall5HotkeyId);
	obs_data_set_array(save_data, "PTZController.Call_Preset5", hotkey_save_array10);
	obs_data_array_release(hotkey_save_array10);

	obs_data_array_t *hotkey_save_array11 = obs_hotkey_save(presetCall6HotkeyId);
	obs_data_set_array(save_data, "PTZController.Call_Preset6", hotkey_save_array11);
	obs_data_array_release(hotkey_save_array11);
}

void ptzHotkeyCallback(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed)
{
	if (ptzMainWindow == NULL || !ptzMainWindow->isActiveWindow())
		return;
	if (pressed)
	{
		if (id == upHotkeyId)
		{
			ptzMainWindow->onMoveTop();
		}
		else if (id == downHotkeyId)
		{
			ptzMainWindow->onMoveBottom();
		}
		else if (id == leftHotkeyId)
		{
			ptzMainWindow->onMoveLeft();
		}
		else if (id == rightHotkeyId)
		{
			ptzMainWindow->onMoveRight();
		}
		else if (id == zoomInHotkeyId)
		{
			ptzMainWindow->onZoomIn();
		}
		else if (id == zoomOutHotkeyId)
		{
			ptzMainWindow->onZoomOut();
		}
		else if (id == presetCall1HotkeyId)
		{
			ptzMainWindow->onCallPreset(1);
		}
		else if (id == presetCall2HotkeyId)
		{
			ptzMainWindow->onCallPreset(2);
		}
		else if (id == presetCall3HotkeyId)
		{
			ptzMainWindow->onCallPreset(3);
		}
		else if (id == presetCall4HotkeyId)
		{
			ptzMainWindow->onCallPreset(4);
		}
		else if (id == presetCall5HotkeyId)
		{
			ptzMainWindow->onCallPreset(5);
		}
		else if (id == presetCall6HotkeyId)
		{
			ptzMainWindow->onCallPreset(6);
		}
	}
	else
	{
		
		ptzMainWindow->onStopMoving();
	
		ptzMainWindow->onStopZooming();
		
	}
}

void obs_module_unload(void)
{
	obs_log(LOG_INFO, "plugin unloaded");
}

