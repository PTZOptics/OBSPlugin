// PTZController.cpp : Defines the entry point for the application.
//


#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/platform.h>
#include <util/threading.h>

#include <QMainWindow>
#include <QAction>
#include <QMessageBox>
#include "PTZController.h"
#include "mainwindow.h"


OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-ptzcontroller", "en-US")

MainWindow *ptzMainWindow;

bool obs_module_load(void)
{

	QMessageBox msgBox;
	msgBox.setText("Loading...");
	msgBox.exec();
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
	
	return true;
}

void obs_module_unload(void)
{
}

