#include "displaywindow.h"

#include <QFont>
#include <qheaderview.h>

#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QStackedLayout>

DisplayWindow::DisplayWindow(QWidget *parent) : QWidget(parent)
{

    setWindowTitle("Publik display");
    auto mainLayout = new QVBoxLayout(this);

    auto timeLayout = new QVBoxLayout();

    currentTime = new QLabel("00:00.0");
    currentTime->setAlignment(Qt::AlignCenter);
    QFont fontCurrentTime = currentTime->font();
    fontCurrentTime.setPointSize(45);
    fontCurrentTime.setBold(true);
    currentTime->setFont(fontCurrentTime);

    text = new QLabel("Senaste målgångarna:");
    text->setAlignment(Qt::AlignCenter);
    QFont fontText = text->font();
    fontText.setPointSize(20);
    fontText.setBold(true);
    text->setFont(fontText);

    latest = new QLabel("");
    latest->setAlignment(Qt::AlignCenter);
    QFont fontLatest = latest->font();
    fontLatest.setPointSize(200);
    fontLatest.setBold(true);
    latest->setFont(fontLatest);

    latestFour = new QLabel("");
    latestFour->setAlignment(Qt::AlignCenter);
    QFont fontLatestFour = latestFour->font();
    fontLatestFour.setPointSize(30);
    fontLatestFour.setBold(true);
    latestFour->setFont(fontLatestFour);/*
    latestFour->setStyleSheet("background-color: rgba(255, 0, 0, 80); color: white;");
    latest->setStyleSheet("background-color: rgba(0, 255, 0, 80); color: white;");*/
    // timeLayout->addSpacing(40);
    timeLayout->addWidget(currentTime);
    // timeLayout->addSpacing(80);
    timeLayout->addWidget(text);
    timeLayout->addWidget(latest);
    timeLayout->addWidget(latestFour);
    //timeLayout->addStretch();

    auto highScoreLayout = new QHBoxLayout();
    auto highScoreLineLayout = new QHBoxLayout();
    auto highScoreTablesLayout = new QVBoxLayout();
    auto highScoreLayoutLeft = new QVBoxLayout();
    auto highScoreLayoutMiddle = new QHBoxLayout();
    //auto highScoreLayoutRight = new QVBoxLayout();
    setUpHighScoreTables();
    loadHighScoresFromCsv();

    toppListor = new QLabel("Topplistor:");
    toppListor->setAlignment(Qt::AlignLeft);
    QFont fontToppListor = toppListor->font();
    fontToppListor.setPointSize(40);
    fontToppListor.setBold(true);
    toppListor->setFont(fontToppListor);

    auto logoDisplay = createDisplayLogo();

    highScoreLineLayout->addWidget(toppListor);
    //highScoreLayoutLeft->addStretch();
    highScoreLayoutLeft->addWidget(logoDisplay, 0, Qt::AlignLeft | Qt::AlignBottom);

    highScoreLayoutMiddle->addWidget(highScoreTableFemale);
    highScoreLayoutMiddle->addWidget(highScoreTableMale);
    //highScoreLayoutMiddle->addStretch();

    highScoreLayoutMiddle->addWidget(highScoreTableGirls);
    highScoreLayoutMiddle->addWidget(highScoreTableBoys);
    //highScoreLayoutMiddle->addStretch();

    highScoreLayout->addLayout(highScoreLayoutLeft, 1);
    highScoreTablesLayout->setContentsMargins(0, 0, 0, 0);
    highScoreTablesLayout->setSpacing(5);

    highScoreLineLayout->setContentsMargins(0, 0, 0, 0);
    highScoreLineLayout->setSpacing(0);

    highScoreLayoutMiddle->setContentsMargins(0, 0, 0, 0);
    highScoreLayoutMiddle->setSpacing(8);

    highScoreTablesLayout->addLayout(highScoreLineLayout, 0);
    highScoreTablesLayout->addLayout(highScoreLayoutMiddle, 0);
    highScoreTablesLayout->addStretch(1);
    highScoreLayout->addLayout(highScoreTablesLayout, 6);
    //highScoreLayout->addLayout(highScoreLayoutRight, 2);

    mainLayout->addLayout(timeLayout);
    mainLayout->addStretch(1);
    mainLayout->addLayout(highScoreLayout, 0);
    //mainLayout->addLayout(spacingLayout, 1);

    //mainLayout->addLayout(logoLayout, 2);
    //mainLayout->addLayout(layout, 6);
    //mainLayout->addLayout(highScoreLayout, 4);
    //mainLayout->addLayout(spacingLayout, 1);
}

