#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QDate>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

void MainWindow::on_exportButton_2_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Exportera resultat",
        "alvstuloppet_resultat.csv",
        "CSV files (*.csv)"
        );

    if(fileName.isEmpty())
        return;

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // Header
    for(int col = 0; col < ui->exportTable->columnCount(); col++)
    {
        out << ui->exportTable->horizontalHeaderItem(col)->text();

        if(col < ui->exportTable->columnCount() - 1)
            out << ";";
    }

    out << "\n";

    // Rows
    for(int row = 0; row < ui->exportTable->rowCount(); row++)
    {
        for(int col = 0; col < ui->exportTable->columnCount(); col++)
        {
            QTableWidgetItem* item = ui->exportTable->item(row, col);

            QString value;

            if(item)
            {
                if(col >= 4 && col <= 7)
                {
                    value = item->checkState() == Qt::Checked ? "TRUE" : "FALSE";
                }
                else
                {
                    value = item->text();
                }
            }

            value.replace("\"", "\"\"");
            out << "\"" << value << "\"";

            if(col < ui->exportTable->columnCount() - 1)
                out << ";";
        }

        out << "\n";
    }

    file.close();

    QMessageBox::information(this, "Export klar", "CSV-filen har exporterats.");
}

void MainWindow::updateExportTable()
{
    updatingExportTable = true;

    ui->exportTable->setRowCount(0);

    for (int r = 0; r < ui->resultTable->rowCount(); r++)
    {
        if(isChecked(ui->resultTable, r, 3))
        {
            continue;
        }

        int row = ui->exportTable->rowCount();
        ui->exportTable->insertRow(row);

        ui->exportTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        ui->exportTable->setItem(row, 1, new QTableWidgetItem(""));
        ui->exportTable->setItem(row, 2, new QTableWidgetItem(cellText(ui->resultTable, r, 1)));
        ui->exportTable->setItem(row, 3, new QTableWidgetItem(cellText(ui->resultTable, r, 2)));

        addCheckBoxesToRow(ui->exportTable, row, 4, 7);

        ui->exportTable->setItem(row, 8, new QTableWidgetItem(""));
        ui->exportTable->setItem(row, 9, new QTableWidgetItem(""));
    }

    updatingExportTable = false;
}

void MainWindow::onExportTableItemChanged(QTableWidgetItem* item)
{
    if(updatingExportTable)
    {
        return;
    }

    if(item->column() != 1)
    {
        return;
    }

    QString startNumber = item->text().trimmed();

    if(startNumber.isEmpty())
    {
        return;
    }

    int exportRow = item->row();

    for(int r = 0; r < ui->registerTable->rowCount(); r++)
    {
        if(cellText(ui->registerTable, r, 0) == startNumber)
        {
            updatingExportTable = true;

            ui->exportTable->item(exportRow, 3)->setText(cellText(ui->registerTable, r, 1));
            for(int c = 4; c < 8; c++)
            {
                auto exportItem = ui->exportTable->item(exportRow, c);

                if(exportItem)
                {
                    if (isChecked(ui->registerTable, r, c-2))
                    {
                        exportItem->setCheckState(Qt::Checked);
                    }
                    else
                    {
                        exportItem->setCheckState(Qt::Unchecked);
                    }
                }
            }

            ui->exportTable->item(exportRow, 8)->setText(cellText(ui->registerTable, r, 6));
            QString today = QDate::currentDate().toString("yyyy-MM-dd");
            ui->exportTable->item(exportRow, 9)->setText(today);

            updatingExportTable = false;
            return;
        }
    }
}
