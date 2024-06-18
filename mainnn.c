#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

//structure for storing all student details
typedef struct student_master {
    int year;
    char roll_no[10];
    char userid[20];
    char password[20];
    char name[50];
    float cgpa;
    int num_prereqs;
    char studprereq[3][50];
} studentmaster;

//structure for storing all the course details
typedef struct course_master {
    char course_no[10];
    char course_name[50];
    char faculty_id[10];
    char faculty_name[50];
    int no_of_seats;
    char course_type;
    int wait_list_no;
    char prereq[5];
} coursemaster;

//structure for storing the courses chosen by the students
typedef struct student_reg_course {
    char roll_no[10];
    char course_no[10];
    char course_name[50];
    char faculty_id[10];
    char faculty_name[50];
    double cgpa;
    char course_type;
} studentregcourse;

//structure for storing the final allotted courses of the student finally
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
} studentallotedcourse;

//structure to store the pre-requisites details
typedef struct pre_requisite{
	char pre_req_no[5];
	char pre_req_name[20];
}prereqlist;


int registration(studentmaster *student);
int login(studentmaster *student);
int courseSelection(coursemaster *course, studentmaster *student, studentregcourse *regCourse);
void parse_course_master(char *line, coursemaster *courseObj);
void parse_student_master(char *line, studentmaster *student);
void write_student_master(FILE *file, studentmaster *student);
int get_course_info(const char *course_no, coursemaster *course);
int check_prerequisites(studentmaster *student, const char *course_no);
void trim_whitespace(char *str);
void parse_prerequisite(char *line, prereqlist *prereq);
bool isValidPassword(char *password);

int main() {
    int choice, menuChoice;
    int flag_r = 0, flag_l = 0;
    studentmaster student;
    coursemaster course;
    studentregcourse regCourse[6];

    printf("SSN COLLEGE OF ENGINEERING\n");
    printf("Welcome to the Course Registration system\n\n");
    printf("1. Registration\n2. Login\n\n");
    choose_choice:
    printf("Enter a choice:");
    scanf("%d", &choice);

    if (choice == 1) 
	{
        flag_r = registration(&student);
    } 
	else if (choice == 2) 
	{
        int attempts = 0;
        login_attempt:  
        attempts++;
        flag_l = login(&student);
        if (flag_l != 1 && attempts < 3) //allowing user to login 3 times in case of username or the password being wrong
		{
            printf("Please try again. Attempt %d of 3.\n", attempts);
            goto login_attempt; 
        }
    } 
	else 
	{
        printf("The entered choice is not valid.\n");
        goto choose_choice;
    }

    if ((flag_r == 1) || (flag_l == 1)) 
	{
        printf("\nWould you like to\n1. Register for the courses\n2. View the waitlist\n3. Add courses\n4. Drop courses\n");
        printf("Enter the choice:");
        scanf("%d", &menuChoice);
        if (menuChoice == 1) 
		{
            courseSelection(&course, &student, regCourse);
        }
    }
    return 0;
}

