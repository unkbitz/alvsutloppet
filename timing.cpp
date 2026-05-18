#include "displaywindow.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QTimer>
#include <QMessageBox>

void MainWindow::on_startButton_clicked()
{
    if(ui->resultTable->rowCount() > 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      "Töm listan",
                                      "Listan innehåller resultat, den töms vid omstart.\nÄr du säker på att du vill tömma den?",
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::No)
        {
            return;
        }
    }
    runnerCount = 0;
    ui->resultTable->setRowCount(0);
    raceStartDateTime = QDateTime::currentDateTime();
    raceTimer.start();
    timer->start(10);
    timerOn = true;

    updateButtonsEnabled();
    saveAutosave();
}

void MainWindow::on_finishButton_clicked()
{
    runnerCount++;

    qint64 elapsedMs;

    if(raceStartDateTime.isValid())
    {
        elapsedMs = raceStartDateTime.msecsTo(QDateTime::currentDateTime());
    }
    else
    {
        elapsedMs = raceTimer.elapsed();
    }

    QString time = formatTime(elapsedMs);
    int row = ui->resultTable->rowCount();
    ui->resultTable->insertRow(row);

    ui->resultTable->setItem(row, 0, new QTableWidgetItem(QString::number(runnerCount)));
    ui->resultTable->setItem(row, 1, new QTableWidgetItem(time));
    ui->resultTable->setItem(row, 2, new QTableWidgetItem(""));

    // Plats (#) – EJ skrivbar
    auto placeItem = new QTableWidgetItem(QString::number(runnerCount));
    placeItem->setFlags(Qt::ItemIsEnabled);
    ui->resultTable->setItem(row, 0, placeItem);

    // Tid – EJ skrivbar
    auto timeItem = new QTableWidgetItem(time);
    timeItem->setFlags(Qt::ItemIsEnabled);
    ui->resultTable->setItem(row, 1, timeItem);

    // Runner – skrivbar
    auto runnerItem = new QTableWidgetItem("");
    runnerItem->setFlags(runnerItem->flags() | Qt::ItemIsEditable);
    ui->resultTable->setItem(row, 2, runnerItem);

    addCheckBoxesToRow(ui->resultTable, row, 3, 3);
    addDeleteButtonToRow(ui->resultTable, row, 4);

    updateButtonsEnabled();
    updateDisplay();
    saveAutosave();
    ui->resultTable->scrollToBottom();
}

void MainWindow::on_stopButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Stoppa tidtagning",
                                  "Är du säker på att du vill stoppa tidtagningen?",
                                  QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::No)
    {
        return;
    }
    timer->stop();
    timerOn = false;

    updateButtonsEnabled();

    saveAutosave();
}

void MainWindow::on_clearButton_clicked()
{
    if(ui->resultTable->rowCount() > 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      "Töm listan",
                                      "Listan innehåller resultat.\nÄr du säker på att du vill tömma den?",
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::No)
        {
            return;
        }
    }
    ui->resultTable->setRowCount(0);
    timer->stop();
    timerOn = false;
    runnerCount = 0;
    raceStartDateTime = QDateTime();
    raceTimer.invalidate();

    ui->currentTimeLabel->setText("00:00.0");

    if(displayWindow)
    {
        displayWindow->setCurrentTimeText("00:00.0");
        displayWindow->setLatestTimeText("");
        displayWindow->setLatestFourTimeText("");
    }

    updateButtonsEnabled();
    saveAutosave();
}

void MainWindow::on_undoButton_clicked()
{
    if(undoStack.isEmpty())
    {
        return;
    }

    RemovedRow last = undoStack.back();
    undoStack.pop_back();

    ui->resultTable->insertRow(last.row);

    ui->resultTable->setItem(last.row, 0, new QTableWidgetItem(last.place));
    ui->resultTable->setItem(last.row, 1, new QTableWidgetItem(last.time));
    ui->resultTable->setItem(last.row, 2, new QTableWidgetItem(last.name));
    addCheckBoxesToRow(ui->resultTable, last.row, 3, 3);

    if(auto item = ui->resultTable->item(last.row, 3))
    {
        item->setCheckState(last.checked ? Qt::Checked : Qt::Unchecked);
    }

    addDeleteButtonToRow(ui->resultTable, last.row, 4);
    renumberRows();
    updateButtonsEnabled();
    updateDisplay();
    saveAutosave();
}

QString MainWindow::formatTime(qint64 ms)
{
    int minutes = ms / 60000;
    int seconds = (ms / 1000) % 60;
    int tenths = (ms / 100) % 10;

    return QString("%1:%2.%3")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(tenths);
}

void MainWindow::updateButtonsEnabled()
{
    if(timerOn == true)
    {
        ui->finishButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        ui->startButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
    }
    else
    {
        ui->finishButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
        ui->startButton->setEnabled(true);
        ui->clearButton->setEnabled(true);
    }

    if(undoStack.isEmpty())
    {
        ui->undoButton->setEnabled(false);
    }
    else
    {
        ui->undoButton->setEnabled(true);
    }
}

void MainWindow::renumberRows()
{
    int place = 1;
    for(int i = 0; i < ui->resultTable->rowCount(); i++)
    {
        if(!isChecked(ui->resultTable, i, 3))
        {
            ui->resultTable->item(i, 0)->setText(QString::number(place));
            place++;
        }
        else
        {
            ui->resultTable->item(i, 0)->setText("");
        }
    }

    runnerCount = ui->resultTable->rowCount();
}

void MainWindow::removeRowWithUndo(QTableWidget* table, int row)
{
    if(row < 0 || row >= table->rowCount())
    {
        return;
    }

    RemovedRow removed;
    removed.row = row;
    removed.place = table->item(row, 0)->text();
    removed.time  = table->item(row, 1)->text();
    removed.name  = table->item(row, 2)->text();
    removed.checked = isChecked(table, row, 3);

    undoStack.push_back(removed);

    table->removeRow(row);
    renumberRows();
    updateButtonsEnabled();
    updateDisplay();
    saveAutosave();
}

void MainWindow::updateDisplay()
{
    if(!displayWindow)
    {
        return;
    }

    int rowCount = ui->resultTable->rowCount();
    if(rowCount == 0)
    {
        displayWindow->setLatestTimeText("");
        displayWindow->setLatestFourTimeText("");
        return;
    }

    int startRow = ui->resultTable->rowCount()-1;
    displayWindow->setLatestTimeText("");
    int latestRow = -1;

    for(int r = startRow; r >= 0; r--)
    {
        if(!isChecked(ui->resultTable, r, 3))
        {
            latestRow = r;
            QString latestText = ui->resultTable->item(r, 1)->text();
            displayWindow->setLatestTimeText(latestText);
            break;
        }
    }

    QStringList latestFourLines;
    int fourBreak = 0;
    for (int r = latestRow - 1; r >= 0; r--)
    {
        if(isChecked(ui->resultTable, r, 3))
        {
            continue;
        }
        fourBreak++;
        latestFourLines << ui->resultTable->item(r, 1)->text();
        if(fourBreak > 3)
        {
            break;
        }
    }

    displayWindow->setLatestFourTimeText(latestFourLines.join("\n"));
}