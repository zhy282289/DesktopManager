#ifndef DESKTOPMANAGER_H
#define DESKTOPMANAGER_H

#include <QtWidgets/QWidget>

class TopLevelWindowTitleBarResizeMoveControl_p;
class HideControl;
class DesktopManager : public QWidget
{
	Q_OBJECT

public:
	DesktopManager(ItemIni * item, QWidget *parent = 0);
	~DesktopManager();

protected:
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);
	void moveEvent(QMoveEvent *event);

	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void mouseDoubleClickEvent(QMouseEvent *event);
	
	bool eventFilter(QObject *watcher, QEvent *event);


private:
	void RebuildItems();
private:
	QPixmap m_bgPixmap;
	ItemIni *m_iniItem;
	TopLevelWindowTitleBarResizeMoveControl_p *m_toplevelControl;

	QList<Item*> m_items;
	
	HideControl *m_hideControl;
	friend class HideControl;
};

#endif // DESKTOPMANAGER_H