int registration(studentmaster *student) 
{
    FILE *file,*prereqFile;
    char filename[] = "student_master.txt";
	char line[200],password[25];
    int i,prereqCount=0,j,found,year;
    prereqlist prereqs[100];

    printf("\nChoose your academic year group:\n1. 2023-2027\n2. 2022-2026\n3. 2021-2025\n4. 2020-2024\n\n");
    choose_year:
	printf("Enter your choice:");
    scanf("%d", &year);
    student->year = year;
    if (year>0 && year<4)
	{
	   printf("Enter reg_no (Regsistration number is the one shared during admission process SSNXXX): ");
       scanf("%s", student->roll_no);
	   printf("Enter name: ");
       scanf("%s", student->name);
       printf("Enter userid: ");
       scanf("%s", student->userid);
       choose_password:
        printf("Enter password: ");
        scanf("%s", password);
        if (!(isValidPassword(password))) //checking whether the entered password is valid using isValidPassword function
		{
            printf("Password must contain at least one uppercase letter, one lowercase letter, one digit, and one special character.\n");
            goto choose_password;
        }
        strcpy(student->password, password);
       choose_cgpa:
       printf("Enter your CGPA of previous year: "); 
       scanf("%f", &student->cgpa);
       if (student->cgpa<0 || student->cgpa>10)  //checking if the CGPA entered is valid
	   {
	   	  printf("Entered CGPA is invalid. Enter CGPA from 1 to 10.\n");
  	      goto choose_cgpa;
	   }
    
       prereqFile = fopen("pre_requisites.txt", "r");
       if (prereqFile == NULL) 
	   {
       	  printf("Error opening file for reading.\n");
          return 0;
 	  }

    while (fgets(line, sizeof(line), prereqFile) != NULL) 
	{
        parse_prerequisite(line, &prereqs[prereqCount]);
        prereqCount++;
    }
    fclose(prereqFile);

    printf("\nPRE-REQUISITE LIST:\n\n");
    printf("|%12s| %25s|\n","PRE-REQ CODE","COURSE NAME");
    for (i = 0; i < prereqCount; i++) {
        printf("|%12s| %25s|\n", prereqs[i].pre_req_no, prereqs[i].pre_req_name);
    };
    printf("\n");
    
    choose_pre_req_count:
    printf("How many pre-requisites have you completed: ");
    scanf("%d", &student->num_prereqs);
    if (student->num_prereqs>prereqCount||student->num_prereqs<0)
    {
    	printf("The maximum number of pre-requisites is %d.Enter valid number of pre-requisites.\n",prereqCount);
    	goto choose_pre_req_count;
	}

    for (i = 0; i < student->num_prereqs; i++) 
	{
        printf("Please enter completed pre-requisite no. %d: ", i + 1);
        scanf("%s", student->studprereq[i]);
        
    }

    file = fopen(filename, "a+");
    if (file == NULL) 
	{
        printf("Error opening file %s for writing.\n", filename);
        return 0;
    }

    fprintf(file, "%d,%s,%s,%s,%s,%f", student->year, student->roll_no, student->name, student->userid, student->password, student->cgpa);
    for (i = 0; i < student->num_prereqs; i++) 
	{
        fprintf(file, ",%s", student->studprereq[i]);
    }
    fprintf(file, "\n");

    fclose(file);

    printf("Student registered successfully!\n");
    return 1;
	}
    
    else
	{
    	printf("Enter a valid year(1-4).\n");
    	goto choose_year;
	}
}

int login(studentmaster *student) 
{
    char userid[20], password[20];
    FILE *file;
    char line[200];

    printf("Enter userid: ");
    scanf("%s", userid);
    printf("Enter password: ");
    scanf("%s", password);

    file = fopen("student_master.txt", "r");
    if (file == NULL) 
	{
        printf("Error opening file for reading.\n");
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) 
	{
        parse_student_master(line, student);
        if (strcmp(userid, student->userid) == 0 && strcmp(password, student->password) == 0) 
		{
            printf("Login successful!\n");
            fclose(file);
            return 1;
        }
    }

    printf("Invalid credentials.\n");
    fclose(file);
    return -1;
}

int courseSelection(coursemaster *course, studentmaster *student, studentregcourse *regCourse) 
{
    int course_count, j;
    FILE *courseList, *studentRegCourse;
    char line[200];

    printf("\nWelcome to the Course selection page\n\n");
    printf("Please select the courses you want to register for\n");

    courseList = fopen("course_master.txt", "r");
    if (courseList == NULL) 
	{
        printf("Error opening file for reading.\n");
        return 0;
    }

    printf("|%10s| %15s| %10s| %13s| %5s| %4s| %13s|\n", "COURSE_NO", "COURSE_NAME", "FACULTY_ID", "FACULTY_NAME", "SEATS", "TYPE", "PRE-REQUISITE");
    while (fgets(line, sizeof(line), courseList) != NULL) 
	{
        parse_course_master(line, course);
        printf("|%10s| %15s| %10s| %13s| %5d| %4c| %13s|\n", course->course_no, course->course_name, course->faculty_id, course->faculty_name, course->no_of_seats, course->course_type, course->prereq);
    }
    fclose(courseList);
    
choose_course_count:
    printf("Enter the number of courses you'd like to register for (minimum 4, maximum 6): ");
    scanf("%d", &course_count);

    if (course_count >= 4 && course_count <= 6) 
	{
        for (j = 0; j < course_count; j++) 
		{
        	choose_alternate_course:
            printf("Enter the course_no you'd like to register for: ");
            scanf("%s", regCourse[j].course_no);

            if (!check_prerequisites(student, regCourse[j].course_no)) 
			{
                printf("Cannot register for course %s. Prerequisites not met.\n", regCourse[j].course_no);
                goto choose_alternate_course;
            }

            get_course_info(regCourse[j].course_no, course);

            strcpy(regCourse[j].roll_no, student->roll_no);
            strcpy(regCourse[j].course_name, course->course_name);
            strcpy(regCourse[j].faculty_id, course->faculty_id);
            strcpy(regCourse[j].faculty_name, course->faculty_name);
            regCourse[j].cgpa = student->cgpa;
            regCourse[j].course_type = course->course_type;

            studentRegCourse = fopen("student_course_registered.txt", "a");
            if (studentRegCourse == NULL) 
			{
                printf("Error opening file for writing.\n");
                return 0;
            }
            fprintf(studentRegCourse, "%s,%s,%s,%s,%s,%f,%c\n",
                    regCourse[j].roll_no, regCourse[j].course_no,
                    regCourse[j].course_name, regCourse[j].faculty_id,
                    regCourse[j].faculty_name, regCourse[j].cgpa,
                    regCourse[j].course_type);
            fclose(studentRegCourse);
        }
    } 
	else 
	{
        printf("You must register for at least 4 courses and at most 6 courses.\n");
        goto choose_course_count;
    }
	printf("Registration of courses is successful!\n");
	
    return 1;
}

