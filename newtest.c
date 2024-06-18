#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COURSES 100
#define MAX_STUDENTS 100
#define MAX_SECTIONS 10
#define MAX_WAITLIST 10

typedef struct {
    char course_code[10];
    char course_name[100];
    char prerequisites[10][10];
    int num_prerequisites;
} Course;

typedef struct {
    int section_id;
    char course_code[10];
    char faculty_name[100];
    int max_strength;
    int current_strength;
} Section;

typedef struct {
    char student_id[10];
    char name[100];
    char password[50];
    char completed_courses[10][10];
    int num_completed_courses;
} Student;

typedef struct {
    int registration_id;
    char student_id[10];
    int section_id;
    char status[10]; // "allotted" or "waitlisted"
    int waitlist_number;
} Registration;

Course courses[MAX_COURSES];
Section sections[MAX_SECTIONS];
Student students[MAX_STUDENTS];
Registration registrations[MAX_COURSES * MAX_SECTIONS];

int num_courses = 0;
int num_sections = 0;
int num_students = 0;
int num_registrations = 0;

void load_courses(const char *filename);
void load_sections(const char *filename);
void load_students(const char *filename);
void login();
void display_courses(char student_id[]);
void register_courses(char student_id[]);
void drop_course(char student_id[]);
int check_prerequisites(char student_id[], char course_code[]);

int main() {
    load_courses("courses.csv");
    load_sections("sections.csv");
    load_students("students.csv");

    while (1) {
        int choice;
        printf("1. Login\n2. Exit\nChoose an option: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                login();
                break;
            case 2:
                exit(0);
        }
    }
    return 0;
}

void load_courses(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Could not open %s\n", filename);
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        strcpy(courses[num_courses].course_code, token);

        token = strtok(NULL, ",");
        strcpy(courses[num_courses].course_name, token);

        int i = 0;
        while ((token = strtok(NULL, ",")) != NULL) {
            strcpy(courses[num_courses].prerequisites[i], token);
            i++;
        }
        courses[num_courses].num_prerequisites = i;

        num_courses++;
    }
    fclose(file);
}

void load_sections(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Could not open %s\n", filename);
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        sections[num_sections].section_id = atoi(token);

        token = strtok(NULL, ",");
        strcpy(sections[num_sections].course_code, token);

        token = strtok(NULL, ",");
        strcpy(sections[num_sections].faculty_name, token);

        token = strtok(NULL, ",");
        sections[num_sections].max_strength = atoi(token);

        sections[num_sections].current_strength = 0;
        num_sections++;
    }
    fclose(file);
}

void load_students(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Could not open %s\n", filename);
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        strcpy(students[num_students].student_id, token);

        token = strtok(NULL, ",");
        strcpy(students[num_students].name, token);

        token = strtok(NULL, ",");
        strcpy(students[num_students].password, token);

        int i = 0;
        while ((token = strtok(NULL, ",")) != NULL) {
            strcpy(students[num_students].completed_courses[i], token);
            i++;
        }
        students[num_students].num_completed_courses = i;

        num_students++;
    }
    fclose(file);
}

void login() {
    char student_id[10], password[50];
    printf("Student ID: ");
    scanf("%s", student_id);
    printf("Password: ");
    scanf("%s", password);
    
    for (int i = 0; i < num_students; i++) {
        if (strcmp(students[i].student_id, student_id) == 0 && strcmp(students[i].password, password) == 0) {
            printf("Login successful\n");
            int logged_in = 1;
            while (logged_in) {
                int choice;
                printf("\n1. Display Courses\n2. Register for Courses\n3. Drop a Course\n4. Logout\nChoose an option: ");
                scanf("%d", &choice);
                switch (choice) {
                    case 1:
                        display_courses(student_id);
                        break;
                    case 2:
                        register_courses(student_id);
                        break;
                    case 3:
                        drop_course(student_id);
                        break;
                    case 4:
                        logged_in = 0;
                        printf("Logged out successfully.\n");
                        break;
                    default:
                        printf("Invalid choice. Try again.\n");
                }
            }
            return;
        }
    }
    printf("Invalid credentials\n");
}

