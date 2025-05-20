#include "data_structures.h"
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openStudentFile();
    void openHallFile();
    void startAllocation();

private:
    Ui::MainWindow *ui;
    QProgressBar *progressBar;
    QLabel *statusLabel;
    QElapsedTimer timer;
    QVector<Student> students;
    QVector<Hall> halls;

    // Helper methods
    void parseStudentCSV(const QString &filePath);
    void parseHallCSV(const QString &filePath);
    void allocateStudents();
    int countFileLines(const QString &filePath);
    void setupStatusBar();
    void setupButtons();
    void applyStyleSheet();
    void connectButtons();
    bool saveAllocationResults();
    void updateSummaryData();
    void setupResultsTables();
    void updateResultsTables();
    void populateResultTables();
};
#endif // MAINWINDOW_H