void DisplayWindow::setCurrentTimeText(const QString& text)
{
    currentTime->setText(text);
}

void DisplayWindow::setLatestTimeText(const QString& text)
{
    latest->setText(text);
}

void DisplayWindow::setLatestFourTimeText(const QString& text)
{
    latestFour->setText(text);
}


void DisplayWindow::setUpHighScoreTables()
{
    highScoreTableFemale = new QTableWidget(this);
    highScoreTableMale = new QTableWidget(this);
    highScoreTableGirls = new QTableWidget(this);
    highScoreTableBoys = new QTableWidget(this);

    highScoreTableFemale->setColumnCount(3);
    highScoreTableFemale->setRowCount(5);
    QStringList headerFemale;
    headerFemale.append("Damer");
    headerFemale.append("Tid");
    headerFemale.append("År");
    highScoreTableFemale->setHorizontalHeaderLabels(headerFemale);
    highScoreTableFemale->verticalHeader()->setVisible(false);
    highScoreTableFemale->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    highScoreTableFemale->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    highScoreTableFemale->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft);

    highScoreTableMale->setColumnCount(3);
    highScoreTableMale->setRowCount(5);
    QStringList headerMale;
    headerMale.append("Herrar");
    headerMale.append("Tid");
    headerMale.append("År");
    highScoreTableMale->setHorizontalHeaderLabels(headerMale);
    highScoreTableMale->verticalHeader()->setVisible(false);
    highScoreTableMale->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    highScoreTableMale->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    highScoreTableMale->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft);

    highScoreTableGirls->setColumnCount(3);
    highScoreTableGirls->setRowCount(5);
    QStringList headerGirls;
    headerGirls.append("Flickor");
    headerGirls.append("Tid");
    headerGirls.append("År");
    highScoreTableGirls->setHorizontalHeaderLabels(headerGirls);
    highScoreTableGirls->verticalHeader()->setVisible(false);
    highScoreTableGirls->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    highScoreTableGirls->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    highScoreTableGirls->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft);

    highScoreTableBoys->setColumnCount(3);
    highScoreTableBoys->setRowCount(5);
    QStringList headerBoys;
    headerBoys.append("Pojkar");
    headerBoys.append("Tid");
    headerBoys.append("År");
    highScoreTableBoys->setHorizontalHeaderLabels(headerBoys);
    highScoreTableBoys->verticalHeader()->setVisible(false);
    highScoreTableBoys->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    highScoreTableBoys->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    highScoreTableBoys->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft);

    highScoreTableFemale->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    highScoreTableFemale->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    highScoreTableFemale->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    highScoreTableMale->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    highScoreTableMale->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    highScoreTableMale->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    highScoreTableGirls->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    highScoreTableGirls->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    highScoreTableGirls->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    highScoreTableBoys->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    highScoreTableBoys->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    highScoreTableBoys->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    QFont font = highScoreTableFemale->font();
    font.setPointSize(15);
    highScoreTableFemale->setFont(font);
    highScoreTableFemale->horizontalHeader()->setFont(font);
    highScoreTableMale->setFont(font);
    highScoreTableMale->horizontalHeader()->setFont(font);
    highScoreTableGirls->setFont(font);
    highScoreTableGirls->horizontalHeader()->setFont(font);
    highScoreTableBoys->setFont(font);
    highScoreTableBoys->horizontalHeader()->setFont(font);

    highScoreTableFemale->setStyleSheet(
        "QTableWidget::item { padding-left: 8px; padding-right: 8px; }"
        "QHeaderView::section { padding-left: 8px; padding-right: 8px; }"
        );
    highScoreTableMale->setStyleSheet(
        "QTableWidget::item { padding-left: 8px; padding-right: 8px; }"
        "QHeaderView::section { padding-left: 8px; padding-right: 8px; }"
        );
    highScoreTableGirls->setStyleSheet(
        "QTableWidget::item { padding-left: 8px; padding-right: 8px; }"
        "QHeaderView::section { padding-left: 8px; padding-right: 8px; }"
        );
    highScoreTableBoys->setStyleSheet(
        "QTableWidget::item { padding-left: 8px; padding-right: 8px; }"
        "QHeaderView::section { padding-left: 8px; padding-right: 8px; }"
        );

    fitTableHeightToRows(highScoreTableFemale);
    fitTableHeightToRows(highScoreTableMale);
    fitTableHeightToRows(highScoreTableGirls);
    fitTableHeightToRows(highScoreTableBoys);
}

