#ifndef MINEBUTTON_H
#define MINEBUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include <QTimer>
#include <QPalette>
#include <QPainter>
#include <QPen>

class MineButton : public QPushButton
{
Q_OBJECT
public:
private:
enum ButtonStatus {_clickable, _clicked, _flaged};
public:
static constexpr ButtonStatus CLICKABLE = _clickable;
static constexpr ButtonStatus CLICKED = _clicked;
static constexpr ButtonStatus FLAGED = _flaged;

private:
enum ButtonColor {_blue, _green, _red, _darkBlue, _darkRed, _cyan, _purple, _black};
public:
static constexpr ButtonColor BLUE = _blue;
static constexpr ButtonColor GREEN = _green;
static constexpr ButtonColor RED = _red;
static constexpr ButtonColor DARK_BLUE = _darkBlue;
static constexpr ButtonColor DARK_RED = _darkRed;
static constexpr ButtonColor CYAN = _cyan;
static constexpr ButtonColor PURPLE = _purple;
static constexpr ButtonColor BLACK = _black;

explicit MineButton(QWidget* parent = nullptr);
void setClicked(bool);
ButtonStatus mineStatus();
void setFlaged(bool);
void shinning();
void setNumber(int);
void setMineIcon();
void drawX();

template <typename T, typename std::enable_if<std::is_same<T, int>::value || std::is_same<T, ButtonColor>::value, bool>::type = true>
void setColor(T color)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::ButtonText, colors[color]);
    this->setPalette(palette);
}

signals:
void rClicked();
void shin();

protected:
void _shinning();
void mousePressEvent(QMouseEvent* event) override;
void paintEvent(QPaintEvent* event) override;

private:
ButtonStatus status;
static const QString MineStyleLight;
static const QString MineStyleDark;
static const QColor colors[];
bool _drawX=false;
};

#endif // MINEBUTTON_H
