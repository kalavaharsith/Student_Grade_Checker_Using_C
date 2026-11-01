#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_STUDENTS 100
#define MAX_SUBJECTS 10
#define MAX_NAME 50
#define FILE_NAME "students.dat"

typedef struct {
    char name[MAX_NAME];
    float marks;
    float maxMarks;
    int credits;
} Subject;

typedef struct {
    int id;
    char name[MAX_NAME];
    char department[MAX_NAME];

    int semester;
    int subjectCount;

    float attendance;
    float previousPercentage;

    Subject subjects[MAX_SUBJECTS];
} Student;

Student students[MAX_STUDENTS];
int studentCount = 0;

/* ---------- INPUT FUNCTIONS ---------- */

void readString(char message[], char value[], int size) {
    printf("%s", message);

    fgets(value, size, stdin);

    value[strcspn(value, "\n")] = '\0';
}

int readInt(const char *message, int min, int max) {
    int value;
    char line[100];

    while (1) {
        printf("%s", message);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            continue;
        }

        if (sscanf(line, "%d", &value) == 1 &&
            value >= min && value <= max) {
            return value;
        }

        printf("Invalid input. Enter between %d and %d.\n",
               min, max);
    }
}

float readFloat(const char *message, float min, float max) {
    float value;
    char line[100];
    char extra;

    while (1) {
        printf("%s", message);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            continue;
        }

        if (sscanf(line, "%f %c", &value, &extra) == 1 &&
            isfinite(value) &&
            value >= min && value <= max) {
            return value;
        }

        printf("Invalid input. Enter between %.1f and %.1f.\n",
               min, max);
    }
}

/* ---------- FILE HANDLING ---------- */

void saveData() {
    FILE *fp = fopen(FILE_NAME, "wb");

    if (fp == NULL) {
        printf("Error saving records.\n");
        return;
    }

    fwrite(&studentCount, sizeof(int), 1, fp);

    fwrite(students, sizeof(Student), studentCount, fp);

    fclose(fp);

    printf("Records saved successfully.\n");
}

void loadData() {
    FILE *fp = fopen(FILE_NAME, "rb");

    if (fp == NULL) {
        return;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return;
    }

    long fileSize = ftell(fp);
    rewind(fp);

    if (fileSize <= 0) {
        fclose(fp);
        studentCount = 0;
        return;
    }

    int count;

    if (fread(&count, sizeof(int), 1, fp) != 1 ||
        count < 0 || count > MAX_STUDENTS) {
        fclose(fp);
        printf("Invalid data file.\n");
        studentCount = 0;
        return;
    }

    size_t readCount = (size_t)count;

    if (fread(students, sizeof(Student), readCount, fp) != readCount) {
        fclose(fp);
        studentCount = 0;
        printf("Could not read student records.\n");
        return;
    }

    studentCount = count;

    fclose(fp);
}

/* ---------- STUDENT SEARCH ---------- */

int findStudent(int id) {
    for (int i = 0; i < studentCount; i++) {
        if (students[i].id == id) {
            return i;
        }
    }

    return -1;
}

/* ---------- ACADEMIC CALCULATIONS ---------- */

float totalMarks(Student s) {
    float total = 0;

    for (int i = 0; i < s.subjectCount; i++) {
        total += s.subjects[i].marks;
    }

    return total;
}

float maximumMarks(Student s) {
    float total = 0;

    for (int i = 0; i < s.subjectCount; i++) {
        total += s.subjects[i].maxMarks;
    }

    return total;
}

float percentage(Student s) {
    float maximum = maximumMarks(s);

    if (maximum == 0) {
        return 0;
    }

    return (totalMarks(s) / maximum) * 100;
}

/* Sample grading system */

const char *grade(float p) {
    if (p >= 90) return "O";
    if (p >= 80) return "A+";
    if (p >= 70) return "A";
    if (p >= 60) return "B+";
    if (p >= 50) return "B";
    if (p >= 40) return "C";

    return "F";
}

