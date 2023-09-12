#include "minebutton.h"

const QString MineButton::MineStyleLight("QPushButton:enabled { background-color: #FFFFFF; font-weight: bold; font-size: 18px; }");
const QString MineButton::MineStyleDark("QPushButton:enabled { background-color: #DDDDDD; font-weight: bold; font-size: 18px;}");
const QColor MineButton::colors[] = {
    QColor(0, 0, 255), // 蓝色，表示数字1
    QColor(0, 128, 0), // 绿色，表示数字2
    QColor(255, 0, 0), // 红色，表示数字3
    QColor(0, 0, 128), // 深蓝色，表示数字4
    QColor(128, 0, 0), // 深红色，表示数字5
    QColor(0, 128, 128), // 青色，表示数字6
    QColor(128, 0, 128), // 紫色，表示数字7
    QColor(0, 0, 0), // 黑色，表示数字8
};
// const QString MineButton::MineTextStyle("QPushButton:enabled { background-color: #FFFFFF; font-weight: bold; font-size: 14px; color: %1; }");
// setStyleSheet(MineTextStyle.arg("red"));

MineButton::MineButton(QWidget *parent)
    : QPushButton(parent), status(MineButton::CLICKABLE)
{
    setClicked(false);
    setFixedHeight(25);
    setFixedWidth(25);
    setMaximumHeight(40);
    setMaximumWidth(40);
    connect(this, &MineButton::shin, this, &MineButton::_shinning);
}

void MineButton::shinning()
{
    emit shin();
}

void MineButton::_shinning()
{
    if (status != MineButton::CLICKED) {
        QTimer *timer = new QTimer;
        timer->setInterval(50); // 设置闪烁间隔时间，单位为毫秒
        int *blinkCount = new int(6);
        timer->start();

        connect(timer, &QTimer::timeout, this, [timer, blinkCount, this]() {
            //setText(QString::number(*blinkCount));
            if ((*blinkCount)--) {
                    if ((*blinkCount) % 2) {
                            setStyleSheet(MineStyleLight);
                } else {
                            setStyleSheet(MineStyleDark);
                }
            } else {
                    if (status == MineButton::CLICKED) {setStyleSheet(MineStyleLight);}
                    timer->stop();
                    delete blinkCount;
                    delete timer;
            }
        });
    }
}

void MineButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        emit rClicked();
    }
    //QPushButton::mousePressEvent(event);
    else if (event->button() == Qt::LeftButton) {
        click();
    }
}

void MineButton::setClicked(bool clicked)
{
    if (clicked) {
        this->setStyleSheet(MineStyleLight);
        status = MineButton::CLICKED;
    }
    else {
        this->setStyleSheet(MineStyleDark);
        status = MineButton::CLICKABLE;
    }
}

MineButton::ButtonStatus MineButton::mineStatus()
{
    return status;
}

void MineButton::setFlaged(bool flaged)
{
    if (flaged) {
        setText("☥");
        status = MineButton::FLAGED;
    }else{
        setText("");
        status = MineButton::CLICKABLE;
    }
}

void MineButton::setNumber(int count)
{
    setText(QString::number(count));
    setColor(count - 1);
};

void MineButton::setMineIcon()
{
    setText("✿");
    setColor(CYAN);
}

void MineButton::drawX()
{
    _drawX = true;
    update();
}

void MineButton::paintEvent(QPaintEvent* event){
    QPushButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (_drawX) {
        // 设置画笔属性
        QPen pen(Qt::red);
        pen.setWidth(2);
        painter.setPen(pen);

        // 计算绘制X的位置
        QRect rect = this->rect();
        int x = rect.width() / 2;
        int y = rect.height() / 2;

        // 绘制红色的X
        painter.drawLine(QPoint(x - 4, y - 4), QPoint(x + 4, y + 4));
        painter.drawLine(QPoint(x - 4, y + 4), QPoint(x + 4, y - 4));
    }
}