void DisplayWindow::fitTableHeightToRows(QTableWidget* table)
{
    table->resizeRowsToContents();

    int height = table->horizontalHeader()->height();

    for(int r = 0; r < table->rowCount(); r++)
        height += table->rowHeight(r);

    height += table->frameWidth() * 2 + 4;

    table->setFixedHeight(height);
}

void DisplayWindow::loadHighScoresFromCsv()
{
    QString path = QCoreApplication::applicationDirPath() + "/highScore.csv";

    QFile file(path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);

    int row = 0;

    while(!in.atEnd() && row < 5)
    {
        QString line = in.readLine();

        QStringList cols = line.split(";");

        // Om CSV-filen använder komma istället för semikolon
        if(cols.count() < 4)
            cols = line.split(",");

        fillHighScoreTable(highScoreTableFemale, cols, row, 0);   // A-D
        fillHighScoreTable(highScoreTableMale,   cols, row, 5);   // F-I
        fillHighScoreTable(highScoreTableGirls,  cols, row, 10);  // K-N
        fillHighScoreTable(highScoreTableBoys,   cols, row, 15);  // P-S

        row++;
    }

    file.close();
}

void DisplayWindow::fillHighScoreTable(QTableWidget* table, const QStringList& cols, int row, int startCol)
{
    QString name;
    QString time;
    QString year;

    if(startCol < cols.count())
        name = cols[startCol].trimmed();

    if(startCol + 2 < cols.count())
        time = cols[startCol + 2].trimmed();

    if(startCol + 3 < cols.count())
        year = cols[startCol + 3].trimmed();

    setTableText(table, row, 0, name);
    setTableText(table, row, 1, time);
    setTableText(table, row, 2, year);
}

void DisplayWindow::setTableText(QTableWidget* table, int row, int col, const QString& text)
{
    auto item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignLeft);
    item->setFlags(Qt::ItemIsEnabled);

    table->setItem(row, col, item);
}

QLabel* DisplayWindow::createDisplayLogo()
{
    auto logo = new QLabel();
    QPixmap pix(":/images/alvsutloppet.png");
    pix = pix.scaledToHeight(300, Qt::SmoothTransformation);
    logo->setPixmap(pix);
    logo->setFixedSize(pix.size());
    logo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    return logo;
}


void DisplayWindow::copyHighScoreTable(QTableWidget* source, QTableWidget* target)
{
    if(!source || !target)
        return;

    target->setColumnCount(source->columnCount());
    target->setRowCount(source->rowCount());

    QStringList headers;

    for(int c = 0; c < source->columnCount(); c++)
    {
        auto header = source->horizontalHeaderItem(c);
        headers << (header ? header->text() : "");
    }

    target->setHorizontalHeaderLabels(headers);
    target->verticalHeader()->setVisible(false);

    for(int r = 0; r < source->rowCount(); r++)
    {
        for(int c = 0; c < source->columnCount(); c++)
        {
            QString value = source->item(r, c)->text().trimmed();
            auto item = new QTableWidgetItem(value);
            item->setFlags(Qt::ItemIsEnabled);
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            target->setItem(r, c, item);
        }
    }

    target->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    target->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    target->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
}

QTableWidget* DisplayWindow::getHighScoreTableFemale() const
{
    return highScoreTableFemale;
}

QTableWidget* DisplayWindow::getHighScoreTableMale() const
{
    return highScoreTableMale;
}

QTableWidget* DisplayWindow::getHighScoreTableGirls() const
{
    return highScoreTableGirls;
}

QTableWidget* DisplayWindow::getHighScoreTableBoys() const
{
    return highScoreTableBoys;
}