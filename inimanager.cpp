#include "stdafx.h"
#include "inimanager.h"

#include <QDesktopServices>

static IniManager *s_IniManager = nullptr;
IniManager* IniManager::GetInstance()
{
	if (s_IniManager == nullptr)
	{
		s_IniManager = new IniManager(nullptr);

	}
	return s_IniManager;
}



void IniManager::Release()
{
	if (s_IniManager)
	{
		delete s_IniManager;
		s_IniManager = nullptr;
	}
}

QString IniManager::ManagerPath()
{
	return QStandardPaths::standardLocations(QStandardPaths::DataLocation)[0];
}

QList<ItemIni*> IniManager::GetPanels()
{
	QList<ItemIni*> ls;

	QSettings settings(m_managerPath, QSettings::IniFormat);
	auto keys = settings.allKeys();
	for (auto temp : keys)
	{
		ls.push_back(GetIni(temp));
	}

	if (ls.isEmpty())
	{
		ls.push_back(GetIni(GetNewPanelName()));
	}

	return ls;
}



ItemIni* IniManager::AddPanel()
{
	return GetIni(GetNewPanelName());
}

IniManager::IniManager(QObject *parent)
:QObject(parent)
{
	m_managerDir = ManagerPath();
	QDir dir(m_managerDir);
	if (!dir.exists())
		dir.mkpath(m_managerDir);
	m_managerPath = m_managerDir + "/manager.ini";

}


IniManager::~IniManager()
{
}

ItemIni* IniManager::GetIni(QString appName)
{
	auto temp = new ItemIni(GetIniPath(appName), this);
	temp->SetPanelName(appName);
	return temp;
}

QString IniManager::GetNewPanelName()
{
	QSettings settings(m_managerPath, QSettings::IniFormat);
	auto keys = settings.allKeys();

	QString newName;
	for (int i = 1;; ++i)
	{
		newName = QString("panel_%1").arg(i);
		bool bNewName = true;
		for (auto key : keys)
		{
			if (newName == key)
			{
				bNewName = false;
				break;
			}
		}
		if (bNewName)
		{
			break;
		}
	}
	return newName;
}

void IniManager::DeleteIni(ItemIni *item)
{
	QSettings settings(m_managerPath, QSettings::IniFormat);
	if (settings.contains(item->GetPanelName()))
	{
		settings.remove(item->GetPanelName());
	}
	QFile file(item->GetIniPath());
	if (file.exists())
		file.remove();
}

QString IniManager::GetIniPath(QString appName)
{
	QSettings settings(m_managerPath, QSettings::IniFormat);
	if (settings.contains(appName))
	{
		return settings.value(appName).toString();
	}
	else
	{
		QString tempPath = m_managerDir + "/" + appName + ".ini";
		settings.setValue(appName, tempPath);
		QFile file(tempPath);
		if (!file.exists())
			file.open(QIODevice::WriteOnly);

		return tempPath;
	}
}

//////////////////////////////////////////////////////////////////////////

ItemIni::ItemIni(QString path, QObject *parent)
:QObject(parent)
, m_path(path)
{
	
}

QRect ItemIni::GetRect()
{
	QSettings settings(m_path, QSettings::IniFormat);
	QRect rect = settings.value("Rect", QRect(300, 300, 600, 400)).value<QRect>();
	QRect availableRect = qApp->desktop()->availableGeometry();
	bool needReset = false;
	needReset = rect.width() + rect.x() <= 0;
	needReset |= rect.height() + rect.y() <= 0;
	needReset |= rect.x() >= availableRect.width();
	needReset |= rect.y() >= availableRect.height();
	if (needReset)
	{
		rect.moveLeft(10);
		rect.moveTop(10);
	}
	return rect;
}

bool ItemIni::GetHide()
{
	QSettings settings(m_path, QSettings::IniFormat);

	return settings.value("Hide", false).toBool();
}

ItemDatas ItemIni::GetItems()
{
	QSettings settings(m_path, QSettings::IniFormat);

	ItemDatas datas;
	settings.beginGroup("Icon");
	auto keys = settings.allKeys();
	for (auto temp : keys)
	{
		auto item = LoadItemFromPath(temp);
		item.pos = settings.value(temp, QPoint(100, 100)).toPoint();
		datas.push_back(item);
	}
	settings.endGroup();

	return datas;
}

void ItemIni::AddItem(QString path, QPoint p)
{
	QSettings settings(m_path, QSettings::IniFormat);

	ItemDatas datas;
	settings.beginGroup("Icon");
	settings.setValue(path, p);
	settings.endGroup();
}

QSize ItemIni::GetSize()
{
	return QSize(80, 80);
}

void ItemIni::UpdateItem(const ItemData &data)
{
	QSettings settings(m_path, QSettings::IniFormat);
	settings.beginGroup("Icon");
	if (settings.contains(data.path))
	{
		settings.setValue(data.path, data.pos);
	}
	settings.endGroup();
}

void ItemIni::UpdateItems(const ItemDatas &datas)
{
	QSettings settings(m_path, QSettings::IniFormat);
	settings.beginGroup("Icon");
	for (auto data : datas)
	{
		if (settings.contains(data.path))
		{
			settings.setValue(data.path, data.pos);
		}
	}

	settings.endGroup();
}

void ItemIni::UpdateRect(QRect rect)
{
	QSettings settings(m_path, QSettings::IniFormat);
	settings.setValue("Rect", rect);
}

void ItemIni::UpdateHide(bool hide)
{
	QSettings settings(m_path, QSettings::IniFormat);
	settings.setValue("Hide", hide);
}

void ItemIni::RemoveItem(const ItemData &data)
{
	QSettings settings(m_path, QSettings::IniFormat);
	settings.beginGroup("Icon");
	if (settings.contains(data.path))
	{
		settings.remove(data.path);
	}
	settings.endGroup();
}

QString ItemIni::GetPanelName()
{
	return m_panelName;
}

void ItemIni::SetPanelName(QString name)
{
	m_panelName = name;
}

QString ItemIni::GetIniPath()
{
	return m_path;
}

ItemData ItemIni::LoadItemFromPath(QString path)
{
	ItemData data;
	data.path = path;
	if (QFile::exists(path))
	{
		QFileInfo fileInfo(data.path);
		QFileIconProvider iconProvider;
		QIcon icon = iconProvider.icon(fileInfo);
		//data.icon = icon.pixmap(50, 60);
		data.icon = gGetFileIcon(path);
		
	}

	return data;
}