int get_course_info(const char *course_no, coursemaster *course) 
{
    FILE *file;
    char line[200];

    file = fopen("course_master.txt", "r");
    if (file == NULL) 
	{
        printf("Error opening file for reading.\n");
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) 
	{
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%d,%c,%[^,],%d",
               course->course_no, course->course_name,
               course->faculty_id, course->faculty_name,
               &course->no_of_seats, &course->course_type,
               course->prereq, &course->wait_list_no);
        
        if (strcmp(course->course_no, course_no) == 0) 
		{
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int check_prerequisites(studentmaster *student, const char *course_no) 
{
    FILE *file;
    char line[200];
    coursemaster course;
    int prereq_met = 0;
    int i;

    file = fopen("course_master.txt", "r");
    if (file == NULL) 
	{
        printf("Error opening file for reading.\n");
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) 
	{
        parse_course_master(line, &course);

        trim_whitespace(course.prereq);

        if (strcmp(course.course_no, course_no) == 0) 
		{
            if (strlen(course.prereq) == 0) 
			{
                printf("Course %s has no prerequisites. Registration allowed.\n", course_no);
                fclose(file);
                return 1;
            }

            if (strlen(course.))

            for (i = 0; i < student->num_prereqs; i++) 
			{
                trim_whitespace(student->studprereq[i]);

                printf("Comparing course prerequisite '%s' with student's completed prerequisite '%s'\n", course.prereq, student->studprereq[i]);

                if (strcmp(course.prereq, student->studprereq[i]) == 0) 
				{
                    prereq_met = 1;
                    break;
                }
            }
            if (!prereq_met) 
			{
                printf("Prerequisite %s not met for course %s.\n", course.prereq, course_no);
                fclose(file);
                return 0;
            }
            fclose(file);
            printf("All prerequisites met for course %s.\n", course_no);
            return 1;
        }
    }

    printf("Course %s not found in course master file.\n", course_no);
    fclose(file);
    return 0;
}

void parse_course_master(char *line, coursemaster *courseObj) 
{
    sscanf(line, "%[^,],%[^,],%[^,],%[^,],%d,%c,%[^,],%d",
           courseObj->course_no, courseObj->course_name,
           courseObj->faculty_id, courseObj->faculty_name,
           &courseObj->no_of_seats, &courseObj->course_type,
           courseObj->prereq, &courseObj->wait_list_no);
}

void parse_student_master(char *line, studentmaster *student) 
{
    char *token;
    int i = 0;
    
    token = strtok(line, ",");
    student->year = atoi(token);
    token = strtok(NULL, ",");
    strcpy(student->roll_no, token);
    token = strtok(NULL, ",");
    strcpy(student->name, token);
    token = strtok(NULL, ",");
    strcpy(student->userid, token);
    token = strtok(NULL, ",");
    strcpy(student->password, token);
    token = strtok(NULL, ",");
    student->cgpa = atof(token);
    student->num_prereqs = 0;
    while ((token = strtok(NULL, ",")) != NULL) {
        strcpy(student->studprereq[student->num_prereqs], token);
        student->num_prereqs++;
    }
}

void trim_whitespace(char *str) 
{
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == ' ')) 
	{
        str[--len] = '\0';
    }
}

void parse_prerequisite(char *line, prereqlist *prereq) 
{
    sscanf(line, "%[^,],%[^\n]", prereq->pre_req_no, prereq->pre_req_name);
}

bool isValidPassword(char *password) 
{
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;
    int i;

    for (i = 0; i < strlen(password); i++) 
	{
        if (isupper(password[i])) 
		{
            hasUpper = true;
        } 
		else if (islower(password[i])) 
		{
            hasLower = true;
        } 
		else if (isdigit(password[i])) 
		{
            hasDigit = true;
        } 
		else if (ispunct(password[i])) 
		{
            hasSpecial = true;
        }
    }

    return hasUpper && hasLower && hasDigit && hasSpecial;
}

