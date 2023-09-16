#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, int difficulty, int ROWS, int COLS, int NUM_MINES)
    : QMainWindow(parent), ui(new Ui::MainWindow), ROWS(ROWS), COLS(COLS), NUM_MINES(NUM_MINES), mineCnt(NUM_MINES), swipedCnt(0)
{
    initWindow(difficulty);
    setupGame();
}

void MainWindow::initWindow(int difficulty)
{
    ui->setupUi(this);
    QMenu *menu = ui->menu_D;

    // 创建单选框动作组
    QActionGroup *actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true); // 设置为互斥的单选框

    // 创建单选框动作
    QAction *action1 = new QAction("初级", this);
    action1->setCheckable(true);
    action1->setChecked(difficulty == EASY); // 设置默认选中
    actionGroup->addAction(action1);
    action1->setProperty("difficulty", EASY);
    connect(action1, &QAction::triggered, this, &MainWindow::handleDifficultyChange);

    QAction *action2 = new QAction("中级", this);
    action2->setCheckable(true);
    action2->setChecked(difficulty == MEDIUM);
    actionGroup->addAction(action2);
    action2->setProperty("difficulty", MEDIUM);
    connect(action2, &QAction::triggered, this, &MainWindow::handleDifficultyChange);

    QAction *action3 = new QAction("高级", this);
    action3->setCheckable(true);
    action3->setChecked(difficulty == DIFFICULT);
    actionGroup->addAction(action3);
    action3->setProperty("difficulty", DIFFICULT);
    connect(action3, &QAction::triggered, this, &MainWindow::handleDifficultyChange);

    QAction *action4 = new QAction("自定义", this);
    action4->setCheckable(true);
    action4->setChecked(difficulty == CUSTOMIZE);
    actionGroup->addAction(action4);
    action4->setProperty("difficulty", CUSTOMIZE);
    connect(action4, &QAction::triggered, this, &MainWindow::handleDifficultyChange);

    // 将单选框动作添加到菜单
    menu->addAction(action1);
    menu->addAction(action2);
    menu->addAction(action3);
    menu->addAction(action4);

    QLCDNumber *lcdNumber = ui->lcdNumber;
    lcdNumber->setDigitCount(4);            // 设置显示的位数
    lcdNumber->setSegmentStyle(QLCDNumber::Filled); // 设置显示样式

    lcdTimer = new QTimer;
    lcdTimer->setInterval(1000); // 设置定时器间隔为1秒

    connect(lcdTimer, &QTimer::timeout, this, [=]()
    {
        int value = lcdNumber->intValue(); // 获取当前显示的数值
        lcdNumber->display(value + 1); // 将数值加1并显示在QLCDNumber上
    });

    setDifficulty(difficulty);
}

MainWindow::~MainWindow()
{
    resetGame();
    delete ui;
}

void MainWindow::customizeDifficulty()
{
    QDialog dialog;
    dialog.setWindowTitle("输入长、宽和雷数");
    QFormLayout layout(&dialog);

    // 添加标签和输入框
    QLabel labelLength("长：");
    QLineEdit editLength;
    editLength.setValidator(new QIntValidator(5, 40, &dialog)); // 设置输入只能为0到39的整数
    editLength.setText(QString::number(ROWS));          // 设置默认值为9
    layout.addRow(&labelLength, &editLength);

    QLabel labelWidth("宽：");
    QLineEdit editWidth;
    editWidth.setValidator(new QIntValidator(5, 40, &dialog)); // 设置输入只能为0到39的整数
    editWidth.setText(QString::number(COLS));          // 设置默认值为9
    layout.addRow(&labelWidth, &editWidth);

    QLabel labelMines("雷数：");
    QLineEdit editMines;
    editMines.setValidator(new QIntValidator(0, 40 * 40 - 9, &dialog)); // 设置输入只能为0到39的整数
    editMines.setText(QString::number(NUM_MINES));              // 设置默认值为10
    layout.addRow(&labelMines, &editMines);

    // 添加确定按钮
    QPushButton buttonOK("确定");
    layout.addRow(&buttonOK);

    // 连接按钮的点击事件
    this->setEnabled(false);
    dialog.raise();
    connect(&buttonOK, &QPushButton::clicked, this, [&]()
    {
        COLS = editLength.text().toInt();
        ROWS = editWidth.text().toInt();
        NUM_MINES = editMines.text().toInt();
        if (NUM_MINES > COLS *ROWS - 9) NUM_MINES = 0;

        // 在这里可以使用输入的值进行后续的处理
        this->setEnabled(true);
        dialog.close(); // 关闭对话框
    });
    connect(&dialog, &QDialog::finished, this, [&]()
    {
        this->setEnabled(true);
    });
    dialog.exec();
}

void MainWindow::setDifficulty(int difficulty)
{
    switch (difficulty)
    {
    case EASY:
        ROWS = 9, COLS = 9, NUM_MINES = 10;
        break;
    case MEDIUM:
        ROWS = 16, COLS = 16, NUM_MINES = 40;
        break;
    case DIFFICULT:
        ROWS = 16, COLS = 30, NUM_MINES = 99;
        break;
    case CUSTOMIZE:
        customizeDifficulty();
        break;
    default:
        break;
    }
}

void MainWindow::handleDifficultyChange()
{
    QAction *action = qobject_cast<QAction *>(sender());
    int difficulty = action->property("difficulty").toInt();
    MainWindow *newMainWindow = new MainWindow(nullptr, difficulty, ROWS, COLS, NUM_MINES);
    newMainWindow->show();
    // 关闭当前窗口
    close();
}

