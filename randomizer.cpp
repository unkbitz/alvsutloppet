#include "mainWindow.h"
#include "ui_mainwindow.h"

#include <random>


void MainWindow::on_generateButton_clicked()
{
    QString targetGroup = ui->targetGroupComboBox->currentText();

    QStringList candidates = buildList(targetGroup);

    int amountOfVinners = ui->amountComboBox->currentText().toInt();

    QTableWidget* table = ui->adultsWinnersTable;

    if (targetGroup == "Vuxna")
    {
        //amountOfVinners = ui->adultsWinnersTable->rowCount();
        table->setVisible(true);
    }

    else if (targetGroup == "Barn")
    {
        //amountOfVinners = ui->childrenWinnersTable->rowCount();
        table = ui->childrenWinnersTable;
        table->setVisible(true);
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
        int row = table->rowCount();
        table->insertRow(row);
        generateWinners(table, winner, row);
    }
}

void MainWindow::generateWinners(QTableWidget* table, QString winner, int row)
{
    table->setItem(row, 0, new QTableWidgetItem(winner));
    table->setItem(row, 1, new QTableWidgetItem(""));
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

        QString runner = startNumber + " " + name;

        bool alreadyWinner = false;

        if(targetGroup == "Vuxna")
        {
            for (int j = 0; j < ui->adultsWinnersTable->rowCount(); j++)
            {
                if (runner == cellText(ui->adultsWinnersTable, j, 0))
                {
                    alreadyWinner = true;
                    break;
                }
            }
        }
        else if(targetGroup == "Barn")
        {
            for (int j = 0; j < ui->childrenWinnersTable->rowCount(); j++)
            {
                if (runner == cellText(ui->childrenWinnersTable, j, 0))
                {
                    alreadyWinner = true;
                    break;
                }
            }
        }
        if (alreadyWinner)
        {
            continue;
        }

        if ((targetGroup == "Vuxna" && senior) ||
            (targetGroup == "Barn" && junior))
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
