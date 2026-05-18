#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>
#include <QPushButton>
#include <QShortcut>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QLabel>
#include <QHeaderView>
#include <QTimer>
#include <QIcon>
#include <QApplication>

#include "displaywindow.h"
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->currentTimeLabel->setText("00:00.0");
    setWindowIcon(QIcon(":/images/Alvsutloppet_Logo.ico"));
    setUpTables();
    setUpComboBoxes();
    connectInput();
    updateButtonsEnabled();

    displayWindow = new DisplayWindow();
    auto screens = QGuiApplication::screens();
    if(screens.size() > 1)
    {
        QRect screenGeometry = screens[1]->geometry();
        displayWindow->setGeometry(screenGeometry);
        displayWindow->showFullScreen();
    }
    else
    {
        displayWindow->show();
    }

    updateDisplay();

    // Startup Page set
    ui->stackedWidget->setCurrentIndex(0);

    this->setCentralWidget(ui->centralwidget);

    setUpTopLayout();
    setUpTimingLayout();
    setUpRegisterLayout();
    setUpExportLayout();
    setUpRandomLayout();
    setUpNewRecordLayout();

    //Start page
    auto startLayout = new QVBoxLayout(ui->starting);
    startLayout->setContentsMargins(40, 40, 40, 40);
    startLayout->addStretch();
    startLayout->addWidget(ui->startImageLabel, 0, Qt::AlignCenter);
    startLayout->addStretch();

    ui->startImageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->startImageLabel->setMinimumSize(400, 250);

    this->showMaximized();

    loadAutosave();
    saveAutosave();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QLabel* MainWindow::createLogo()
{
    auto logo = new QLabel();
    QPixmap pix(":/images/alvsutloppet.png");
    pix = pix.scaledToHeight(300, Qt::SmoothTransformation);
    logo->setPixmap(pix);
    logo->setFixedSize(pix.size());
    logo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    return logo;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    QPixmap pix(":/images/start.png");

    if (!pix.isNull())
    {
        ui->startImageLabel->setPixmap(
            pix.scaled(
                ui->startImageLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );
    }
}

void MainWindow::setUpTables()
{
    auto headerTime = ui->resultTable->horizontalHeader();
    headerTime->setSectionResizeMode(0, QHeaderView::ResizeToContents); // #
    headerTime->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Time
    headerTime->setSectionResizeMode(3, QHeaderView::ResizeToContents); // Delete
    headerTime->setSectionResizeMode(2, QHeaderView::Stretch);

    auto headerRegister = ui->registerTable->horizontalHeader();
    headerRegister->setSectionResizeMode(0, QHeaderView::ResizeToContents); // #
    headerRegister->setSectionResizeMode(1, QHeaderView::Stretch); // namn
    headerRegister->setSectionResizeMode(2, QHeaderView::ResizeToContents); // kvinna
    headerRegister->setSectionResizeMode(3, QHeaderView::ResizeToContents); // man
    headerRegister->setSectionResizeMode(4, QHeaderView::ResizeToContents); // grön
    headerRegister->setSectionResizeMode(5, QHeaderView::ResizeToContents); // orange
    headerRegister->setSectionResizeMode(6, QHeaderView::ResizeToContents); // födelseår
    headerRegister->setSectionResizeMode(7, QHeaderView::ResizeToContents); // Delete

    auto headerExport = ui->exportTable->horizontalHeader();
    headerExport->setSectionResizeMode(0, QHeaderView::ResizeToContents); // #placering
    headerExport->setSectionResizeMode(1, QHeaderView::ResizeToContents); // #startnummer
    headerExport->setSectionResizeMode(2, QHeaderView::ResizeToContents); // tid
    headerExport->setSectionResizeMode(3, QHeaderView::Stretch); // namn
    headerExport->setSectionResizeMode(4, QHeaderView::ResizeToContents); // kvinna
    headerExport->setSectionResizeMode(5, QHeaderView::ResizeToContents); // man
    headerExport->setSectionResizeMode(6, QHeaderView::ResizeToContents); // grön
    headerExport->setSectionResizeMode(7, QHeaderView::ResizeToContents); // orange
    headerExport->setSectionResizeMode(8, QHeaderView::ResizeToContents); // födelseår
    headerExport->setSectionResizeMode(9, QHeaderView::ResizeToContents); // datum
}

void MainWindow::setUpComboBoxes()
{
    ui->targetGroupComboBox->addItem("Vuxna", 1);
    ui->targetGroupComboBox->addItem("Män", 2);
    ui->targetGroupComboBox->addItem("Kvinnor", 3);
    ui->targetGroupComboBox->addItem("Barn", 4);
    ui->targetGroupComboBox->addItem("Pojkar", 5);
    ui->targetGroupComboBox->addItem("Flickor", 6);
    ui->targetGroupComboBox->setCurrentIndex(0);
}

void MainWindow::connectInput()
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if(timerOn && raceStartDateTime.isValid())
        {
            qint64 ms = raceStartDateTime.msecsTo(QDateTime::currentDateTime());
            QString timeText = formatTime(ms);

            ui->currentTimeLabel->setText(timeText);

            if(displayWindow)
            {
                displayWindow->setCurrentTimeText(timeText);
            }
        }
    });

    QShortcut* spaceShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    spaceShortcut->setContext(Qt::ApplicationShortcut);
    connect(spaceShortcut, &QShortcut::activated, this, [this]() {
        if(ui->stackedWidget->currentWidget() == ui->OnGoingRace)
        {
            if(timerOn)
                on_finishButton_clicked();
            else
                on_startButton_clicked();
        }

        if(ui->stackedWidget->currentWidget() == ui->registerRunners)
        {
            auto item = ui->registerTable->currentItem();

            if(item && (item->flags() & Qt::ItemIsUserCheckable))
            {
                item->setCheckState(
                    item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked
                    );
            }

            return;
        }
    });

    QShortcut* addEnterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    QShortcut* addEnterShortcutNumpad = new QShortcut(QKeySequence(Qt::Key_Enter), this);

    auto handleEnter = [this]()
            {
                if(ui->stackedWidget->currentWidget() == ui->registerRunners)
                {
                    on_addRunnerButton_clicked();
                }
                else if (ui->stackedWidget->currentWidget() == ui->OnGoingRace)
                {
                    QWidget* focus = QApplication::focusWidget();

                    if(focus && ui->resultTable->isAncestorOf(focus))
                    {
                        ui->resultTable->setCurrentItem(nullptr);
                        ui->resultTable->clearFocus();
                        ui->finishButton->setFocus();
                    }
                }
            };

    connect(addEnterShortcut, &QShortcut::activated, this, handleEnter);
    connect(addEnterShortcutNumpad, &QShortcut::activated, this, handleEnter);

    addEnterShortcut->setContext(Qt::ApplicationShortcut);
    addEnterShortcutNumpad->setContext(Qt::ApplicationShortcut);

    connect(ui->resultTable, &QTableWidget::itemChanged,
            this,
            [this](QTableWidgetItem* item)
            {
                if(item && item->column() == 2)
                {
                    saveAutosave();
                }
                if(item && item->column() == 3)
                {
                    renumberRows();
                    updateDisplay();
                    saveAutosave();
                }
            });

    connect(ui->registerTable, &QTableWidget::itemChanged,
            this,
            [this](QTableWidgetItem* item)
            {
                if(item && item->column() == 6)
                {
                    saveAutosave();
                }
                if(item && item->column() >= 2 && item->column() <= 5)
                {
                    saveAutosave();
                }
            });
}

