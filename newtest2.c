#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Structure to store student details
typedef struct student_master {
    int year;
    char roll_no[10];
    char userid[20];
    char password[20];
    char name[50];
    float cgpa;
    int num_prereqs;
    char studprereq[3][50];
} student_master;

// Structure to store course details
typedef struct course_master {
    char course_no[10];
    char course_name[50];
    char faculty_id[10];
    char faculty_name[50];
    int no_of_seats;
    char course_type;
    int wait_list_no;
    char prereq[5];
    int section_count;
    char sections[5][10];
} course_master;

// Structure to store courses chosen by students
typedef struct student_reg_course {
    char roll_no[10];
    char course_no[10];
    char course_name[50];
    char faculty_id[10];
    char faculty_name[50];
    double cgpa;
    char course_type;
    char section[10];
} student_reg_course;

// Structure to store final allotted courses of the student
typedef struct student_alloted_course {
    char roll_no[10];
    char name[50];
    char course_no[10];
    char course_name[50];
    char faculty_id[10];
    char faculty_name[50];
    float cgpa;
    char course_type;
    char alloted;
    int wait_list_no;
    char section[10];
} student_alloted_course;

// Structure to store prerequisite details
typedef struct pre_requisite {
    char pre_req_no[5];
    char pre_req_name[20];
} pre_requisite;

typedef struct wait_list {
    char roll_no[10];
    char course_no[10];
    char course_name[50];
    char faculty_id[10];
    char faculty_name[50];
    float cgpa;
    char course_type;
    int wait_list_no;
    char section[10];
} wait_list;

int registration(student_master *student);
int login(student_master *student);
int course_selection(course_master *course, student_master *student, student_reg_course *regCourse, wait_list *waitList);
void parse_course_master(char *line, course_master *courseObj);
void parse_student_master(char *line, student_master *student);
void write_student_master(FILE *file, student_master *student);
int get_course_info(const char *course_no, course_master *course);
int check_prerequisites(student_master *student, const char *course_no);
void trim_whitespace(char *str);
void parse_prerequisite(char *line, pre_requisite *prereq);
bool is_valid_password(char *password);
void display_courses(course_master *course);
void update_waitlist(course_master *course, wait_list *waitList);
void process_waitlist(course_master *course, wait_list *waitList);

int main() {
    int choice, menuChoice;
    int flag_r = 0, flag_l = 0;
    student_master student;
    course_master course;
    student_reg_course regCourse[6];
    wait_list waitList[100]; // Adjust size as needed

    printf("SSN COLLEGE OF ENGINEERING\n");
    printf("Welcome to the Course Registration system\n\n");
    printf("1. Registration\n2. Login\n\n");
choose_choice:
    printf("Enter a choice:");
    scanf("%d", &choice);

    if (choice == 1) {
        flag_r = registration(&student);
    } else if (choice == 2) {
        int attempts = 0;
login_attempt:  
        attempts++;
        flag_l = login(&student);
        if (flag_l != 1 && attempts < 3) {
            printf("Please try again. Attempt %d of 3.\n", attempts);
            goto login_attempt;
        }
    } else {
        printf("The entered choice is not valid.\n");
        goto choose_choice;
    }

    if ((flag_r == 1) || (flag_l == 1)) {
        printf("\nWould you like to\n1. Register for the courses\n2. View the waitlist\n3. Add courses\n4. Drop courses\n");
        printf("Enter the choice:");
        scanf("%d", &menuChoice);
        if (menuChoice == 1) {
            course_selection(&course, &student, regCourse, waitList);
        }
    }
    return 0;
}

