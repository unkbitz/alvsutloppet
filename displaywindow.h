#ifndef DISPLAYWINDOW_H
#define DISPLAYWINDOW_H
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QBoxLayout>

class QWidget;

class DisplayWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayWindow(QWidget *parent = nullptr);

    void setCurrentTimeText(const QString& text);
    void setLatestTimeText(const QString& text);
    void setLatestFourTimeText(const QString& text);
    bool raceFinished = false;

    void copyHighScoreTable(QTableWidget* source, QTableWidget* target);
    QTableWidget* getHighScoreTableFemale() const;
    QTableWidget* getHighScoreTableMale() const;
    QTableWidget* getHighScoreTableGirls() const;
    QTableWidget* getHighScoreTableBoys() const;

private:
    QLabel* currentTime;
    QLabel* text;
    QLabel* toppListor;
    QLabel* latest;
    QLabel* latestFour;

    QTableWidget* exportDisplayTable;
    QTableWidget* highScoreTableFemale;
    QTableWidget* highScoreTableMale;
    QTableWidget* highScoreTableGirls;
    QTableWidget* highScoreTableBoys;

    QLabel* createDisplayLogo();
    void setUpHighScoreTables();
    void loadHighScoresFromCsv();
    void fillHighScoreTable(QTableWidget* table, const QStringList& cols, int row, int startCol);
    void setTableText(QTableWidget* table, int row, int col, const QString& text);
    void fitTableHeightToRows(QTableWidget* table);
};

#endif // DISPLAYWINDOW_H