void MainWindow::setUpTopLayout()
{
    auto mainLayout = new QVBoxLayout(ui->centralwidget);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    auto topLayout = new QHBoxLayout();

    topLayout->addWidget(ui->registerButton);
    topLayout->addWidget(ui->timeButton);
    topLayout->addWidget(ui->randomButton);
    topLayout->addWidget(ui->newRecordButton);
    topLayout->addWidget(ui->exportButton);
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(ui->stackedWidget, 1);
}

void MainWindow::setUpTimingLayout()
{
    auto raceLayout = new QHBoxLayout(ui->OnGoingRace);
    raceLayout->setContentsMargins(8, 8, 8, 8);

    auto leftLayoutTime = new QVBoxLayout();
    leftLayoutTime->addWidget(ui->currentTimeLabel);
    ui->currentTimeLabel->setAlignment(Qt::AlignCenter);
    leftLayoutTime->addWidget(ui->startButton);
    leftLayoutTime->addWidget(ui->finishButton);
    leftLayoutTime->addWidget(ui->undoButton);
    leftLayoutTime->addStretch();
    leftLayoutTime->addWidget(ui->stopButton);
    leftLayoutTime->addWidget(ui->clearButton);

    // empty space
    leftLayoutTime->addSpacing(30);


    ui->startButton->setMinimumWidth(300);
    ui->finishButton->setMinimumWidth(300);
    ui->stopButton->setMinimumWidth(300);
    ui->clearButton->setMinimumWidth(300);
    ui->undoButton->setMinimumWidth(300);

    auto bottomLayoutTime = new QVBoxLayout();
    auto logoTime = createLogo();
    bottomLayoutTime->addWidget(logoTime);

    leftLayoutTime->addLayout(bottomLayoutTime);

    auto rightLayoutTime = new QVBoxLayout();
    rightLayoutTime->addWidget(ui->resultTable, 1);
    raceLayout->addLayout(leftLayoutTime, 0);
    raceLayout->addLayout(rightLayoutTime, 1);
}

