#ifndef TITLE_BAR
#define TITLE_BAR

#include <QtWidgets/QWidget>

class QLabel;
class QPushButton;

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = 0);
    ~TitleBar();

protected:

    // ˫�����������н�������/��ԭ
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

    // �����������϶�
    virtual void mousePressEvent(QMouseEvent *event);

    // ���ý��������ͼ��
    virtual bool eventFilter(QObject *obj, QEvent *event);

private slots:

    // ������С�������/��ԭ���رղ���
    void onClicked();

private:

    // ���/��ԭ
    void updateMaximize();

private:
    QLabel *m_pIconLabel;
    QLabel *m_pTitleLabel;
    QPushButton *m_psystemButton;
    QPushButton *m_pMinimizeButton;
    QPushButton *m_pMaximizeButton;
    QPushButton *m_pCloseButton;
};

#endif // TITLE_BAR