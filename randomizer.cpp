#include "mainWindow.h"
#include "ui_mainwindow.h"

#include <random>

void MainWindow::on_clearAdults_clicked()
{
    ui->adultsWinnersTable->setRowCount(0);
    addRowInRandomTable(ui->adultsWinnersTable);
}

void MainWindow::on_clearFemale_clicked()
{
    ui->femaleWinnersTable->setRowCount(0);
    addRowInRandomTable(ui->femaleWinnersTable);
}

void MainWindow::on_clearMale_clicked()
{
    ui->maleWinnersTable->setRowCount(0);
    addRowInRandomTable(ui->maleWinnersTable);
}

void MainWindow::on_clearGirls_clicked()
{
    ui->girlsWinnersTable->setRowCount(0);
    addRowInRandomTable(ui->girlsWinnersTable);
}

void MainWindow::on_clearBoys_clicked()
{
    ui->boysWinnersTable->setRowCount(0);
    addRowInRandomTable(ui->boysWinnersTable);
}

void MainWindow::on_clearChildren_clicked()
{

    ui->childrenWinnersTable->setRowCount(0);
    addRowInRandomTable(ui->childrenWinnersTable);
}

void MainWindow::on_generateButton_clicked()
{
    QString targetGroup = ui->targetGroupComboBox->currentText();

    QStringList candidates = buildList(targetGroup);

    int amountOfVinners = 1;
    QTableWidget* table = ui->adultsWinnersTable;

    if (targetGroup == "Vuxna")
    {
        amountOfVinners = ui->adultsWinnersTable->rowCount();
    }

    else if (targetGroup == "Kvinnor")
    {
        amountOfVinners = ui->femaleWinnersTable->rowCount();
        table = ui->femaleWinnersTable;
    }

    else if (targetGroup == "Män")
    {
        amountOfVinners = ui->maleWinnersTable->rowCount();
        table = ui->maleWinnersTable;
    }

    else if (targetGroup == "Barn")
    {
        amountOfVinners = ui->childrenWinnersTable->rowCount();
        table = ui->childrenWinnersTable;
    }


    else if (targetGroup == "Flickor")
    {
        amountOfVinners = ui->girlsWinnersTable->rowCount();
        table = ui->girlsWinnersTable;
    }

    else if (targetGroup == "Pojkar")
    {
        amountOfVinners = ui->boysWinnersTable->rowCount();
        table = ui->boysWinnersTable;
    }

    for (int a = 0; a < amountOfVinners; a++)
    {
        if(candidates.isEmpty())
        {
            break;
        }
        int max = candidates.count() -1;
        int random = getRandom(0, max);
        QString winner = candidates.takeAt(random);
        generateWinners(table, winner, a);
        //if (targetGroup == "Alla")
        //{
        //    int row = ui->allWinnersTable->rowCount();
        //    ui->allWinnersTable->insertRow(row);
        //    ui->allWinnersTable->setItem(row, 0, new QTableWidgetItem(winner));
        //}
    }
}

void MainWindow::generateWinners(QTableWidget* table, QString winner, int row)
{
    table->setItem(row, 0, new QTableWidgetItem(winner));
}

QStringList MainWindow::buildList(QString targetGroup)
{
    QStringList candidates;

    for (int i = 0; i <  ui->registerTable->rowCount(); i++)
    {
        QString startNumber = cellText(ui->registerTable, i, 0);
        QString name = cellText(ui->registerTable, i, 1);
        QString yearText = cellText(ui->registerTable, i, 6);

        if(startNumber.isEmpty())
        {
            continue;
        }

        bool yearIsNumber = false;
        int year = yearText.toInt(&yearIsNumber);

        bool senior = false;
        bool junior = false;

        if(yearIsNumber)
        {
            senior = year < ageLimit;
            junior = year >= ageLimit;
        }

        bool female = isChecked(ui->registerTable, i, 2);
        bool male = isChecked(ui->registerTable, i, 3);

        QString runner = startNumber + " " + name;

        if ((targetGroup == "Vuxna" && senior) ||
            (targetGroup == "Kvinnor" && female && senior) ||
            (targetGroup == "Män" && male && senior) ||
            (targetGroup == "Barn" && junior) ||
            (targetGroup == "Flickor" && female && junior) ||
            (targetGroup == "Pojkar" && male && junior))
        {
            candidates.append(runner);
        }
    }

    return candidates;
}

int MainWindow::getRandom(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min, max);

    return dist(gen);
}

void MainWindow::addRowInRandomTable(QTableWidget* table)
{
    int row = table->rowCount();
    table->insertRow(row);

    table->setItem(row, 0, new QTableWidgetItem(""));
    table->setItem(row, 1, new QTableWidgetItem(""));

    table->setCurrentCell(row, 1);
    table->editItem(table->item(row, 1));
    saveAutosave();
}

void MainWindow::on_addToAdultsButton_clicked()
{
    addRowInRandomTable(ui->adultsWinnersTable);
}

void MainWindow::on_addToFemaleButton_clicked()
{
    addRowInRandomTable(ui->femaleWinnersTable);
}

void MainWindow::on_addToMaleButton_clicked()
{
    addRowInRandomTable(ui->maleWinnersTable);
}

void MainWindow::on_addToGirlsButton_clicked()
{
    addRowInRandomTable(ui->girlsWinnersTable);
}

void MainWindow::on_addToBoysButton_clicked()
{
    addRowInRandomTable(ui->boysWinnersTable);
}

void MainWindow::on_addToChildrenButton_clicked()
{
    addRowInRandomTable(ui->childrenWinnersTable);
}