int registration(student_master *student) {
    FILE *file, *prereqFile;
    char filename[] = "student_master.txt";
    char line[200], password[25];
    int i, prereqCount = 0, year;
    pre_requisite prereqs[100];

    printf("\nChoose your academic year group:\n1. 2023-2027\n2. 2022-2026\n3. 2021-2025\n4. 2020-2024\n\n");
choose_year:
    printf("Enter your choice:");
    scanf("%d", &year);
    student->year = year;
    if (year > 0 && year < 4) {
        printf("Enter reg_no (Registration number is the one shared during the admission process SSNXXX): ");
        scanf("%s", student->roll_no);
        printf("Enter name: ");
        scanf("%s", student->name);
        printf("Enter userid: ");
        scanf("%s", student->userid);
choose_password:
        printf("Enter password: ");
        scanf("%s", password);
        if (!(is_valid_password(password))) {
            printf("Password must contain at least one uppercase letter, one lowercase letter, one digit, and one special character.\n");
            goto choose_password;
        }
        strcpy(student->password, password);
choose_cgpa:
        printf("Enter your CGPA of the previous year: "); 
        scanf("%f", &student->cgpa);
        if (student->cgpa < 0 || student->cgpa > 10) {
            printf("Entered CGPA is invalid. Enter CGPA from 1 to 10.\n");
            goto choose_cgpa;
        }

        prereqFile = fopen("pre_requisites.txt", "r");
        if (prereqFile == NULL) {
            printf("Error opening file for reading.\n");
            return 0;
        }

        while (fgets(line, sizeof(line), prereqFile) != NULL) {
            parse_prerequisite(line, &prereqs[prereqCount]);
            prereqCount++;
        }
        fclose(prereqFile);

        printf("\nPRE-REQUISITE LIST:\n\n");
        printf("|%12s| %25s|\n", "PRE-REQ CODE", "COURSE NAME");
        for (i = 0; i < prereqCount; i++) {
            printf("|%12s| %25s|\n", prereqs[i].pre_req_no, prereqs[i].pre_req_name);
        };
        printf("\n");

choose_pre_req_count:
        printf("How many pre-requisites have you completed: ");
        scanf("%d", &student->num_prereqs);
        if (student->num_prereqs > prereqCount || student->num_prereqs < 0) {
            printf("The maximum number of pre-requisites is %d. Enter a valid number of pre-requisites.\n", prereqCount);
            goto choose_pre_req_count;
        }

        for (i = 0; i < student->num_prereqs; i++) {
            printf("Please enter completed pre-requisite no. %d: ", i + 1);
            scanf("%s", student->studprereq[i]);
        }

        file = fopen(filename, "a+");
        if (file == NULL) {
            printf("Error opening file %s for writing.\n", filename);
            return 0;
        }

        fprintf(file, "%d,%s,%s,%s,%s,%f", student->year, student->roll_no, student->name, student->userid, student->password, student->cgpa);
        for (i = 0; i < student->num_prereqs; i++) {
            fprintf(file, ",%s", student->studprereq[i]);
        }
        fprintf(file, "\n");

        fclose(file);

        printf("Student registered successfully!\n");
        return 1;
    } else {
        printf("Enter a valid year (1-4).\n");
        goto choose_year;
    }
}

int login(student_master *student) {
    char userid[20], password[20];
    FILE *file;
    char line[200];

    printf("Enter userid: ");
    scanf("%s", userid);
    printf("Enter password: ");
    scanf("%s", password);

    file = fopen("student_master.txt", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        parse_student_master(line, student);
        if (strcmp(userid, student->userid) == 0 && strcmp(password, student->password) == 0) {
            fclose(file);
            printf("Login successful!\n");
            return 1;
        }
    }
    fclose(file);
    printf("Invalid credentials. Please try again.\n");
    return 0;
}

int course_selection(course_master *course, student_master *student, student_reg_course *regCourse, wait_list *waitList) {
    FILE *courseList, *studentRegCourse, *waitListFile;
    char line[200];
    int course_count, j;
    int count = 0;

    courseList = fopen("course_master.txt", "r");
    if (courseList == NULL) {
        printf("Error opening course file.\n");
        return 0;
    }

    printf("\nAvailable courses in the semester:\n");
    printf("|%-12s|%-30s|%-20s|%-20s|%-10s|\n", "COURSE CODE", "COURSE NAME", "FACULTY ID", "FACULTY NAME", "SEATS");

    while (fgets(line, sizeof(line), courseList) != NULL) {
        parse_course_master(line, course);
        printf("|%-12s|%-30s|%-20s|%-20s|%-10d|\n", course->course_no, course->course_name, course->faculty_id, course->faculty_name, course->no_of_seats);
    }
    fclose(courseList);

    printf("\nYou can register for 4 to 6 courses.\n");
    printf("Please enter the number of courses you wish to register for: ");
    scanf("%d", &course_count);

    if (course_count < 4 || course_count > 6) {
        printf("You must choose between 4 and 6 courses.\n");
        return 0;
    }

    for (j = 0; j < course_count; j++) {
        printf("Enter Course Code for course %d: ", j + 1);
        scanf("%s", regCourse[j].course_no);

        if (get_course_info(regCourse[j].course_no, course) == 0) {
            printf("Invalid course code. Please try again.\n");
            j--;
            continue;
        }

        if (check_prerequisites(student, regCourse[j].course_no) == 0) {
            printf("You do not meet the prerequisites for this course.\n");
            j--;
            continue;
        }

        strcpy(regCourse[j].faculty_id, course->faculty_id);
        strcpy(regCourse[j].faculty_name, course->faculty_name);
        strcpy(regCourse[j].course_name, course->course_name);
        strcpy(regCourse[j].roll_no, student->roll_no);
        regCourse[j].cgpa = student->cgpa;
        regCourse[j].course_type = course->course_type;
        strcpy(regCourse[j].section, course->sections[0]); // Assuming default section for now.
    }

    studentRegCourse = fopen("student_course_registered.txt", "a+");
    if (studentRegCourse == NULL) {
        printf("Error opening student course file.\n");
        return 0;
    }

    for (j = 0; j < course_count; j++) {
        fprintf(studentRegCourse, "%s,%s,%s,%s,%s,%lf,%c,%s\n", regCourse[j].roll_no, regCourse[j].course_no, regCourse[j].course_name, regCourse[j].faculty_id, regCourse[j].faculty_name, regCourse[j].cgpa, regCourse[j].course_type, regCourse[j].section);
    }
    fclose(studentRegCourse);
    printf("Courses registered successfully!\n");

    printf("Updating waitlist...\n");
    update_waitlist(course, waitList);

    process_waitlist(course, waitList);

    return 1;
}

