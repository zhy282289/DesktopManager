
#include "stdafx.h"
#include "TopLevelWindowTitleBar_p.h"


//////////////////////////////////////////////////////////////////////////
TopLevelWindowTitleBarResizeMoveControl_p::TopLevelWindowTitleBarResizeMoveControl_p()
:QObject()
{
	m_hint = false;
	m_resizeing = false;
	m_mousePress = false;
	m_left = m_top = m_right = m_bottom = false;

	m_maximizedDragResize = false;
	m_titleMousePress = false;
	m_maximizedDragResize_fullscreen = false;

	m_titleBar = nullptr;
	m_toplevelWindow = nullptr;
	m_toplevelWindowCanResize = true;
	m_toplevelWindowFirstShow = true;
	m_toplevelDragMaximum = true;

	m_borderColor = QColor(73, 73, 250);

	m_moveable = true;
	m_isMaximized = false;
}

void TopLevelWindowTitleBarResizeMoveControl_p::SetControlWidget(QWidget *titleBar, QWidget *toplevelWindow)
{
	Q_ASSERT(titleBar);
	Q_ASSERT(toplevelWindow);
	m_titleBar = titleBar;
	m_toplevelWindow = toplevelWindow;

	setParent(titleBar);
	m_dashlineWidget = new DashlineWidget(titleBar);
	qApp->installEventFilter(this);
}



void TopLevelWindowTitleBarResizeMoveControl_p::SetResizeble(bool enable)
{
	m_toplevelWindowCanResize = enable;
	m_toplevelWindow->setCursor(Qt::ArrowCursor);
}

bool TopLevelWindowTitleBarResizeMoveControl_p::GetResizeble()
{
	return m_toplevelWindowCanResize;
}

void TopLevelWindowTitleBarResizeMoveControl_p::SetMoveable(bool enable)
{
	m_moveable = enable;
}

bool TopLevelWindowTitleBarResizeMoveControl_p::GetMoveable()
{
	return m_moveable;
}

void TopLevelWindowTitleBarResizeMoveControl_p::SetDragMaximum(bool enable)
{
	m_toplevelDragMaximum = enable;
}

void TopLevelWindowTitleBarResizeMoveControl_p::SetDashlineColor(QColor color)
{
	m_dashlineWidget->SetColor(color);
}

void TopLevelWindowTitleBarResizeMoveControl_p::SetBorderColor(QColor color)
{
	m_borderColor = color;
}


void TopLevelWindowTitleBarResizeMoveControl_p::MouseMove(QPoint p)
{
	if (!IsMaximized() && !Resizeing() && m_toplevelWindowCanResize)
	{
		const int margins = 4;
		QRect previewRect = m_toplevelWindow->geometry();
		QRect interRect = previewRect.adjusted(margins, margins, -margins, -margins);
		if (!interRect.contains(p) && previewRect.contains(p))
		{
			m_left = (p.x() >= previewRect.x() && p.x() <= interRect.x());
			m_top = (p.y() >= previewRect.y() && p.y() <= interRect.y());
			m_right = (p.x() >= interRect.right() && p.x() <= previewRect.right());
			m_bottom = (p.y() >= interRect.bottom() && p.y() <= previewRect.bottom());

			if (m_left && m_top || m_right && m_bottom)
				m_toplevelWindow->setCursor(Qt::SizeFDiagCursor);
			else if (m_left && m_bottom || m_top && m_right)
				m_toplevelWindow->setCursor(Qt::SizeBDiagCursor);
			else if (m_left || m_right)
				m_toplevelWindow->setCursor(Qt::SizeHorCursor);
			else if (m_top || m_bottom)
				m_toplevelWindow->setCursor(Qt::SizeVerCursor);
			else
				m_toplevelWindow->setCursor(Qt::ArrowCursor);

			m_hint = (m_left || m_top || m_right || m_bottom);
		}
		else
		{
			m_hint = false;
			m_toplevelWindow->setCursor(Qt::ArrowCursor);
		}
	}

}

