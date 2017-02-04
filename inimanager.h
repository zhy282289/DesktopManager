#pragma once

class ItemIni;
class IniManager : public QObject
{
	Q_OBJECT
public:
	static IniManager* GetInstance();
	void Release();
	QString ManagerPath();

	QList<ItemIni*> GetPanels();
	ItemIni* AddPanel();

	void DeleteIni(ItemIni *item);
public:
	IniManager(QObject *parent);
	~IniManager();

private:
	QString GetIniPath(QString appName);
	ItemIni* GetIni(QString appName);
	QString GetNewPanelName();
private:
	QString m_managerDir;
	QString m_managerPath;
};


struct ItemData
{
	QString path;
	QPoint pos;
	QPixmap icon;
};
typedef QList<ItemData> ItemDatas;


class ItemIni : public QObject
{
	Q_OBJECT
public:
	ItemIni(QString path, QObject *parent);

	QRect GetRect();
	bool GetHide();
	ItemDatas GetItems();
	void AddItem(QString path, QPoint p);
	QSize GetSize();

	void UpdateItem(const ItemData &data);
	void UpdateItems(const ItemDatas &datas);
	void UpdateRect(QRect rect);
	void UpdateHide(bool hide);

	void RemoveItem(const ItemData &data);

	QString GetPanelName();
	void SetPanelName(QString name);
	QString GetIniPath();

protected:
	ItemData LoadItemFromPath(QString path);
private:
	QString m_path;
	QString m_panelName;
};