void MainWindow::handleButtonClick()
{
    MineButton *button = qobject_cast<MineButton *>(sender());

    if (onGameStart)
    {
        int row = button->property("row").toInt();
        int col = button->property("col").toInt();
        onGameStart = false;
        lcdTimer->start();
        setMines(row, col);
    }

    if (button->mineStatus() == MineButton::CLICKABLE)
    {
        int row = button->property("row").toInt();
        int col = button->property("col").toInt();

        if (mines[row][col])
        {
            button->setMineIcon();
            button->drawX();
            checkGameOver(true);
        }
        else
        {
            int count = countAdjacentMines(row, col);
            button->setClicked(true);

            if (count == 0)
            {
                revealAdjacentEmptyCells(row, col);
            }
            else
            {
                button->setNumber(count);
            }
            swipedCnt++;
            checkGameOver();
        }
    }
}

inline void MainWindow::checkGameOver(bool triger)
{
    if (triger || ROWS * COLS - swipedCnt == NUM_MINES)
    {
        lcdTimer->stop();
        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                if (mines[row][col] && buttons[row][col]->mineStatus() != MineButton::FLAGED)
                {
                    buttons[row][col]->setMineIcon();
                }
                if (!mines[row][col] && buttons[row][col]->mineStatus() == MineButton::FLAGED)
                {
                    buttons[row][col]->drawX();
                }
            }
        }
        gameOver(!triger);
        onGameStart = true;
    }
}

void MainWindow::syncMineCnt(bool _incflag)
{
    if (_incflag)
    {
        mineCnt--;
    }
    else
    {
        mineCnt++;
    }
    ui->lcdNumber_2->display(mineCnt);
}

void MainWindow::handleRButtonClick()
{
    MineButton *button = qobject_cast<MineButton *>(sender());
    int row = button->property("row").toInt();
    int col = button->property("col").toInt();

    switch (button->mineStatus())
    {
    case MineButton::CLICKABLE:
        button->setFlaged(true);
        syncMineCnt(true);
        break;

    case MineButton::FLAGED:
        button->setFlaged(false);
        syncMineCnt(false);
        break;

    case MineButton::CLICKED:
        if (countAdjacentMines(row, col) == countAdjacentFlags(row, col))
        {
            revealAdjacentEmptyCells(row, col);
        }
        else
        {
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    int newRow = row + i;
                    int newCol = col + j;

                    if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS)
                    {
                        buttons[newRow][newCol]->shinning();
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}

void MainWindow::setupGame()
{
    gridLayout = ui->gridLayout;
    ui->lcdNumber_2->display(NUM_MINES);

    // 初始化按钮和地雷
    for (int row = 0; row < ROWS; row++)
    {
        QVector<bool> rowMines;
        QVector<MineButton *> rowButtons;

        for (int col = 0; col < COLS; col++)
        {
            MineButton *button = new MineButton;
            button->setProperty("row", row);
            button->setProperty("col", col);
            connect(button, &MineButton::clicked, this, &MainWindow::handleButtonClick);
            connect(button, &MineButton::rClicked, this, &MainWindow::handleRButtonClick);

            rowMines.append(false);
            rowButtons.append(button);
            gridLayout->addWidget(button, row, col);
        }

        mines.append(rowMines);
        buttons.append(rowButtons);
    }
}

int MainWindow::countAdjacentMines(int row, int col)
{
    int count = 0;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int newRow = row + i;
            int newCol = col + j;

            if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS && mines[newRow][newCol])
            {
                count++;
            }
        }
    }

    return count;
}

int MainWindow::countAdjacentFlags(int row, int col)
{
    int count = 0;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int newRow = row + i;
            int newCol = col + j;

            if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS && buttons[newRow][newCol]->mineStatus() == MineButton::FLAGED)
            {
                count++;
            }
        }
    }

    return count;
}

void MainWindow::revealAdjacentEmptyCells(int row, int col)
{
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int newRow = row + i;
            int newCol = col + j;

            if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS && !onGameStart)
            {
                buttons[newRow][newCol]->click();
            }
        }
    }
}

void MainWindow::gameOver(bool win)
{
    if (!win)
    {
        QMessageBox::information(this, "游戏结束", "你踩中了地雷！游戏失败。");
    }
    else
    {
        QMessageBox::information(this, "游戏结束", "你清除了所有地雷！");
    }
    resetGame();
    setupGame();
}

void MainWindow::resetGame()
{
    for (QLayoutItem *Item = gridLayout->takeAt(0); Item != nullptr; Item = gridLayout->takeAt(0))
    {
        delete Item->widget();
        delete Item;
    }
    buttons.clear();
    mines.clear();
    ui->lcdNumber->display(0);
    ui->lcdNumber_2->display(NUM_MINES);
}

void MainWindow::setMines(int crow, int ccol)
{
    // 随机布雷
    int count = 0, row, col;
    while (count < NUM_MINES)
    {
        row = QRandomGenerator::global()->generateDouble() * ROWS;
        col = QRandomGenerator::global()->generateDouble() * COLS;
        if (!mines[row][col] && !(abs(row - crow) <= 1 && abs(col - ccol) <= 1))
        {
            mines[row][col] = true;
            count++;
        }
    }
    mineCnt = NUM_MINES;
    swipedCnt = 0;
}

void MainWindow::on_action_triggered()
{
    about_dialog ab(this);
    this->setEnabled(false);
    ab.raise();
    connect(&ab, &QDialog::finished, this, [this]()
    {
        this->setEnabled(true);
    });
    ab.show();
    ab.exec();
}