bool TopLevelWindowTitleBarResizeMoveControl_p::eventFilter(QObject *watched, QEvent *event)
{

	auto type = event->type();
	if (type == QEvent::MouseMove)
	{
		MouseMove(static_cast<QMouseEvent*>(event)->globalPos());
	}

	if (watched == m_toplevelWindow)
	{
		if (type == QEvent::MouseButtonPress && m_hint && m_toplevelWindowCanResize)
		{
			m_mousePress = true;
			m_curPoint = static_cast<QMouseEvent*>(event)->pos();
			m_dashlineWidget->setGeometry(m_toplevelWindow->geometry());
			m_dashlineWidget->ShowDashLine();

			SetResizeing(true);
		}
		else if (type == QEvent::MouseMove && m_toplevelWindowCanResize)
		{
			if (m_mousePress && m_hint)
			{
				QPoint globalPos = static_cast<QMouseEvent*>(event)->globalPos();
				QRect rect = m_toplevelWindow->geometry();

				int minWidth = m_toplevelWindow->minimumSize().width();
				int minHeight = m_toplevelWindow->minimumSize().height();
				if (m_left)
				{
					int x = globalPos.x();
					if (rect.right() - globalPos.x() < minWidth)
						x = rect.right() - minWidth;
					rect.setX(x);

				}

				if (m_top)
				{
					int y = globalPos.y();
					if (rect.bottom() - globalPos.y() < minHeight)
						y = rect.bottom() - minHeight;
					rect.setY(y);

				}

				if (m_right)
					rect.setRight(globalPos.x());
				if (m_bottom)
					rect.setBottom(globalPos.y());

				rect.setWidth(qMax(rect.width(), minWidth));
				rect.setHeight(qMax(rect.height(), minHeight));

				m_dashlineWidget->MovePosition(rect);
			}

		}
		else if (type == QEvent::MouseButtonRelease && m_hint && m_toplevelWindowCanResize)
		{
			m_mousePress = false;
			SetResizeing(false);
			m_hint = m_left = m_top = m_right = m_bottom = false;

			m_toplevelWindow->setGeometry(m_dashlineWidget->geometry());
			m_normalGeometry = m_dashlineWidget->geometry();
			m_dashlineWidget->HideDashline();

		}
		else if (type == QEvent::Move)
		{
			auto ffff = m_toplevelWindow->geometry();
			if (!IsMaximized())
				m_normalGeometry = m_toplevelWindow->geometry();
		}
		else if (type == QEvent::Resize)
		{
			auto dd = m_toplevelWindow->geometry();
			int i = 0;
		}
		else if (type == QEvent::Show)
		{
			auto dd = m_toplevelWindow->geometry();
			int i = 0;
		}
		else if (type == QEvent::Paint)
		{
			if (m_toplevelWindowFirstShow)
			{
				m_toplevelWindowFirstShow = false;
				if (!IsMaximized())
					m_normalGeometry = m_toplevelWindow->geometry();
				else
				{
					QSize tempSize(1024, 610);
					auto size = m_toplevelWindow->minimumSize();
					if (size.isNull())
						size = tempSize;
					m_normalGeometry = QRect((QApplication::desktop()->availableGeometry().width() - size.width()) / 2,
						(QApplication::desktop()->availableGeometry().height() - size.height()) / 2,
						size.width(), size.height());
				}
			}

			if (!IsMaximized())
			{
				QPainter painter(m_toplevelWindow);
				QPen pen = painter.pen();
				pen.setWidth(4);
				pen.setColor(m_borderColor);
				painter.setPen(pen);
				painter.setBrush(Qt::NoBrush);
				painter.drawRect(m_toplevelWindow->rect());
			}

		}

	}
	if (watched == m_titleBar && !m_hint)
	{
		if (type == QEvent::MouseButtonPress || type == QEvent::MouseMove || type == QEvent::MouseButtonRelease)
		{
			QMouseEvent* pEvent = static_cast<QMouseEvent*>(event);
			if (pEvent->button() != Qt::RightButton)
			{
				if (type == QEvent::MouseButtonPress && m_moveable)
				{

					m_titleMousePress = true;
					m_titleousePressPoint = static_cast<QMouseEvent*>(event)->pos();


				}
				else if (type == QEvent::MouseMove)
				{
					auto pEvent = static_cast<QMouseEvent*>(event);
					QPoint offset = pEvent->pos() - m_titleousePressPoint;
					if (m_titleMousePress)
					{
						if (IsMaximized())
						{
							if (offset.manhattanLength() > QApplication::startDragDistance())
							{
								QRect maximizedRect = m_toplevelWindow->geometry();
								QRect rect = m_normalGeometry;
								rect.moveTopLeft(QPoint(pEvent->pos().x() - (1.0 * m_titleousePressPoint.x() / maximizedRect.width()*rect.width()), pEvent->pos().y()));

								if (rect.y() <= 2 && m_toplevelDragMaximum)
								{
									rect = QApplication::desktop()->availableGeometry();
									m_maximizedDragResize = false;
								}
								else
									m_maximizedDragResize = true;

								m_dashlineWidget->ShowDashLine();
								m_dashlineWidget->setGeometry(rect);


							}

						}
						else
						{
							auto curPos = m_toplevelWindow->geometry().topLeft() + offset;

							m_dashlineWidget->ShowDashLine();
							if (curPos.y() <= 2 && m_toplevelDragMaximum)
							{
								m_dashlineWidget->resize(QApplication::desktop()->availableGeometry().size());
								m_dashlineWidget->move(0, 0);
								m_maximizedDragResize_fullscreen = true;
								m_maximizedDragResize = false;
							}
							else
							{

								m_dashlineWidget->resize(m_toplevelWindow->size());
								m_dashlineWidget->move(curPos);
								m_maximizedDragResize_fullscreen = false;
								m_maximizedDragResize = true;
							}


							//m_nlePreview->move(m_nlePreview->geometry().topLeft() + offset);
						}
					}

				}
				else if (type == QEvent::MouseButtonRelease)
				{
					if (m_maximizedDragResize)
					{
						m_isMaximized = false;
						m_toplevelWindow->setGeometry(m_dashlineWidget->geometry());
						m_normalGeometry = m_dashlineWidget->geometry();
					}
					else if (m_maximizedDragResize_fullscreen)
					{
						ShowMaximized();

					}
					m_dashlineWidget->HideDashline();
					m_titleMousePress = false;
					m_mousePress = false;
					m_maximizedDragResize = false;
					m_maximizedDragResize_fullscreen = false;
				}
			}
		}



	}
	return false;
}

