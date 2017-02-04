/*
*Author:		zhy
*Date:			2016.09.30
*Description:	
*/

#ifndef TOPLEVELWINDOWTITLEBAR_P
#define TOPLEVELWINDOWTITLEBAR_P

class DashlineWidget;
class TopLevelWindowTitleBarResizeMoveControl_p : public QObject
{
	Q_OBJECT
public:
	TopLevelWindowTitleBarResizeMoveControl_p();

	void SetControlWidget(QWidget *titleBar, QWidget *toplevelWindow) ;

	void SetResizeble(bool enable);
	bool GetResizeble();
	void SetMoveable(bool enable);
	bool GetMoveable();

	void SetDragMaximum(bool enable) ;
	void SetDashlineColor(QColor color) ;
	void SetBorderColor(QColor color) ;

	bool IsMaximized() ;
	void ShowMaximized() ;
	void ShowNormal() ;
	void ShowMinimized() ;
	void CloseWindow() ;

protected:
	bool eventFilter(QObject *watched, QEvent *event);

	void MouseMove(QPoint p);
	bool CanResize();
	bool Resizeing();
	void SetResizeing(bool resize);

	
private:
	QWidget *m_titleBar;
	QWidget *m_toplevelWindow;
	bool m_toplevelWindowCanResize;
	bool m_toplevelDragMaximum;
	bool m_toplevelWindowFirstShow;

	bool m_mousePress;
	QPoint m_curPoint;

	bool m_left;
	bool m_top;
	bool m_right;
	bool m_bottom;

	bool m_resizeing;
	bool m_hint;
	DashlineWidget *m_dashlineWidget;


	///
	bool m_titleMousePress;
	bool m_maximizedDragResize;
	bool m_maximizedDragResize_fullscreen;
	QPoint m_titleousePressPoint;

	QRect m_normalGeometry;

	QColor m_borderColor;

	bool m_moveable;
	bool m_isMaximized;
private:
	friend class TopLevelWindowTitleBar_p;
};

class DashlineWidget :public QWidget
{
	Q_OBJECT
public:
	DashlineWidget(QWidget *parent);

	void SetColor(QColor color);

	void MovePosition(QRect r);
	void HideDashline();
	void ShowDashLine();

protected:
	void paintEvent(QPaintEvent *event);

	QColor m_color;
};
#endif // TIMELINEVIEWRULER_H
