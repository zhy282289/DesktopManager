#pragma once


#include "inimanager.h"
class Item : public QWidget
{
	Q_OBJECT
public:
	Item(QWidget *parent);

	void SetData(ItemData data);
	const ItemData& GetData() const;
Q_SIGNALS:
	void Update();
	void Remove();

protected:
	void paintEvent(QPaintEvent *event);
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

	void mouseDoubleClickEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:
	ItemData m_data;
	bool m_hover;
	bool m_pressDown;
};

class DesktopManager;
class HideControl : public QObject
{
	Q_OBJECT
public:
	HideControl(DesktopManager *widget, ItemIni *itemIni, QObject *parent);

	void Hide();

Q_SIGNALS:
	void HideSignal();

protected:
	bool eventFilter(QObject *watcher, QEvent *event);
private:
	DesktopManager *m_widget;
	ItemIni *m_iniItem;
	int m_marings;
	QTimer *m_hideTimer;
	bool m_hide;
	QPoint m_showPoint;
	bool m_menuPopup;
};