void drop_course(char student_id[]) {
    char course_code[10];
    int section_id;

    printf("Enter course code to drop: ");
    scanf("%s", course_code);
    printf("Enter section ID: ");
    scanf("%d", &section_id);

    int registration_found = 0;
    for (int i = 0; i < num_registrations; i++) {
        if (strcmp(registrations[i].student_id, student_id) == 0 && registrations[i].section_id == section_id) {
            registration_found = 1;
            registrations[i].registration_id = -1; // Mark registration as dropped
            for (int j = 0; j < num_sections; j++) {
                if (sections[j].section_id == section_id) {
                    sections[j].current_strength--;
                    printf("Dropped course %s, section %d\n", course_code, section_id);

                    // Check for waitlisted students to promote
                    for (int k = 0; k < num_registrations; k++) {
                        if (registrations[k].section_id == section_id && strcmp(registrations[k].status, "waitlisted") == 0) {
                            strcpy(registrations[k].status, "allotted");
                            sections[j].current_strength++;
                            printf("Promoted waitlisted student %s to course %s, section %d\n", registrations[k].student_id, course_code, section_id);
                            break; // Promote one student at a time
                        }
                    }
                    break;
                }
            }
            break;
        }
    }

    if (!registration_found) {
        printf("No registration found for course %s, section %d\n", course_code, section_id);
    }
}


void display_courses(char student_id[]) {
    printf("Available courses:\n");
    for (int i = 0; i < num_courses; i++) {
        printf("%s: %s\n", courses[i].course_code, courses[i].course_name);
        for (int j = 0; j < num_sections; j++) {
            if (strcmp(sections[j].course_code, courses[i].course_code) == 0) {
                printf("  Section %d: %s (Current Strength: %d, Max Strength: %d)\n", sections[j].section_id, sections[j].faculty_name, sections[j].current_strength, sections[j].max_strength);
            }
        }
    }
}


void register_courses(char student_id[]) {
    int num_courses_to_register;
    printf("How many courses do you want to register for? ");
    scanf("%d", &num_courses_to_register);

    if (num_courses_to_register < 4 || num_courses_to_register > 6) {
        printf("You must register for a minimum of 4 and a maximum of 6 courses.\n");
        return;
    }

    for (int i = 0; i < num_courses_to_register; i++) {
        char course_code[10];
        int section_id;
        printf("Enter course code: ");
        scanf("%s", course_code);
        printf("Enter section ID: ");
        scanf("%d", &section_id);

        if (check_prerequisites(student_id, course_code)) {
            for (int j = 0; j < num_sections; j++) {
                if (sections[j].section_id == section_id && strcmp(sections[j].course_code, course_code) == 0) {
                    if (sections[j].current_strength < sections[j].max_strength) {
                        sections[j].current_strength++;
                        registrations[num_registrations].registration_id = num_registrations + 1;
                        strcpy(registrations[num_registrations].student_id, student_id);
                        registrations[num_registrations].section_id = section_id;
                        strcpy(registrations[num_registrations].status, "allotted");
                        registrations[num_registrations].waitlist_number = 0;
                        num_registrations++;
                        printf("Registered for course %s, section %d\n", course_code, section_id);
                    } else {
                        int waitlist_count = 0;
                        for (int k = 0; k < num_registrations; k++) {
                            if (registrations[k].section_id == section_id && strcmp(registrations[k].status, "waitlisted") == 0) {
                                waitlist_count++;
                            }
                        }
                        if (waitlist_count < sections[j].max_strength * 0.1) {
                            registrations[num_registrations].registration_id = num_registrations + 1;
                            strcpy(registrations[num_registrations].student_id, student_id);
                            registrations[num_registrations].section_id = section_id;
                            strcpy(registrations[num_registrations].status, "waitlisted");
                            registrations[num_registrations].waitlist_number = waitlist_count + 1;
                            num_registrations++;
                            printf("Waitlisted for course %s, section %d. Waitlist number: %d\n", course_code, section_id, waitlist_count + 1);
                        } else {
                            printf("Cannot register for course %s, section %d. No seats available and waitlist is full.\n", course_code, section_id);
                        }
                    }
                    break;
                }
            }
        } else {
            printf("Prerequisites not met for course %s\n", course_code);
        }
    }
}



int check_prerequisites(char student_id[], char course_code[]) {
    for (int i = 0; i < num_courses; i++) {
        if (strcmp(courses[i].course_code, course_code) == 0) {
            for (int j = 0; j < courses[i].num_prerequisites; j++) {
                int completed = 0;
                for (int k = 0; k < num_students; k++) {
                    if (strcmp(students[k].student_id, student_id) == 0) {
                        for (int l = 0; l < students[k].num_completed_courses; l++) {
                            if (strcmp(students[k].completed_courses[l], courses[i].prerequisites[j]) == 0) {
                                completed = 1;
                                break;
                            }
                        }
                    }
                }
                if (!completed) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