void parse_course_master(char *line, course_master *courseObj) {
    sscanf(line, "%[^,],%[^,],%[^,],%[^,],%d,%c,%d,%[^,],%d",
           courseObj->course_no, courseObj->course_name, courseObj->faculty_id, courseObj->faculty_name,
           &courseObj->no_of_seats, &courseObj->course_type, &courseObj->wait_list_no, courseObj->prereq, &courseObj->section_count);
}

void parse_student_master(char *line, student_master *student) {
    sscanf(line, "%d,%[^,],%[^,],%[^,],%[^,],%f", &student->year, student->roll_no, student->name, student->userid, student->password, &student->cgpa);
}

void write_student_master(FILE *file, student_master *student) {
    fprintf(file, "%d,%s,%s,%s,%s,%f", student->year, student->roll_no, student->name, student->userid, student->password, student->cgpa);
    for (int i = 0; i < student->num_prereqs; i++) {
        fprintf(file, ",%s", student->studprereq[i]);
    }
    fprintf(file, "\n");
}

int get_course_info(const char *course_no, course_master *course) {
    FILE *file = fopen("course_master.txt", "r");
    char line[200];

    while (fgets(line, sizeof(line), file) != NULL) {
        parse_course_master(line, course);
        if (strcmp(course_no, course->course_no) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int check_prerequisites(student_master *student, const char *course_no) {
    FILE *file = fopen("course_master.txt", "r");
    char line[200];
    course_master course;
    int i;

    while (fgets(line, sizeof(line), file) != NULL) {
        parse_course_master(line, &course);
        if (strcmp(course.course_no, course_no) == 0) {
            for (i = 0; i < student->num_prereqs; i++) {
                if (strcmp(student->studprereq[i], course.prereq) == 0) {
                    fclose(file);
                    return 1;
                }
            }
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 0;
}

void trim_whitespace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
}

void parse_prerequisite(char *line, pre_requisite *prereq) {
    sscanf(line, "%[^,],%[^\n]", prereq->pre_req_no, prereq->pre_req_name);
}

bool is_valid_password(char *password) {
    int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0;
    if (strlen(password) < 8) return false;
    for (int i = 0; i < strlen(password); i++) {
        if (isupper(password[i])) hasUpper = 1;
        if (islower(password[i])) hasLower = 1;
        if (isdigit(password[i])) hasDigit = 1;
        if (ispunct(password[i])) hasSpecial = 1;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

void display_courses(course_master *course) {
    FILE *courseList = fopen("course_master.txt", "r");
    char line[200];

    printf("\nAvailable courses in the semester:\n");
    printf("|%-12s|%-30s|%-20s|%-20s|%-10s|\n", "COURSE CODE", "COURSE NAME", "FACULTY ID", "FACULTY NAME", "SEATS");

    while (fgets(line, sizeof(line), courseList) != NULL) {
        parse_course_master(line, course);
        printf("|%-12s|%-30s|%-20s|%-20s|%-10d|\n", course->course_no, course->course_name, course->faculty_id, course->faculty_name, course->no_of_seats);
    }
    fclose(courseList);
}

void update_waitlist(course_master *course, wait_list *waitList) {
    FILE *file = fopen("student_course_registered.txt", "r");
    char line[200];
    int count = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%lf,%c,%[^\n]", 
               waitList[count].roll_no, waitList[count].course_no, waitList[count].course_name, waitList[count].faculty_id, 
               waitList[count].faculty_name, &waitList[count].cgpa, &waitList[count].course_type, waitList[count].section);
        count++;
    }
    fclose(file);
}

void process_waitlist(course_master *course, wait_list *waitList) {
    FILE *waitListFile = fopen("waitlist.txt", "a+");
    FILE *studentRegCourse = fopen("student_course_registered.txt", "a+");
    char line[200];
    int waitListCount = 0;

    while (fgets(line, sizeof(line), waitListFile) != NULL) {
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%lf,%c,%[^\n]", 
               waitList[waitListCount].roll_no, waitList[waitListCount].course_no, waitList[waitListCount].course_name, waitList[waitListCount].faculty_id, 
               waitList[waitListCount].faculty_name, &waitList[waitListCount].cgpa, &waitList[waitListCount].course_type, waitList[waitListCount].section);
        waitListCount++;
    }
    fclose(waitListFile);
}

