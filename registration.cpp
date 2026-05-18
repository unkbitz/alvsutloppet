#include "mainWindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QSize>

void MainWindow::on_addRunnerButton_clicked()
{
    int row = ui->registerTable->rowCount();
    ui->registerTable->insertRow(row);

    ui->registerTable->setItem(row, 0, new QTableWidgetItem(""));
    ui->registerTable->setItem(row, 1, new QTableWidgetItem(""));
    ui->registerTable->setItem(row, 6, new QTableWidgetItem(""));

    addCheckBoxesToRow(ui->registerTable, row, 2, 5);
    addDeleteButtonToRow(ui->registerTable, row, 7);

    ui->registerTable->setCurrentCell(row, 0);
    ui->registerTable->editItem(ui->registerTable->item(row, 0));
    saveAutosave();
}

void MainWindow::on_clearRegisterTableButton_clicked()
{
    if(ui->registerTable->rowCount() > 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      "Empty list",
                                      "Listan innehåller löpare.\nÄr du säker på att du vill tömma den?",
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::No)
        {
            return;
        }
    }
    ui->registerTable->setRowCount(0);
    saveAutosave();
}

void MainWindow::addDeleteButtonToRow(QTableWidget* table, int row, int column)
{
    QPushButton* deleteButton = new QPushButton("X");
    deleteButton->setStyleSheet("QPushButton { color: red; font-weight: bold; }");
    deleteButton->setFocusPolicy(Qt::NoFocus);
    auto deleteItem = new QTableWidgetItem("");
    deleteItem->setFlags(Qt::ItemIsEnabled);
    table->setItem(row, column, deleteItem);
    table->setCellWidget(row, column, deleteButton);

    connect(deleteButton, &QPushButton::clicked, this, [this, table, deleteButton, column]() {
        for(int r = 0; r < table->rowCount(); r++)
        {
            if(table->cellWidget(r, column) == deleteButton)
            {
                if(table == ui->resultTable)
                    removeRowWithUndo(table, r);
                else
                    table->removeRow(r);
                break;
            }
        }
    });

    saveAutosave();
}

void MainWindow::addCheckBoxesToRow(QTableWidget* table, int row, int startColumn, int endColumn)
{
    for(int col = startColumn; col <= endColumn; col++)
    {
        QTableWidgetItem* checkbox = new QTableWidgetItem();
        checkbox->setFlags(
            Qt::ItemIsUserCheckable |
            Qt::ItemIsEnabled |
            Qt::ItemIsSelectable
            );
        checkbox->setCheckState(Qt::Unchecked);
        table->setItem(row, col, checkbox);
    }
}

QString MainWindow::cellText(QTableWidget* table, int row, int col)
{
    auto item = table->item(row, col);
    return item ? item->text().trimmed() : "";
}

bool MainWindow::isChecked(QTableWidget* table, int row, int col)
{
    auto item = table->item(row, col);
    return item && item->checkState() == Qt::Checked;
}