bool TopLevelWindowTitleBarResizeMoveControl_p::CanResize()
{
	return !IsMaximized();
}

bool TopLevelWindowTitleBarResizeMoveControl_p::Resizeing()
{
	return m_resizeing;
}

void TopLevelWindowTitleBarResizeMoveControl_p::SetResizeing(bool resize)
{
	m_resizeing = resize;

}




bool TopLevelWindowTitleBarResizeMoveControl_p::IsMaximized()
{
	return  m_isMaximized;
}

void TopLevelWindowTitleBarResizeMoveControl_p::ShowMaximized()
{
	m_isMaximized = true;
	m_toplevelWindow->show();
	m_toplevelWindow->setGeometry(QApplication::desktop()->availableGeometry());
}

void TopLevelWindowTitleBarResizeMoveControl_p::ShowNormal()
{
	m_isMaximized = false;
	m_toplevelWindow->setGeometry(m_normalGeometry);
}

void TopLevelWindowTitleBarResizeMoveControl_p::ShowMinimized()
{
	m_toplevelWindow->showMinimized();
}

void TopLevelWindowTitleBarResizeMoveControl_p::CloseWindow()
{
	m_toplevelWindow->close();
}

//////////////////////////////////////////////////////////////////////////
DashlineWidget::DashlineWidget(QWidget *parent)
:QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setAttribute(Qt::WA_TranslucentBackground);
	m_color = QColor(220, 220, 220, 180);

	HideDashline();
}

void DashlineWidget::SetColor(QColor color)
{
	m_color = color;
}

void DashlineWidget::MovePosition(QRect r)
{
	setGeometry(r);
	if (!isVisible())
		ShowDashLine();
}

void DashlineWidget::HideDashline()
{
	hide();
}

void DashlineWidget::ShowDashLine()
{
	show();
}

void DashlineWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QPen pen = painter.pen();
	//pen.setStyle(Qt::DashLine);
	pen.setWidth(8);
	pen.setColor(m_color);
	painter.setPen(pen);

	painter.drawRect(rect());
}
