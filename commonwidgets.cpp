#include "stdafx.h"
#include "commonwidgets.h"

#include "TopLevelWindowTitleBar_p.h"

#include "desktopmanager.h"

Item::Item(QWidget *parent)
:QWidget(parent)
, m_hover(false)
, m_pressDown(false)
{
	
}

void Item::SetData(ItemData data)
{
	m_data = data;
	setToolTip(data.path);
	update();
}

const ItemData& Item::GetData() const
{
	return m_data;
}

void Item::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	const int textHeight = 25;
	int iconw = m_data.icon.width();
	int iconh = m_data.icon.height();
	painter.drawPixmap(QRect((width() - iconw)/2, ((height()-textHeight) - iconh)/2, iconw, iconh ), m_data.icon);


	QPen pen = painter.pen();
	pen.setColor(Qt::white);
	painter.setPen(pen);

	QFileInfo fileinfo(m_data.path);
	QFontMetrics metrics(font());
	auto text = metrics.elidedText(fileinfo.baseName(), Qt::ElideRight, width());
	painter.drawText(rect().adjusted(0, height() - textHeight, 0, 0), Qt::AlignCenter, text);
	//painter.setBrush(Qt::NoBrush);
	//painter.drawRect(rect());

	if (m_hover)
	{
		painter.fillRect(rect(), QColor(255, 255, 255, 100));
	}

}

void Item::enterEvent(QEvent *event)
{
	m_hover = true;
	update();
}

void Item::leaveEvent(QEvent *event)
{
	m_hover = false;
	update();
}

void Item::mouseDoubleClickEvent(QMouseEvent *event)
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(m_data.path));
}

void Item::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_pressDown = true;
	}
	else
	{
		QMenu menu(this);
		auto removeAction = menu.addAction("Remove");
		auto urlAction = menu.addAction("URL");

		auto temp = menu.exec(event->globalPos());
		if (temp == removeAction)
		{
			emit Remove();
		}
		else if (temp == urlAction)
		{
			QFileInfo fileInfo(m_data.path);
			QString dirPath;
			if (fileInfo.isDir())
				QDesktopServices::openUrl(QUrl::fromLocalFile(m_data.path));
			else
				QProcess::execute(QString("explorer /e,/select,%1").arg(QDir::toNativeSeparators(m_data.path)));
		}
	}
}

void Item::mouseMoveEvent(QMouseEvent *event)
{
	if (m_pressDown)
	{
		move(mapToParent(event->pos()) - QPoint(width()/2, height()/2));
	}
}

void Item::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_pressDown)
	{
		m_data.pos = mapToParent(event->pos()) - QPoint(width() / 2, height() / 2);
		emit Update();
		m_pressDown = false;
	}
}


//////////////////////////////////////////////////////////////////////////
HideControl::HideControl(DesktopManager *widget, ItemIni *itemIni, QObject *parent)
:QObject(parent)
, m_widget(widget)
, m_iniItem(itemIni)
, m_menuPopup(false)
, m_marings(1)
{
	qApp->installEventFilter(this);

	m_hide = m_iniItem->GetHide();
	m_widget->m_toplevelControl->SetResizeble(false);
	m_widget->m_toplevelControl->SetMoveable(false);

	m_hideTimer = new QTimer(this);
	m_hideTimer->setInterval(3 * 1000);
	m_hideTimer->setSingleShot(true);
	connect(m_hideTimer, &QTimer::timeout, this, &HideControl::Hide);


}

//void HideControl::SetHide(bool hide)
//{
//	m_hide = hide;
//}

void HideControl::Hide()
{

	QRect rect = m_widget->geometry();
	bool left = rect.x() < 0;
	bool top = rect.y() < 0;
	bool right = rect.x() + rect.width() > QApplication::desktop()->availableGeometry().width();
	bool bottom = rect.y() + rect.height() > QApplication::desktop()->availableGeometry().height();

	QPoint dstPoint(rect.x(), rect.y());
	if (top)
	{
		dstPoint.setY(m_marings - rect.height());
	}
	else if (left)
	{
		dstPoint.setX(m_marings - rect.width());
	}
	else if (right)
	{
		dstPoint.setX(QApplication::desktop()->availableGeometry().width() - m_marings);
	}
	else if (bottom)
	{
		dstPoint.setY(QApplication::desktop()->availableGeometry().height() - m_marings);
	}

	if (left || top || right || bottom)
	{
		m_hide = true;
		m_showPoint = m_widget->pos();
		m_widget->move(dstPoint);
		m_widget->m_toplevelControl->SetResizeble(false);
		m_widget->m_toplevelControl->SetMoveable(false);
		m_iniItem->UpdateHide(m_hide);
		emit HideSignal();
	}
}

bool HideControl::eventFilter(QObject *watcher, QEvent *event)
{
	if (watcher == m_widget)
	{
		if (event->type() == QEvent::Leave)
		{
			if (!m_hide && !m_menuPopup)
				m_hideTimer->start();
		}
		else if (event->type() == QEvent::Enter)
		{
			m_menuPopup = false;
			if (m_hideTimer->isActive())
				m_hideTimer->stop();
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			auto pEvent = static_cast<QMouseEvent*>(event);
			if (pEvent->button() == Qt::LeftButton)
			{
				if (m_hide)
				{
					m_widget->move(m_showPoint);
					//m_widget->m_toplevelControl->SetResizeble(true);
					m_hide = false;
					m_iniItem->UpdateHide(m_hide);
				}
			}
			else
			{
				m_menuPopup = true;
			}
		}
		else if (event->type() == QEvent::Move)
		{
			//auto pEvent = static_cast<QMoveEvent*>(event);

		}
		else if (event->type() == QEvent::Show)
		{
			auto pEvent = static_cast<QShowEvent*>(event);
			QRect rect = m_widget->geometry();

			m_showPoint = m_widget->pos();
			if (m_hide)
			{
				if (m_showPoint.y() < 0)
				{
					m_showPoint.setY(-m_marings);
				}
				if (m_showPoint.x() < 0)
				{
					m_showPoint.setX(-m_marings);
				}
				
				else if (m_showPoint.x() + rect.width() > QApplication::desktop()->availableGeometry().width())
				{
					m_showPoint.setX(QApplication::desktop()->availableGeometry().width() - rect.width() + m_marings);
				}
				else if (m_showPoint.y() + rect.height() > QApplication::desktop()->availableGeometry().height())
				{
					m_showPoint.setY(QApplication::desktop()->availableGeometry().height() - rect.height() - m_marings);
				}

			}
		}
	}
	else
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			auto pEvent = static_cast<QMouseEvent*>(event);
			if (pEvent->button() == Qt::RightButton)
			{
				m_menuPopup = true;
			}
		}
	}
	return false;
}
