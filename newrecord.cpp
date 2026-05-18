#include "displaywindow.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QMessageBox>

#include <QDate>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <algorithm>

struct HighScoreEntry
{
    QString name;
    QString time;
    QString year;
};

void MainWindow::on_tryAddButton_clicked()
{
    QString time = ui->timeInput->text();

    QRegularExpression regex(R"(^\d{2}:[0-5]\d\.\d$)");

    if(!regex.match(time).hasMatch())
    {
        QMessageBox::warning(this, "Fel tid", "sekunderna måste vara mellan 0-59");
        return;
    }

    QString name = ui->nameInput->text().trimmed();

    if(name.isEmpty())
    {
        QMessageBox::warning(this, "Namn saknas", "Skriv ett namn.");
        return;
    }

    QString group = ui->groupComboBox->currentText();

    QString text = "Stämmer uppgifterna?\nNamn: " + name + "\nGrupp: " + group +"\nTid: " + time;
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Kontroll",
                                  text,
                                  QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::No)
    {
        return;
    }

    if(!displayWindow)
    {
        return;
    }

    QTableWidget* table = nullptr;
    bool highScore = false;

    if(group == "Kvinna")
    {
        table = displayWindow->getHighScoreTableFemale();
    }

    else if(group == "Man")
    {
        table = displayWindow->getHighScoreTableMale();
    }

    else if(group == "Flicka")
    {
        table = displayWindow->getHighScoreTableGirls();
    }

    else if(group == "Pojke")
    {
        table = displayWindow->getHighScoreTableBoys();
    }

    if(!table)
    {
        return;
    }

    highScore = desideNewRecord(table, time);

    if(!highScore)
    {
        QMessageBox::warning(this, "Inget rekord", "Tyvärr kvalar tiden inte in på topplistan");
        return;
    }

    insertHighScore(table, name, time);
    QMessageBox::information(this, "Nytt rekord!", "Tiden kvalar in på topplistan!");
}

int MainWindow::timeToTenths(const QString& timeText)
{
    QString trimmed = timeText.trimmed();

    QStringList parts = trimmed.split(":");
    if(parts.size() != 2)
        return -1;

    bool minutesOk = false;
    int minutes = parts[0].toInt(&minutesOk);

    if(!minutesOk)
        return -1;

    int seconds = 0;
    int tenths = 0;

    if(parts[1].contains("."))
    {
        QStringList secParts = parts[1].split(".");
        if(secParts.size() != 2)
            return -1;

        bool secondsOk = false;
        bool tenthsOk = false;

        seconds = secParts[0].toInt(&secondsOk);
        tenths = secParts[1].toInt(&tenthsOk);

        if(!secondsOk || !tenthsOk)
            return -1;
    }
    else
    {
        bool secondsOk = false;
        seconds = parts[1].toInt(&secondsOk);

        if(!secondsOk)
            return -1;

        tenths = 0;
    }

    return minutes * 600 + seconds * 10 + tenths;
}

bool MainWindow::desideNewRecord(QTableWidget* table, const QString& newTime)
{
    int newValue = timeToTenths(newTime);
    if(newValue < 0)
        return false;

    for(int row = 0; row < table->rowCount(); row++)
    {
        QString oldTime = cellText(table, row, 1); // kolumn 1 = Tid
        int oldValue = timeToTenths(oldTime);

        if(oldValue < 0 || newValue < oldValue)
        {
            return true;
        }
    }
    return false;
}

void MainWindow::insertHighScore(QTableWidget* table, const QString& name, const QString& time)
{
    QList<HighScoreEntry> entries;

    // Läs befintliga rader
    for(int row = 0; row < table->rowCount(); row++)
    {
        QString oldName = cellText(table, row, 0);
        QString oldTime = cellText(table, row, 1);
        QString oldYear = cellText(table, row, 2);

        if(oldName.isEmpty() || oldTime.isEmpty())
            continue;

        entries.append({ oldName, oldTime, oldYear });
    }

    // Lägg till nya rekordet
    QString currentYear = QString::number(QDate::currentDate().year());
    entries.append({ name, time, currentYear });

    // Sortera snabbast först
    std::sort(entries.begin(), entries.end(), [this](const HighScoreEntry& a, const HighScoreEntry& b)
              {
                  return timeToTenths(a.time) < timeToTenths(b.time);
              });

    // Behåll bara 5 bästa
    while(entries.size() > 5)
        entries.removeLast();

    // Skriv tillbaka
    table->setRowCount(5);

    for(int row = 0; row < 5; row++)
    {
        if(row < entries.size())
        {
            setTableText(table, row, 0, entries[row].name);
            setTableText(table, row, 1, entries[row].time);
            setTableText(table, row, 2, entries[row].year);
        }
        else
        {
            setTableText(table, row, 0, "");
            setTableText(table, row, 1, "");
            setTableText(table, row, 2, "");
        }
    }

    ui->nameInput->clear();
    ui->timeInput->clear();

    displayWindow->copyHighScoreTable(table, ui->highScoreFemale);
}

void MainWindow::setTableText(QTableWidget* table, int row, int col, const QString& text)
{
    auto item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    item->setFlags(Qt::ItemIsEnabled);

    table->setItem(row, col, item);
}