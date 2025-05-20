#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QScreen>
#include <QGuiApplication>
#include <QProgressBar>
#include <QLabel>
#include <QElapsedTimer>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QSet>
#include "data_structures.h"

void MainWindow::applyStyleSheet() {
    QString style = R"(
        /* Main Window */
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #f5f7fa, stop:1 #e4e8eb);
            font-family: 'Segoe UI', sans-serif;
        }

        /* Buttons */
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #4CAF50, stop:1 #45a049);
            color: white;
            border: none;
            border-radius: 6px;
            padding: 12px 24px;
            font-size: 14px;
            font-weight: 500;
            min-width: 120px;
            margin: 5px;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #5CBF60, stop:1 #55b059);
        }

        QPushButton:pressed {
            background: #3d8b40;
            padding-top: 13px;
            padding-bottom: 11px;
        }

        /* Labels */
        QLabel {
            font-size: 14px;
            color: #34495e;
            qproperty-alignment: AlignCenter;
        }

        QLabel[objectName^="label_"] { /* Style specific labels */
            font-weight: bold;
            color: #2c3e50;
            font-size: 15px;
        }

        /* Progress Bar */
        QProgressBar {
            border: 1px solid #dcdde1;
            border-radius: 4px;
            background: white;
            height: 18px;
            text-align: center;
        }

        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3498db, stop:1 #2980b9);
            border-radius: 3px;
        }

        /* Status Bar */
        QStatusBar {
            background: #ecf0f1;
            border-top: 1px solid #bdc3c7;
            color: #7f8c8d;
            font-size: 12px;
        }

        /* Menu Bar */
        QMenuBar {
            background: transparent;
            padding: 5px;
        }

        QMenuBar::item {
            padding: 8px 15px;
            background: transparent;
            border-radius: 4px;
            color: #2c3e50;
        }

        QMenuBar::item:selected {
            background: rgba(52, 152, 219, 0.2);
        }

        /* Table Widget (if used) */
        QTableWidget {
            background: white;
            gridline-color: #dcdde1;
            border: 1px solid #dcdde1;
        }

        QHeaderView::section {
            background: #3498db;
            color: white;
            padding: 6px;
            border: none;
        }
    )";

    this->setStyleSheet(style);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window setup
    setWindowTitle("Exam Hall Allocation System");
    resize(800, 600);

    // Initialize UI components
    setupStatusBar();
    setupButtons();
    applyStyleSheet();

    // Connect signals
    connectButtons();
}

void MainWindow::setupResultsTables()
{
    // Setup student table model
    QStandardItemModel *studentModel = new QStandardItemModel(this);
    studentModel->setHorizontalHeaderLabels({"Roll No", "Name", "Branch", "Hall", "Table", "Side"});
    ui->tableView_students->setModel(studentModel);
    ui->tableView_students->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Setup hall table model
    QStandardItemModel *hallModel = new QStandardItemModel(this);
    hallModel->setHorizontalHeaderLabels({"Hall Name", "Capacity", "Occupied", "Available"});
    ui->tableView_halls->setModel(hallModel);
    ui->tableView_halls->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::setupStatusBar()
{
    // Progress bar
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setTextVisible(false);
    progressBar->setFixedHeight(15);
    progressBar->setValue(0);
    ui->statusbar->addPermanentWidget(progressBar, 1);

    // Status label
    statusLabel = new QLabel("Ready", this);
    ui->statusbar->addPermanentWidget(statusLabel);
}

void MainWindow::setupButtons()
{
    ui->pushButton->setText("Browse");
    ui->pushButton_2->setText("Browse");
    ui->pushButton_3->setText("Start Allocation");
}

void MainWindow::connectButtons()
{
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::openStudentFile);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::openHallFile);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::startAllocation);
}

int MainWindow::countFileLines(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return 0;

    int lines = 0;
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        stream.readLine();
        lines++;
    }
    file.close();
    return lines;
}