void MainWindow::setUpRegisterLayout()
{
    auto registerLayout = new QHBoxLayout(ui->registerRunners);
    registerLayout->setContentsMargins(8, 8, 8, 8);

    auto leftLayoutRegister = new QVBoxLayout();
    leftLayoutRegister->addWidget(ui->addRunnerButton);
    leftLayoutRegister->addWidget(ui->clearRegisterTableButton);
    leftLayoutRegister->addStretch();

    ui->addRunnerButton->setMinimumWidth(300);
    ui->clearRegisterTableButton->setMinimumWidth(300);

    auto bottomLayoutRegister = new QVBoxLayout();
    auto logoRegister = createLogo();
    bottomLayoutRegister->addWidget(logoRegister);

    leftLayoutRegister->addLayout(bottomLayoutRegister);

    auto rightLayoutRegister = new QVBoxLayout();
    rightLayoutRegister->addWidget(ui->registerTable, 1);
    registerLayout->addLayout(leftLayoutRegister, 0);
    registerLayout->addLayout(rightLayoutRegister, 1);
}

void MainWindow::setUpExportLayout()
{
    auto exportLayout = new QHBoxLayout(ui->exportResults);
    exportLayout->setContentsMargins(8, 8, 8, 8);

    auto leftLayoutExport = new QVBoxLayout();
    leftLayoutExport->addWidget(ui->exportButton_2);
    leftLayoutExport->addStretch();

    ui->exportButton_2->setMinimumWidth(300);

    auto bottomLayoutExport = new QVBoxLayout();
    auto logoExport = createLogo();
    bottomLayoutExport->addWidget(logoExport);

    leftLayoutExport->addLayout(bottomLayoutExport);

    auto rightLayoutExport = new QVBoxLayout();
    rightLayoutExport->addWidget(ui->exportTable, 1);
    exportLayout->addLayout(leftLayoutExport, 0);
    exportLayout->addLayout(rightLayoutExport, 1);

    connect(ui->exportTable, &QTableWidget::itemChanged, this, &MainWindow::onExportTableItemChanged);
}

void MainWindow::setUpRandomLayout()
{
    auto randomLayout = new QHBoxLayout(ui->Randomizer);
    randomLayout->setContentsMargins(8, 8, 8, 8);

    auto leftLayoutRandom = new QVBoxLayout();
    auto leftLayoutTargetGroup = new QVBoxLayout();
    leftLayoutTargetGroup->addWidget(ui->targetGroupLabel);
    leftLayoutTargetGroup->addWidget(ui->targetGroupComboBox);

    auto leftLayoutComboBoxes = new QHBoxLayout();
    leftLayoutComboBoxes->addLayout(leftLayoutTargetGroup);

    leftLayoutRandom->addLayout(leftLayoutComboBoxes);
    leftLayoutRandom->addWidget(ui->generateButton);
    leftLayoutRandom->setContentsMargins(0, 10, 0, 0);
    leftLayoutRandom->addStretch();

    ui->generateButton->setMinimumWidth(300);

    auto bottomLayoutRandom = new QVBoxLayout();
    auto logoRandom = createLogo();
    bottomLayoutRandom->addWidget(logoRandom);

    leftLayoutRandom->addLayout(bottomLayoutRandom);

    auto rightLayoutRandom = new QVBoxLayout();

    // TOP ROW
    auto topRow = new QHBoxLayout();

    // Adults
    auto allLayout = new QVBoxLayout();
    auto allHeader = new QHBoxLayout();

    allHeader->addWidget(ui->vinnersAdultsLabel);
    allHeader->addWidget(ui->addToAdultsButton);
    allHeader->addWidget(ui->clearAdults);

    allLayout->addLayout(allHeader);
    allLayout->addWidget(ui->adultsWinnersTable);

    topRow->addLayout(allLayout);

    ui->adultsWinnersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Female
    auto femaleLayout = new QVBoxLayout();
    auto femaleHeader = new QHBoxLayout();

    femaleHeader->addWidget(ui->vinnersFemaleLabel);
    femaleHeader->addWidget(ui->addToFemaleButton);
    femaleHeader->addWidget(ui->clearFemale);

    femaleLayout->addLayout(femaleHeader);
    femaleLayout->addWidget(ui->femaleWinnersTable);

    topRow->addLayout(femaleLayout);

    ui->femaleWinnersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //Male
    auto maleLayout = new QVBoxLayout();
    auto maleHeader = new QHBoxLayout();

    maleHeader->addWidget(ui->vinnersMaleLabel);
    maleHeader->addWidget(ui->addToMaleButton);
    maleHeader->addWidget(ui->clearMale);

    maleLayout->addLayout(maleHeader);
    maleLayout->addWidget(ui->maleWinnersTable);

    topRow->addLayout(maleLayout);

    ui->maleWinnersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // BOTTOM ROW
    auto bottomRow = new QHBoxLayout();

    // Children
    auto childrenLayout = new QVBoxLayout();
    auto childrenHeader = new QHBoxLayout();

    childrenHeader->addWidget(ui->vinnersChildrenLabel);
    childrenHeader->addWidget(ui->addToChildrenButton);
    childrenHeader->addWidget(ui->clearChildren);

    childrenLayout->addLayout(childrenHeader);
    childrenLayout->addWidget(ui->childrenWinnersTable);

    bottomRow->addLayout(childrenLayout, 1);

    ui->childrenWinnersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Girls
    auto girlsLayout = new QVBoxLayout();
    auto girlsHeader = new QHBoxLayout();

    girlsHeader->addWidget(ui->vinnerGirlsLabel);
    girlsHeader->addWidget(ui->addToGirlsButton);
    girlsHeader->addWidget(ui->clearGirls);

    girlsLayout->addLayout(girlsHeader);
    girlsLayout->addWidget(ui->girlsWinnersTable);

    bottomRow->addLayout(girlsLayout, 1);

    ui->girlsWinnersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Boys
    auto boysLayout = new QVBoxLayout();
    auto boysHeader = new QHBoxLayout();

    boysHeader->addWidget(ui->vinnerBoysLabel);
    boysHeader->addWidget(ui->addToBoysButton);
    boysHeader->addWidget(ui->clearBoys);

    boysLayout->addLayout(boysHeader);
    boysLayout->addWidget(ui->boysWinnersTable);

    bottomRow->addLayout(boysLayout, 1);

    ui->boysWinnersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    rightLayoutRandom->addLayout(topRow, 1);
    rightLayoutRandom->addSpacing(40);
    rightLayoutRandom->addLayout(bottomRow, 1);
    rightLayoutRandom->addStretch();

    randomLayout->addLayout(leftLayoutRandom, 0);
    randomLayout->addLayout(rightLayoutRandom, 1);
}

