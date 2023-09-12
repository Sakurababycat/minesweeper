#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QTime>
#include <QRandomGenerator>
#include <minebutton.h>
#include <about_dialog.h>
#include <QActionGroup>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
enum Difficulty {_easy, _medium, _difficult, _customize};
static constexpr Difficulty EASY = _easy;
static constexpr Difficulty MEDIUM = _medium;
static constexpr Difficulty DIFFICULT = _difficult;
static constexpr Difficulty CUSTOMIZE = _customize;
MainWindow(QWidget *parent = nullptr, int difficulty = EASY, int ROWS = 9, int COLS = 9, int NUM_MINES = 10);
~MainWindow();

private slots:
void handleButtonClick();

void handleRButtonClick();

void on_action_triggered();

void handleDifficultyChange();

private:
QGridLayout *gridLayout;
QVector<QVector<bool> > mines;
QVector<QVector<MineButton*> > buttons;

void setupGame();

void revealAdjacentEmptyCells(int row, int col);

int countAdjacentMines(int row, int col);

int countAdjacentFlags(int row, int col);

void gameOver(bool);

void syncMineCnt(bool);

void resetGame();

void setMines(int,int );

void customizeDifficulty();

void setDifficulty(int);

void initWindow(int difficulty=EASY);

inline void checkGameOver(bool triger=false);

Ui::MainWindow *ui;
int ROWS = 9;
int COLS = 9;
int NUM_MINES = 10;
int mineCnt, swipedCnt;
bool onGameStart = true;
QTimer *lcdTimer;
};
#endif // MAINWINDOW_H