void MainWindow::parseStudentCSV(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not open student file: " + file.errorString());
        return;
    }

    students.clear();
    QTextStream in(&file);
    int lineCount = 0;
    bool hasHeader = false;

    // Show loading progress
    progressBar->setRange(0, 0); // Indeterminate mode
    statusLabel->setText("Loading student data...");
    qApp->processEvents();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineCount++;

        // Skip empty lines and detect header
        if (line.isEmpty()) continue;
        if (lineCount == 1) {
            if (line.contains("name", Qt::CaseInsensitive) ||
                line.contains("roll", Qt::CaseInsensitive)) {
                hasHeader = true;
                continue;
            }
        }

        QStringList fields = line.split(",");

        // Basic validation
        if (fields.size() < 3) {
            qDebug() << "Skipping invalid line:" << line;
            continue;
        }

        Student s;
        s.name = fields[0].trimmed();
        s.rollNo = fields[1].trimmed();
        s.branch = fields[2].trimmed().toUpper(); // Normalize branch names

        students.append(s);

        // Update progress every 1000 records
        if (students.size() % 1000 == 0) {
            statusLabel->setText(QString("Loaded %1 students...").arg(students.size()));
            qApp->processEvents();
        }
    }

    file.close();
    progressBar->setRange(0, 100);
    progressBar->setValue(100);
    statusLabel->setText(QString("Loaded %1 students").arg(students.size()));
}

void MainWindow::openStudentFile() {
    QString file = QFileDialog::getOpenFileName(this, "Select Student Data File");
    if (file.isEmpty()) return;

    students.clear();
    parseStudentCSV(file);
    statusLabel->setText(QString("Loaded %1 students").arg(students.size()));

    // Update the line edit with the file path
    ui->lineEdit_studentFile->setText(QFileInfo(file).fileName());

    // Update the summary data
    updateSummaryData();
}

void MainWindow::parseHallCSV(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QTextStream in(&file);
    int lineCount = 0;
    bool hasHeader = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineCount++;

        // Skip empty lines and detect header
        if (line.isEmpty()) continue;
        if (lineCount == 1) {
            if (line.contains("name", Qt::CaseInsensitive) ||
                line.contains("roll", Qt::CaseInsensitive)) {
                hasHeader = true;
                continue;
            }
        }

        QStringList fields = line.split(",");

        if (fields.size() >= 2) {  // name, capacity
            Hall h;
            h.name = fields[0].trimmed();
            h.capacity = fields[1].toInt();
            halls.append(h);
        }
    }
    file.close();
}

void MainWindow::openHallFile() {
    QString file = QFileDialog::getOpenFileName(this, "Select Hall Data File");
    if (file.isEmpty()) return;

    halls.clear();
    parseHallCSV(file);
    statusLabel->setText(QString("Loaded %1 halls").arg(halls.size()));

    // Update the line edit with the file path
    ui->lineEdit_hallFile->setText(QFileInfo(file).fileName());

    // Update the summary data
    updateSummaryData();
}

void MainWindow::updateSummaryData() {
    qDebug() << "Updating summary data";
    qDebug() << "Students:" << students.size();
    qDebug() << "Halls:" << halls.size();

    // Update student count
    ui->label_studentsCountValue->setText(QString::number(students.size()));
    qDebug() << "Student count label set to:" << ui->label_studentsCountValue->text();

    // Update hall count
    ui->label_hallsCountValue->setText(QString::number(halls.size()));
    qDebug() << "Hall count label set to:" << ui->label_hallsCountValue->text();

    // Calculate and update unique branches
    QSet<QString> branches;
    for (const auto &s : students) {
        branches.insert(s.branch);
    }
    ui->label_branchesValue->setText(QString::number(branches.size()));
    qDebug() << "Branch count label set to:" << ui->label_branchesValue->text();

    // Calculate and update total capacity
    int totalCapacity = 0;
    for (const auto &h : halls) {
        totalCapacity += h.capacity;
    }
    ui->label_totalCapacityValue->setText(QString::number(totalCapacity));
    qDebug() << "Capacity label set to:" << ui->label_totalCapacityValue->text();
}

void MainWindow::populateResultTables() {
    QStandardItemModel *studentModel = qobject_cast<QStandardItemModel *>(ui->tableView_students->model());
    QStandardItemModel *hallModel = qobject_cast<QStandardItemModel *>(ui->tableView_halls->model());

    studentModel->removeRows(0, studentModel->rowCount());
    hallModel->removeRows(0, hallModel->rowCount());

    for (const auto &s : students) {
        QList<QStandardItem *> row;
        row << new QStandardItem(s.rollNo)
            << new QStandardItem(s.name)
            << new QStandardItem(s.branch)
            << new QStandardItem(s.hall)
            << new QStandardItem(s.table > 0 ? QString::number(s.table) : "UNALLOCATED")
            << new QStandardItem(s.side == 1 ? "Left" : (s.side == 2 ? "Right" : "NA"));
        studentModel->appendRow(row);
    }

    for (const auto &h : halls) {
        QList<QStandardItem *> row;
        row << new QStandardItem(h.name)
            << new QStandardItem(QString::number(h.capacity))
            << new QStandardItem(QString::number(h.occupied))
            << new QStandardItem(QString::number(h.capacity - h.occupied));
        hallModel->appendRow(row);
    }
}


