
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include <qprocess.h>

#include "title_bar.h"

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#include <qt_windows.h>
#endif

//#define USE_ICON

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(30);

    QPalette pal = this->palette();
    pal.setColor(QPalette::Background, Qt::darkMagenta);
    this->setPalette(pal);
    this->setAutoFillBackground(true);
#ifdef USE_ICON
    m_pIconLabel = new QLabel(this);
    m_pIconLabel->setFixedSize(20, 40);
    m_pIconLabel->setScaledContents(true);
#endif
    m_pTitleLabel = new QLabel(this);
    m_pTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pTitleLabel->setObjectName("whiteLabel");
    QPalette pa;
    pa.setColor(QPalette::WindowText, Qt::white);
    m_pTitleLabel->setPalette(pa);
    m_pTitleLabel->setText( QString::fromLocal8Bit("FuturesForcast.sparta.ver.1.0  ...˳��  ��ʱֹ�� ����...�а������µ�...��ʱƽ�ް��յ�...�侲 �͹�") ); // titile 

    m_psystemButton = new QPushButton("system", this);
    m_psystemButton->setFixedSize(50, 22);
    m_psystemButton->setObjectName("systemButton");
    m_psystemButton->setToolTip("system");

    m_pMinimizeButton = new QPushButton("-", this);
    m_pMinimizeButton->setFixedSize(27, 22);
    m_pMinimizeButton->setObjectName("minimizeButton");
    m_pMinimizeButton->setToolTip("Minimize");

    m_pMaximizeButton = new QPushButton(QString::fromLocal8Bit("��"), this);
    m_pMaximizeButton->setObjectName("maximizeButton");
    m_pMaximizeButton->setFixedSize(27, 22);
    m_pMaximizeButton->setToolTip("Maximize");

    m_pCloseButton = new QPushButton("X", this);
    m_pCloseButton->setObjectName("closeButton");
    m_pCloseButton->setToolTip("Close");
    m_pCloseButton->setFixedSize(27, 22);

    QHBoxLayout *pLayout = new QHBoxLayout(this);
#ifdef USE_ICON
    pLayout->addWidget(m_pIconLabel);
    pLayout->addSpacing(5);
#endif
    pLayout->addWidget(m_pTitleLabel);

    pLayout->addWidget(m_psystemButton);
    pLayout->addWidget(m_pMinimizeButton);
    pLayout->addWidget(m_pMaximizeButton);
    pLayout->addWidget(m_pCloseButton);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(5, 0, 5, 0);

    setLayout(pLayout);

    connect(m_psystemButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pMinimizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pMaximizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pCloseButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
}

TitleBar::~TitleBar()
{

}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    emit m_pMaximizeButton->clicked();
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
#ifdef Q_OS_WIN
    if (ReleaseCapture())
    {
        QWidget *pWindow = this->window();
        if (pWindow->isTopLevel())
        {
            SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
        }
    }
    event->ignore();
#else
#endif
}

bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::WindowTitleChange:
        {
            QWidget *pWidget = qobject_cast<QWidget *>(obj);
            if (pWidget)
            {
                m_pTitleLabel->setText(pWidget->windowTitle());
                return true;
            }
        }
    case QEvent::WindowIconChange:
        {
            QWidget *pWidget = qobject_cast<QWidget *>(obj);
            if (pWidget)
            {
                QIcon icon = pWidget->windowIcon();
                m_pIconLabel->setPixmap(icon.pixmap(m_pIconLabel->size()));
                return true;
            }
        }
    case QEvent::WindowStateChange:
    case QEvent::Resize:
        updateMaximize();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void TitleBar::onClicked()
{
    QPushButton *pButton = qobject_cast<QPushButton *>(sender());
    QWidget *pWindow = this->window();
    if (pWindow->isTopLevel())
    {
        if (pButton == m_pMinimizeButton)
        {
            pWindow->showMinimized();
        }
        else if (pButton == m_pMaximizeButton)
        {
            QDesktopWidget * deskTop = QApplication::desktop();
            int cur_monitor = deskTop->screenNumber(this);
            QRect  desk_rect = deskTop->availableGeometry(cur_monitor);

            //pWindow->isMaximized() ? pWindow->showNormal() : pWindow->showMaximized();
            if( pWindow->geometry() == desk_rect )
                pWindow->setGeometry(desk_rect.left(), desk_rect.top(), desk_rect.width() * 0.8, desk_rect.height() * 0.8);
            else
                pWindow->setGeometry(desk_rect);
        }
        else if (pButton == m_pCloseButton)
        {
            pWindow->close();
        }else if( pButton == m_psystemButton )
        {
            QProcess  start_forcast_tool;
            start_forcast_tool.startDetached("forcasttool.exe");
            start_forcast_tool.waitForStarted();
        }
    }
}

void TitleBar::updateMaximize()
{
    QWidget *pWindow = this->window();
    if (pWindow->isTopLevel())
    {
        bool bMaximize = pWindow->isMaximized();
        if (bMaximize)
        {
            m_pMaximizeButton->setToolTip(tr("Restore"));
            m_pMaximizeButton->setProperty("maximizeProperty", "restore");
        }
        else
        {
            m_pMaximizeButton->setProperty("maximizeProperty", "maximize");
            m_pMaximizeButton->setToolTip(tr("Maximize"));
        }

        m_pMaximizeButton->setStyle(QApplication::style());
    }
}