void MainWindow::setUpNewRecordLayout()
{
    ui->timeInput->setInputMask("99:99.9");
    ui->timeInput->setPlaceholderText("00:00.0");
    //QRegularExpression regex(R"(^[0-9]{2}:[0-5][0-9]\.[0-9]$)");
    //auto validator = new QRegularExpressionValidator(regex, this);
    //ui->timeInput->setValidator(validator);

    auto mainNewRecordLayout = new QHBoxLayout(ui->NewRecord);
    auto highScoreLayout = new QVBoxLayout();
    auto newRecordLayout = new QVBoxLayout();
    mainNewRecordLayout->setContentsMargins(8, 8, 8, 8);

    const int fieldWidth = 300;
    const int fieldHeight = 40;

    ui->nameInput->setFixedSize(fieldWidth, fieldHeight);
    ui->groupComboBox->setFixedSize(fieldWidth, fieldHeight);
    ui->timeInput->setFixedSize(fieldWidth, fieldHeight);
    ui->tryAddButton->setMinimumWidth(fieldWidth);

    newRecordLayout->addWidget(ui->nameLabel);
    newRecordLayout->addWidget(ui->nameInput);
    newRecordLayout->addWidget(ui->groupLabel);
    newRecordLayout->addWidget(ui->groupComboBox);
    newRecordLayout->addWidget(ui->timeLabel);
    newRecordLayout->addWidget(ui->timeInput);
    newRecordLayout->addWidget(ui->tryAddButton);
    newRecordLayout->addStretch();

    displayWindow->copyHighScoreTable(displayWindow->getHighScoreTableFemale(), ui->highScoreFemale);
    displayWindow->copyHighScoreTable(displayWindow->getHighScoreTableMale(), ui->highScoreMale);
    displayWindow->copyHighScoreTable(displayWindow->getHighScoreTableGirls(), ui->highScoreGirls);
    displayWindow->copyHighScoreTable(displayWindow->getHighScoreTableBoys(), ui->highScoreBoys);

    highScoreLayout->addWidget(ui->highScoreFemale);
    highScoreLayout->addWidget(ui->highScoreMale);
    highScoreLayout->addWidget(ui->highScoreGirls);
    highScoreLayout->addWidget(ui->highScoreBoys);

    mainNewRecordLayout->addLayout(newRecordLayout);
    mainNewRecordLayout->addLayout(highScoreLayout);

    mainNewRecordLayout->addStretch();
}