float gradePoint(float p) {
    if (p >= 90) return 10;
    if (p >= 80) return 9;
    if (p >= 70) return 8;
    if (p >= 60) return 7;
    if (p >= 50) return 6;
    if (p >= 40) return 5;

    return 0;
}

float gpa(Student s) {
    float weightedPoints = 0;
    int credits = 0;

    for (int i = 0; i < s.subjectCount; i++) {
        float p = (s.subjects[i].marks /
                   s.subjects[i].maxMarks) * 100;

        weightedPoints +=
            gradePoint(p) * s.subjects[i].credits;

        credits += s.subjects[i].credits;
    }

    if (credits == 0) {
        return 0;
    }

    return weightedPoints / credits;
}

int failedSubjects(Student s) {
    int failed = 0;

    for (int i = 0; i < s.subjectCount; i++) {
        float p = (s.subjects[i].marks /
                   s.subjects[i].maxMarks) * 100;

        if (p < 40) {
            failed++;
        }
    }

    return failed;
}

int studentPassed(Student s) {
    return failedSubjects(s) == 0 && s.attendance >= 75;
}

float cgpa(Student s) {
    float currentGPA = gpa(s);

    if (s.semester <= 1) {
        return currentGPA;
    }

    /*
       Demonstration:
       Previous semester GPA is approximated from
       previous percentage.

       Replace this with actual semester GPA records
       for an official cumulative CGPA.
    */

    float previousGPA = gradePoint(s.previousPercentage);

    return ((previousGPA * (s.semester - 1)) +
            currentGPA) / s.semester;
}

/* ---------- PERSONALIZED IMPROVEMENT ---------- */

void improvementPlan(Student s) {
    int weakest = 0;
    float lowest = 101;
    int backlogCount = failedSubjects(s);

    printf("\nPERSONALIZED IMPROVEMENT PLAN\n");

    for (int i = 0; i < s.subjectCount; i++) {
        float p = (s.subjects[i].marks /
                   s.subjects[i].maxMarks) * 100;

        if (p < lowest) {
            lowest = p;
            weakest = i;
        }
    }

    if (backlogCount > 0) {
        printf("Backlog subjects: %d\n", backlogCount);

        for (int i = 0; i < s.subjectCount; i++) {
            float p = (s.subjects[i].marks /
                       s.subjects[i].maxMarks) * 100;

            if (p < 40) {
                printf("- %s: Review the basic topics and ask your teacher "
                       "about anything unclear.\n",
                       s.subjects[i].name);
                printf("  Practice a few questions every day and take "
                       "another mock test.\n");
            }
        }
    } else {
        printf("No subject backlogs.\n");
        printf("Weakest Subject: %s (%.2f%%)\n",
               s.subjects[weakest].name, lowest);
        printf("Keep revising this subject and practise questions regularly.\n");
    }

    if (s.attendance < 75) {
        printf("Attendance is below 75%%, so the overall result is FAIL.\n");
        printf("Attend classes regularly to improve your attendance.\n");
    }
}

/* ---------- STUDENT REGISTRATION ---------- */

void addStudent() {
    if (studentCount >= MAX_STUDENTS) {
        printf("Student capacity reached.\n");
        return;
    }

    Student s = {0};

    printf("\nADD STUDENT\n");

    s.id = readInt("Student ID: ", 1, 999999);

    if (findStudent(s.id) != -1) {
        printf("Student ID already exists.\n");
        return;
    }

    readString("Student Name: ", s.name, MAX_NAME);

    readString("Department: ", s.department, MAX_NAME);

    s.semester = readInt("Semester: ", 1, 8);

    s.subjectCount = readInt(
        "Number of Subjects: ", 1, MAX_SUBJECTS);

    s.previousPercentage = readFloat(
        "Previous Semester Percentage: ", 0, 100);

    for (int i = 0; i < s.subjectCount; i++) {
        printf("\nSUBJECT %d\n", i + 1);

        readString("Subject Name: ",
                   s.subjects[i].name, MAX_NAME);

        s.subjects[i].maxMarks = readFloat(
            "Maximum Marks: ", 1, 1000);

        s.subjects[i].marks = readFloat(
            "Obtained Marks: ",
            0, s.subjects[i].maxMarks);

        s.subjects[i].credits = readInt(
            "Subject Credits: ", 1, 10);
    }

    s.attendance = readFloat(
        "Attendance Percentage: ", 0, 100);

    students[studentCount++] = s;

    saveData();

    printf("\nStudent added successfully!\n");

    printf("Percentage: %.2f%%\n", percentage(s));

    printf("GPA: %.2f\n", gpa(s));
}

