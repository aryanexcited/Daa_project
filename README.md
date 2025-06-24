
# Exam Hall Allocation System

A Qt-based C++ application to efficiently allocate students to exam halls based on branch-wise constraints and seating capacity, ensuring no two students from the same branch are seated together at the same table.

---

## Features

- 📁 Upload student and hall CSV files
- 🧠 Smart greedy allocation to avoid seating same-branch students together
- 🪑 Tracks hall capacity, table, and seat-side (left/right)
- 📊 Displays allocation results in GUI tables
- 💾 Exports results to a CSV file
- 📈 Real-time progress tracking and visual feedback
- 🌈 Polished and modern UI with live summary stats

---

## Folder Structure

```
├── main.cpp
├── mainwindow.cpp
├── mainwindow.h
├── mainwindow.ui
├── data_structures.h
├── CMakeLists.txt
└── README.md
```

---

## How It Works

1. **Load Data**:
   - Upload a student CSV (name, roll number, branch)
   - Upload a hall CSV (hall name, capacity)

     ![WhatsApp Image 2025-05-18 at 19 23 20 (3)](https://github.com/user-attachments/assets/eebf0105-6f58-428c-b0bc-6d71e137435e)


2. **Allocation Logic**:
   - Each table seats 2 students (left/right)
   - Branch conflict is checked to ensure diverse seating
   - Students are iteratively allocated to available valid seats

3. **Display and Export**:
   - Allocation results are shown in the UI (tabular)
   - Can be exported to `allocation_results.csv`
   
   ![WhatsApp Image 2025-05-18 at 19 23 20 (2)](https://github.com/user-attachments/assets/20f7e3de-6637-4019-bf09-ef3c8424ec17)

---

## Sample Input Format

### Student File (`students.csv`)
```
Name,Roll No,Branch
John Doe,1234,CSE
Jane Smith,5678,ECE
```

### Hall File (`halls.csv`)
```
Hall Name,Capacity
Hall A,40
Hall B,60
```

---

## Technologies Used

- **C++17**
- **Qt 5/6** (Widgets, UI Design)
- **QTableView** for result display
- **CSV parsing** using `QTextStream`

---

## Setup & Build Instructions

### Prerequisites:
- Qt Creator or Qt libraries installed
- CMake if building via CLI

### Build via Qt Creator:
1. Open `.pro` or `CMakeLists.txt`
2. Configure project
3. Build and run

---

## Allocation Constraints

- One table = 2 seats (Left, Right)
- No same-branch students on same table
- Greedy allocation: first available valid seat
- Unallocated students (if any) are notified

---

## Output Example (`allocation_results.csv`)

```
RollNo,Name,Branch,Hall,Table,Side
1234,John Doe,CSE,Hall A,1,Left
5678,Jane Smith,ECE,Hall A,1,Right


```

---

## Contributors

- Aryan and team
