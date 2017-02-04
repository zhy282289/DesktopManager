#include "stdafx.h"
#include "desktopmanager.h"


#include "TopLevelWindowTitleBar_p.h"

#include "inimanager.h"

DesktopManager::DesktopManager(ItemIni * item, QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint);
	//setMinimumSize(100, 100);
	resize(400, 300);
	setAcceptDrops(true);
	setAttribute(Qt::WA_DeleteOnClose);

	m_bgPixmap = gGetDesktopWallpaper();

	m_toplevelControl = new TopLevelWindowTitleBarResizeMoveControl_p();
	m_toplevelControl->SetControlWidget(this, this);
	m_toplevelControl->SetDragMaximum(false);

	//QString appName = QApplication::applicationName();

	m_iniItem = item;
	setGeometry(m_iniItem->GetRect());
	

	RebuildItems();

	::SetParent((HWND)winId(), gFindDesktopHwnd());
	m_hideControl = new HideControl(this, m_iniItem, this);

	qApp->installEventFilter(this);
}

DesktopManager::~DesktopManager()
{

}

void DesktopManager::resizeEvent(QResizeEvent *event)
{
	m_iniItem->UpdateRect(geometry());
	update();
}

void DesktopManager::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	//if (!m_bgPixmap.isNull())
	//{
	//	painter.drawPixmap(-geometry().x(), -geometry().y(), m_bgPixmap);

	//}
	//else
	{
		painter.fillRect(rect(), Qt::black);
	}
	painter.fillRect(rect(), QColor(250, 250, 250, 50));
}

void DesktopManager::moveEvent(QMoveEvent *event)
{
	update();
	m_iniItem->UpdateRect(geometry());
}

void DesktopManager::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}

void DesktopManager::dragMoveEvent(QDragMoveEvent *event)
{
	event->accept();

}

void DesktopManager::dropEvent(QDropEvent *event)
{
	auto mime = event->mimeData();
	if (mime->hasUrls())
	{ 
		auto paths = mime->urls();
		for (auto temp : paths)
		{
			auto size = m_iniItem->GetSize();
			m_iniItem->AddItem(temp.toLocalFile(), event->pos() - QPoint(size.width() / 2, size.height() / 2));
			RebuildItems();
		}
		resizeEvent(nullptr);
	}
}

void DesktopManager::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		QMenu menu(this);
		auto sortAction = menu.addAction("Sort");
		auto hideAction = menu.addAction("Hide");
		hideAction->setVisible(false);
		auto addFile = menu.addAction("Add File");
		auto addAction = menu.addAction("Add Panel");
		auto resizeAction = menu.addAction("Resize && Move");
		auto updateWallpaperAction = menu.addAction("Update Wallpaper");
		updateWallpaperAction->setVisible(false);
		
		auto temp = menu.exec(event->globalPos());

		if (temp == sortAction)
		{
			const int margins = 15;
			int left = margins;
			int top = margins;
			int w = m_iniItem->GetSize().width();
			int h = m_iniItem->GetSize().height();

			auto items = m_iniItem->GetItems();
			for (ItemData &data : items)
			{
				data.pos = QPoint(left, top);
				left += w + margins;
				if (left + w + margins > width())
				{
					left = margins;
					top += h + margins;
				}
			}
			m_iniItem->UpdateItems(items);
			RebuildItems();
		}
		else if (temp == hideAction)
		{
			close();
		}
		else if (temp == addAction)
		{
			auto item = IniManager::GetInstance()->AddPanel();
			auto w = new DesktopManager(item);
			w->show();
		}
		else if (temp == addFile)
		{
			static QString prePath;
			QString path = QFileDialog::getOpenFileName(this, "add file", prePath);
			if (!path.isEmpty())
			{
				auto size = m_iniItem->GetSize();
				int count = m_items.size();
				m_iniItem->AddItem(path, event->pos() - QPoint(size.width() / 2, size.height() / 2));
				RebuildItems();
			}
		}
		else if (temp == updateWallpaperAction)
		{
			m_bgPixmap = gGetDesktopWallpaper();
			update();
		}
		else if (temp == resizeAction)
		{
			m_toplevelControl->SetResizeble(!m_toplevelControl->GetResizeble());
			m_toplevelControl->SetMoveable(!m_toplevelControl->GetMoveable());
		}
	}
	QWidget::mousePressEvent(event);
}

void DesktopManager::mouseMoveEvent(QMouseEvent *event)
{
	QWidget::mouseMoveEvent(event);
}

void DesktopManager::mouseReleaseEvent(QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
}

void DesktopManager::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Delete && event->modifiers() & Qt::ControlModifier)
	{
		auto result = QMessageBox::question(this, "warn", "will delete this panel, do u like it?");
		if (result == QMessageBox::Yes)
		{
			IniManager::GetInstance()->DeleteIni(m_iniItem);
			close();
		}
	}
}

void DesktopManager::mouseDoubleClickEvent(QMouseEvent *event)
{
	m_hideControl->Hide();
	QWidget::mouseDoubleClickEvent(event);
}

bool DesktopManager::eventFilter(QObject *watcher, QEvent *event)
{
	if (watcher == this)
	{
	}
	return false;
}


void DesktopManager::RebuildItems()
{
	qDeleteAll(m_items);
	m_items.clear();
	auto items = m_iniItem->GetItems();
	for (auto temp : items)
	{
		Item *item = new Item(this);
		item->resize(m_iniItem->GetSize());
		item->move(temp.pos);
		item->show();
		item->SetData(temp);
		m_items.push_back(item);

		connect(item, &Item::Update, this, [&](){
			auto item = qobject_cast<Item*>(sender());
			m_iniItem->UpdateItem(item->GetData());

		});
		connect(item, &Item::Remove, this, [&](){
			auto item = qobject_cast<Item*>(sender());
			m_iniItem->RemoveItem(item->GetData());
			item->close();
		});
	}
}