/* ---------- REPORT CARD ---------- */

void reportCard(Student s) {
    printf("\n====================================\n");
    printf("       STUDENT REPORT CARD\n");
    printf("====================================\n");

    printf("ID         : %d\n", s.id);
    printf("Name       : %s\n", s.name);
    printf("Department  : %s\n", s.department);
    printf("Semester    : %d\n", s.semester);

    printf("------------------------------------\n");

    printf("%-15s %-10s %-8s\n",
           "Subject", "Marks", "Grade");

    for (int i = 0; i < s.subjectCount; i++) {
        float p = (s.subjects[i].marks /
                   s.subjects[i].maxMarks) * 100;

        printf("%-15s %5.1f/%-4.1f %-8s\n",
               s.subjects[i].name,
               s.subjects[i].marks,
               s.subjects[i].maxMarks,
               grade(p));
    }

    printf("------------------------------------\n");

    printf("Total Marks : %.2f / %.2f\n",
           totalMarks(s), maximumMarks(s));

    printf("Percentage  : %.2f%%\n", percentage(s));

    printf("Grade      : %s\n", grade(percentage(s)));

    printf("SGPA       : %.2f / 10\n", gpa(s));

    printf("CGPA       : %.2f / 10\n", cgpa(s));

    printf("Attendance  : %.2f%%\n", s.attendance);
    printf("Attendance Status: %s\n",
           s.attendance < 75 ? "FAIL (below 75%)" : "PASS");

    printf("Result     : %s\n",
           studentPassed(s) ? "PASS" : "FAIL");

    printf("Failed Subjects: %d\n", failedSubjects(s));

    printf("====================================\n");

    improvementPlan(s);
}

/* ---------- SEARCH STUDENT ---------- */

void searchStudent() {
    int id = readInt("Enter Student ID: ", 1, 999999);

    int index = findStudent(id);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    reportCard(students[index]);
}

/* ---------- UPDATE MARKS ---------- */

void updateMarks() {
    int id = readInt("Student ID: ", 1, 999999);

    int index = findStudent(id);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    Student *s = &students[index];

    for (int i = 0; i < s->subjectCount; i++) {
        printf("%d. %s\n", i + 1, s->subjects[i].name);
    }

    int choice = readInt(
        "Select Subject: ", 1, s->subjectCount);

    Subject *sub = &s->subjects[choice - 1];

    sub->marks = readFloat(
        "Enter New Marks: ", 0, sub->maxMarks);

    saveData();

    printf("Marks updated successfully.\n");
}

/* ---------- CLASS ANALYTICS ---------- */

