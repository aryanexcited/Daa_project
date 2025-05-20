#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H
#include <QString>
#include <QVector>

struct Student {
    QString name;
    QString rollNo;
    QString branch;
    QString hall;
    int table = -1;  // -1 = unallocated
    int side = -1;   // 1 = left, 2 = right
};

struct Hall {
    QString name;
    int capacity;
    int occupied = 0;  // Track used seats
    int tables() const { return capacity / 2; }  // 2 students per table
};

// Global containers
extern QVector<Student> students;
extern QVector<Hall> halls;

#endif // DATA_STRUCTURES_H
