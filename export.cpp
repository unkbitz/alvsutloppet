#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QDate>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

void MainWindow::on_exportButton_2_clicked()
{
    /*
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
*/
    exportDatabaseCsv();
}

void MainWindow::exportDatabaseCsv()
{
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString defaultFileName = "database_export_" + today + ".csv";

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Exportera resultat",
        defaultFileName,
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
    //out << "Namn;Kön;Distans;Bana;Födelseår;År på Loppet;Datum;Tid\n";

    // Rows
    for(int row = 0; row < ui->exportTable->rowCount(); row++)
    {
        QString name = cellText(ui->exportTable, row, 3);
        QString birthYear = cellText(ui->exportTable, row, 8);
        QString date = cellText(ui->exportTable, row, 9);
        QString time = cellText(ui->exportTable, row, 2);

        if(name.isEmpty())
            continue;

        QString gender;

        if(isChecked(ui->exportTable, row, 4))
            gender = "Kvinna";
        else if(isChecked(ui->exportTable, row, 5))
            gender = "Man";

        QString distance;
        QString track;

        if(isChecked(ui->exportTable, row, 6))
        {
            distance = "2.1";
            track = "Grön";
        }
        else if(isChecked(ui->exportTable, row, 7))
        {
            distance = "5";
            track = "Orange";
        }
        else
        {
            distance = "5";
            track = "Blå";
        }

        QString raceYear = QDate::currentDate().toString("yyyy");

        auto writeCsvValue = [&out](QString value)
        {
            value.replace("\"", "\"\"");
            out << "\"" << value << "\"";
        };

        writeCsvValue(name); out << ";";
        writeCsvValue(gender); out << ";";
        writeCsvValue(distance); out << ";";
        writeCsvValue(track); out << ";";
        writeCsvValue(birthYear); out << ";";
        writeCsvValue(raceYear); out << ";";
        writeCsvValue(date); out << ";";
        writeCsvValue(time);
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

        addCheckBoxesToRow(ui->exportTable, row, 4, 7, false);

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
        saveAutosave();
        return;
    }

    QString startNumber = item->text().trimmed();
    if(startNumber != "")
    {
        for(int r = 0; r < ui->exportTable->rowCount(); r++)
        {
            if(r == item->row())
            {
                continue;
            }
            if(cellText(ui->exportTable, r, 1) == startNumber)
            {
                QMessageBox::warning(this, "Startnummret finns redan med!", "Ange ett unikt startnummer");
                item->setText("");
                return;
            }
        }
    }
    if(startNumber.isEmpty())
    {
        saveAutosave();
        return;
    }

    int exportRow = item->row();

    for(int r = 0; r < ui->registerTable->rowCount(); r++)
    {
        if(cellText(ui->registerTable, r, 0) == startNumber)
        {
            updatingExportTable = true;

            // Namn
            ui->exportTable->item(exportRow, 3)->setText(
                cellText(ui->registerTable, r, 1)
                );

            // Kvinna
            ui->exportTable->item(exportRow, 4)->setCheckState(
                isChecked(ui->registerTable, r, 2)
                    ? Qt::Checked
                    : Qt::Unchecked
                );

            // Man
            ui->exportTable->item(exportRow, 5)->setCheckState(
                isChecked(ui->registerTable, r, 3)
                    ? Qt::Checked
                    : Qt::Unchecked
                );

            // 2.1km
            ui->exportTable->item(exportRow, 6)->setCheckState(
                isChecked(ui->registerTable, r, 5)
                    ? Qt::Checked
                    : Qt::Unchecked
                );

            // Orange bana alltid o-checkad
            ui->exportTable->item(exportRow, 7)->setCheckState(Qt::Unchecked);

            // föselseår
            ui->exportTable->item(exportRow, 8)->setText(
                cellText(ui->registerTable, r, 4)
                );

            QString today = QDate::currentDate().toString("yyyy-MM-dd");
            ui->exportTable->item(exportRow, 9)->setText(today);

            updatingExportTable = false;
            enableAddWalkersToggle();
            saveAutosave();
            return;
        }
    }
}

void MainWindow::on_addWalkersButton_clicked()
{
    AddWalkersToExportTable();
}

void MainWindow::AddWalkersToExportTable()
{
    for (int registerRow = 0; registerRow < ui->registerTable->rowCount(); registerRow++)
    {
        bool startNumberFound = false;
        QString startNumber = cellText(ui->registerTable, registerRow, 0);

        if(startNumber.isEmpty())
        {
            continue;
        }

        for (int exportRow = 0; exportRow < ui->exportTable->rowCount(); exportRow++)
        {
            if(cellText(ui->registerTable, registerRow, 0) == cellText(ui->exportTable, exportRow, 1))
            {
                startNumberFound = true;
                break;
            }
        }
        if(!startNumberFound)
        {
            int row = ui->exportTable->rowCount();
            ui->exportTable->insertRow(row);

            ui->exportTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
            ui->exportTable->setItem(row, 1, new QTableWidgetItem(""));
            ui->exportTable->setItem(row, 2, new QTableWidgetItem(""));
            ui->exportTable->setItem(row, 3, new QTableWidgetItem(""));

            addCheckBoxesToRow(ui->exportTable, row, 4, 7, false);

            ui->exportTable->setItem(row, 8, new QTableWidgetItem(""));
            ui->exportTable->setItem(row, 9, new QTableWidgetItem(""));

            ui->exportTable->item(row, 1)->setText(startNumber);
        }

    }
}

void MainWindow::enableAddWalkersToggle()
{
    allFinishesAreAssigned = true;

    if(ui->exportTable->rowCount() == 0)
    {
        allFinishesAreAssigned = false;
    }

    for (int exportRow = 0; exportRow < ui->exportTable->rowCount(); exportRow++)
    {
        if(cellText(ui->exportTable, exportRow, 1) == "")
        {
            allFinishesAreAssigned = false;
        }
    }
    if(allFinishesAreAssigned)
    {
        ui->addWalkersButton->setEnabled(true);
    }
    else
    {
        ui->addWalkersButton->setEnabled(false);
    }
}