void classAnalytics() {
    if (studentCount == 0) {
        printf("No student records available.\n");
        return;
    }

    float total = 0;
    float totalGPA = 0;
    float highest = -1;
    float lowest = 101;

    int passed = 0;
    int failed = 0;
    int studentsWithBacklog = 0;

    for (int i = 0; i < studentCount; i++) {
        float p = percentage(students[i]);

        total += p;
        totalGPA += gpa(students[i]);

        if (p > highest) {
            highest = p;
        }

        if (p < lowest) {
            lowest = p;
        }

        if (studentPassed(students[i])) {
            passed++;
        } else {
            failed++;
        }

        if (failedSubjects(students[i]) > 0) {
            studentsWithBacklog++;
        }
    }

    printf("\nCLASS PERFORMANCE ANALYTICS\n");

    printf("Students       : %d\n", studentCount);

    printf("Class Average  : %.2f%%\n",
           total / studentCount);

    printf("Class GPA      : %.2f / 10\n",
           totalGPA / studentCount);

    printf("Highest        : %.2f%%\n", highest);

    printf("Lowest         : %.2f%%\n", lowest);

    printf("Passed         : %d\n", passed);

    printf("Failed         : %d\n", failed);

    printf("Students with Backlog: %d\n", studentsWithBacklog);
}

/* ---------- STUDENT RANKING ---------- */

void ranking() {
    if (studentCount == 0) {
        printf("No students available.\n");
        return;
    }

    Student sorted[MAX_STUDENTS];

    for (int i = 0; i < studentCount; i++) {
        sorted[i] = students[i];
    }

    for (int i = 0; i < studentCount - 1; i++) {
        for (int j = 0; j < studentCount - i - 1; j++) {
            if (percentage(sorted[j]) <
                percentage(sorted[j + 1])) {

                Student temp = sorted[j];

                sorted[j] = sorted[j + 1];

                sorted[j + 1] = temp;
            }
        }
    }

    printf("\nSTUDENT RANKING\n");

    for (int i = 0; i < studentCount; i++) {
        printf("%d. %s | %.2f%% | GPA %.2f\n",
               i + 1,
               sorted[i].name,
               percentage(sorted[i]),
               gpa(sorted[i]));
    }
}

/* ---------- SEMESTER PROGRESS ---------- */

void progressTracker() {
    int id = readInt("Student ID: ", 1, 999999);

    int index = findStudent(id);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    Student s = students[index];

    float current = percentage(s);
    float previous = s.previousPercentage;
    float currentGPA = gpa(s);
    float previousGPA = gradePoint(previous);

    printf("\nSEMESTER PROGRESS\n");

    printf("Previous Percentage: %.2f%%\n", previous);
    printf("Previous SGPA     : %.2f / 10\n", previousGPA);

    printf("Current Percentage : %.2f%%\n", current);
    printf("Current SGPA      : %.2f / 10\n", currentGPA);

    float difference = current - previous;
    float gpaDifference = currentGPA - previousGPA;

    if (difference > 0) {
        printf("Improvement: +%.2f%%\n", difference);
    } else if (difference < 0) {
        printf("Decline: %.2f%%\n", difference);
    } else {
        printf("Performance unchanged.\n");
    }

    if (gpaDifference > 0) {
        printf("GPA Trend: +%.2f\n", gpaDifference);
    } else if (gpaDifference < 0) {
        printf("GPA Trend: %.2f\n", gpaDifference);
    } else {
        printf("GPA unchanged.\n");
    }
}

/* ---------- MENU ---------- */

int main() {
    loadData();

    int choice;

    do {
        printf("\n====================================\n");
        printf(" STUDENT PERFORMANCE MANAGEMENT\n");
        printf("====================================\n");

        printf("1. Add Student\n");
        printf("2. Search Student Report\n");
        printf("3. Update Marks\n");
        printf("4. Class Analytics\n");
        printf("5. Student Ranking\n");
        printf("6. Semester Progress Tracker\n");
        printf("7. Exit\n");

        choice = readInt("Enter Choice: ", 1, 7);

        switch (choice) {
            case 1:
                addStudent();
                break;

            case 2:
                searchStudent();
                break;

            case 3:
                updateMarks();
                break;

            case 4:
                classAnalytics();
                break;

            case 5:
                ranking();
                break;

            case 6:
                progressTracker();
                break;

            case 7:
                saveData();
                printf("Thank you!\n");
                break;
        }

    } while (choice != 7);

    return 0;
}