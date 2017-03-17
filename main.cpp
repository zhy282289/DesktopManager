#include "stdafx.h"
#include "desktopmanager.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);


	auto panels = IniManager::GetInstance()->GetPanels();
	for (auto temp : panels)
	{
		DesktopManager *w = new DesktopManager(temp);
		w->show();
	}
	

	auto h1 = gFindDesktopHwnd();
	HWND h2 = (HWND)a.desktop()->winId();

	return a.exec();
}
