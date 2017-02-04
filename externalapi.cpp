#include "stdafx.h"
#include "externalapi.h"

static BOOL enumUserWindowsCB(HWND hwnd, LPARAM lParam)
{
	long wflags = GetWindowLong(hwnd, GWL_STYLE);
	if (!(wflags & WS_VISIBLE)) return TRUE;

	HWND sndWnd;
	if (!(sndWnd = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL))) return TRUE;

	HWND targetWnd;
	if (!(targetWnd = FindWindowEx(sndWnd, NULL, L"SysListView32", L"FolderView"))) return TRUE;

	HWND* resultHwnd = (HWND*)lParam;
	*resultHwnd = targetWnd;

	return FALSE;
}

HWND gFindDesktopHwnd()
{
	HWND resultHwnd = NULL;
	EnumWindows((WNDENUMPROC)enumUserWindowsCB, (LPARAM)&resultHwnd);
	return resultHwnd;
}

QPixmap gGetDesktopWallpaper()
{
	QSettings settings("\\HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
	auto ddd = settings.value("Wallpaper").toString();
	QPixmap wallpaperPixmap(settings.value("Wallpaper").toString());
	if (!wallpaperPixmap.isNull())
		wallpaperPixmap = wallpaperPixmap.scaled(QApplication::desktop()->screenGeometry().size());
	return wallpaperPixmap;
}

extern QPixmap qt_pixmapFromWinHICON(HICON icon);
QPixmap gGetFileIcon(QString file_name)
{
	QPixmap pixmap;
	if (!file_name.isEmpty())
	{
		//»ñÈ¡ºó×º
		std::string str = "file";
		int index = file_name.lastIndexOf(".");
		//if (index >= 0)
		//{
		//	QString suffix = file_name.mid(index);
		//	str = suffix.toLocal8Bit();
		//	LPCSTR name = str.c_str();

		//	SHFILEINFOA info;
		//	if (SHGetFileInfoA(name,
		//		FILE_ATTRIBUTE_NORMAL,
		//		&info,
		//		sizeof(info),
		//		SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES))
		//	{
		//		HICON icon = info.hIcon;
		//		//file_icon = QIcon(QPixmap::fromWinHICON(icon));
		//		pixmap = qt_pixmapFromWinHICON(icon);
		//	}
		//}
		//else
		{
			QFileInfo fileInfo(file_name);
			QFileIconProvider iconProvider;
			QIcon icon = iconProvider.icon(fileInfo);
			pixmap = icon.pixmap(35, 42);
		}

	}

	return pixmap;
}