void MainWindow::startAllocation() {
    if (students.empty() || halls.empty()) {
        QMessageBox::warning(this, "Error", "Load student and hall data first!");
        return;
    }

    // Reset previous allocation
    for (auto &s : students) {
        s.hall.clear();
        s.table = -1;
        s.side = -1;
    }
    for (auto &h : halls) {
        h.occupied = 0;
    }

    progressBar->setRange(0, students.size());
    statusLabel->setText("Starting allocation...");
    qApp->processEvents();

    // creates a table to track each seat (hall , (table , (side , branch)))
    QHash<QString, QHash<int, QHash<int, QString>>> tableBranches;

    int allocatedCount = 0;
    int unallocatedCount = 0;

    for (int i = 0; i < students.size(); ++i) {
        Student &student = students[i];
        bool allocated = false;

        // Iterate over each hall
        for (auto &hall : halls) {
            if (hall.occupied >= hall.capacity) continue;

            // iterate over each table in the hall
            for (int table = 1; table <= hall.tables() && !allocated; ++table) {
                // first it will check on the left side , then on the right side
                for (int side = 1; side <= 2 && !allocated; ++side) {
                   //check if is already filled
                    if (tableBranches[hall.name][table].contains(side)) continue;

                    //checks if it have same branch
                    int otherSide = (side == 1) ? 2 : 1;
                    QString otherBranch = tableBranches[hall.name][table].value(otherSide);
                    //skips the same branch
                    if (!otherBranch.isEmpty() && otherBranch == student.branch) {
                        continue;
                    }

                    // Found a valid spot - allocate it
                    student.hall = hall.name;
                    student.table = table;
                    student.side = side;
                    hall.occupied++;
                    tableBranches[hall.name][table][side] = student.branch;
                    allocated = true;
                    allocatedCount++;
                }
            }
        }

        if (!allocated) {
            unallocatedCount++;
        }

        // Update progress
        if ((i + 1) % 100 == 0 || i == students.size() - 1) {
            progressBar->setValue(i + 1);
            statusLabel->setText(QString("Allocated %1/%2 students...")
                                     .arg(allocatedCount).arg(students.size()));
            qApp->processEvents();
        }
    }

    progressBar->setValue(students.size());

    // Display results
    if (unallocatedCount > 0) {
        QMessageBox::warning(this, "Partial Allocation",
                             QString("%1 students could not be allocated due to capacity limits.")
                                 .arg(unallocatedCount));
    } else {
        QMessageBox::information(this, "Success", "All students have been allocated.");
    }

    // Save and show results
    if (!saveAllocationResults()) {
        QMessageBox::critical(this, "Error", "Failed to save allocation results!");
    } else {
        setupResultsTables();
        populateResultTables();
        QMessageBox::information(this, "Success", "Allocation saved successfully.");
    }
}


bool MainWindow::saveAllocationResults() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Allocation Results",
                                                    "allocation_results.csv",
                                                    "CSV Files (*.csv)");
    if (fileName.isEmpty()) return false;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file for writing: " + file.errorString());
        return false;
    }

    QTextStream out(&file);

    // Set the proper text codec for UTF-8 encoding
    out.setEncoding(QStringConverter::Utf8);

    // Write UTF-8 BOM to ensure Excel opens it correctly with proper encoding
    out << "\xEF\xBB\xBF";

    // Write the header
    out << "RollNo,Name,Branch,Hall,Table,Side\n";

    // Write data rows
    for (const auto &s : students) {
        auto escapeCSV = [](const QString &str) {
            QString escaped = str;
            if (escaped.contains(',') || escaped.contains('"') || escaped.contains('\n')) {
                escaped.replace("\"", "\"\"");
                escaped = "\"" + escaped + "\"";
            }
            return escaped;
        };

        out << escapeCSV(s.rollNo) << ","
            << escapeCSV(s.name) << ","
            << escapeCSV(s.branch) << ","
            << escapeCSV(s.hall) << ","
            << (s.table > 0 ? QString::number(s.table) : "UNALLOCATED") << ","
            << (s.side == 1 ? "Left" : (s.side == 2 ? "Right" : "NA")) << "\n";
    }

    file.close();
    return true;
}


MainWindow::~MainWindow()
{
    delete ui;
}
