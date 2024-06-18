#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 1000
#define MAX_COURSES 12
#define MAX_STUDENTS 10
#define MAX_CHOICES 10

typedef struct course_master
{
    char course_no[10];
    char course_name[50];
    char faculty_id[10];
    char faculty_name[50];
    int no_of_seats;
    char course_type;
    int wait_list_no;
} coursemaster;

typedef struct student_master
{
    char roll_no[10];
    char first_name[50];
    char last_name[50];
    double cgpa;
} studentmaster;

typedef struct student_reg_course
{
    char roll_no[10];
    char course_no[10];
    char course_name[50];
    char faculty_id[10];
    char faculty_name[50];
    double cgpa;
    char course_type;
} studentregcourse;


typedef struct student_alloted_course
{
    char roll_no[10];
    char first_name[50];
    char last_name[50];
    char course_no[10];
    char course_name[50];
    char faculty_id[10];
    char faculty_name[50];
    float cgpa;
    char course_type;
    char alloted;
    int wait_list_no;
} studentallotedcourse;

int sortByCourseId(const void* a, const void* b);
int sortByFacultyId(const void* a, const void* b);
int sortByCgpa(const void* a, const void* b);
void parse_course_master(char *line, coursemaster *course);
void parse_student_master(char *line, studentmaster *student);


// Function to swap two elements
void swap(studentmaster* a, studentmaster* b)
{
    studentmaster temp = *a;
    *a = *b;
    *b = temp;
}

// Partition function
int partition(studentmaster studentmasterArray[], int low, int high)
{

    // initialize pivot to be the first element
    int pivot = studentmasterArray[low].cgpa*100;
    int i = low;
    int j = high;

    while (i < j) {

        // condition 1: find the first element greater than
        // the pivot (from starting)
        while (studentmasterArray[i].cgpa*100 >= pivot && i <= high - 1) {
            i++;
        }

        // condition 2: find the first element smaller than
        // the pivot (from last)
        while (studentmasterArray[j].cgpa*100 < pivot && j >= low + 1) {
            j--;
        }
        if (i < j) {
            swap(&studentmasterArray[i], &studentmasterArray[j]);
        }
    }
    swap(&studentmasterArray[low], &studentmasterArray[j]);
    return j;
}

// QuickSort function
void quickSort(studentmaster studentmasterArray[], int low, int high)
{
    if (low < high) {

        // call Partition function to find Partition Index
        int partitionIndex = partition(studentmasterArray, low, high);

        // Recursively call quickSort() for left and right
        // half based on partition Index
        quickSort(studentmasterArray, low, partitionIndex - 1);
        quickSort(studentmasterArray, partitionIndex + 1, high);
    }
}

int main()
{
    //Reading and storing students master data to Array
    FILE *studentsFile=fopen("student_master.txt", "r");
    studentmaster studentmasterArray[MAX_STUDENTS];
    char student_line[MAX_LINE_LENGTH];
    int num_students = 0;
    while (fgets(student_line, sizeof(student_line), studentsFile) && num_students <
            MAX_STUDENTS)
    {
        parse_student_master(student_line, &studentmasterArray[num_students++]);
    }
    fclose(studentsFile);
    int n = sizeof(studentmasterArray) / sizeof(studentmasterArray[0]);
     quickSort(studentmasterArray, 0, n - 1);
    //qsort(studentmasterArray, n, sizeof(studentmaster), sortByCgpa);
    printf("\nStudent Details are");
    printf("\nRoll No\t\tFirst Name\t\t\tLast Name\t\tCGPA");
    for (int i = 0; i < MAX_STUDENTS; i++)
    {

        printf("\n%s\t\t%s\t\t\t%s\t\t%1.2lf",
               studentmasterArray[i].roll_no,
               studentmasterArray[i].first_name,
               studentmasterArray[i].last_name,
               studentmasterArray[i].cgpa
              );
    }

    //Reading and storing course master data to Array
    FILE *courseMasterFile=fopen("course_master.txt", "r");
    coursemaster coursemasterArray[MAX_COURSES];
    char line[MAX_LINE_LENGTH];
    int num_courses = 0;
    while (fgets(line, sizeof(line), courseMasterFile) && num_courses <
            MAX_COURSES)
    {
        coursemasterArray[num_courses].wait_list_no=0;
        parse_course_master(line, &coursemasterArray[num_courses++]);
    }
    fclose(courseMasterFile);
    printf("\nCourse Details are");
    printf("\nCourse No\t\tCourse Name\t\t\tFaculty Id\t\tFaculty Name\t\t\tCourse Type\t\tNo. Of Seats");
    for (int i = 0; i < MAX_COURSES; i++)
    {

        printf("\n%s\t\t%s\t\t\t%s\t\t%s\t\t\t%c\t\t%d",
               coursemasterArray[i].course_no,
               coursemasterArray[i].course_name,
               coursemasterArray[i].faculty_id,
               coursemasterArray[i].faculty_name,
               coursemasterArray[i].course_type,
               coursemasterArray[i].no_of_seats
              );
    }

    //Reading and storing the students registered courses data to Array
    FILE *studentCourseRegdFile=fopen("student_course_registered.txt", "r");
    studentregcourse studentregcourseArray[100];
    char studentCourseRegdLine[MAX_LINE_LENGTH];
    int num_studentsregd = 0;
    while (fgets(studentCourseRegdLine, sizeof(studentCourseRegdLine), studentCourseRegdFile))
    {
        parse_course_registered(studentCourseRegdLine, &studentregcourseArray[num_studentsregd++]);
    }
    fclose(studentCourseRegdFile);
    printf("\nRegistered course Details are");
    printf("\nRoll No\t\tCourse No\t\t\Faculty Id\t\t\Course Type");
    for (int i = 0; i < num_studentsregd; i++)
    {

        printf("\n%s\t\t%s\t\t%s\t\t%c",
               studentregcourseArray[i].roll_no,
               studentregcourseArray[i].course_no,
               studentregcourseArray[i].faculty_id,
               studentregcourseArray[i].course_type
              );
    }
    //Logic of Allotment begins
    studentallotedcourse studentAllotArray[num_studentsregd];
    int allotCount=0;
    for (int i = 0; i < MAX_STUDENTS; i++) //loop to iterate student master
    {

        for(int j = 0; j < num_studentsregd; j++)
        {
             /*printf("\ninside 2");
             printf("\ninside 2 studentmasterArray[i].roll_no %s",studentmasterArray[i].roll_no);
              printf("\ninside 2 studentregcourseArray[j].roll_no %s",studentregcourseArray[j].roll_no);
               printf("\ninside 2 allotCount is %d",allotCount);
        printf("\ninside 2 num_studentsregd is %d",num_studentsregd);*/
            if(strcmp(studentmasterArray[i].roll_no,studentregcourseArray[j].roll_no)==0 && allotCount<num_studentsregd)
            {
                //printf("\ninside 3");
                strcpy(studentAllotArray[allotCount].roll_no,studentmasterArray[i].roll_no);
                strcpy(studentAllotArray[allotCount].first_name,studentmasterArray[i].first_name);
                strcpy(studentAllotArray[allotCount].last_name,studentmasterArray[i].last_name);
                strcpy(studentAllotArray[allotCount].course_no,studentregcourseArray[j].course_no);
               studentAllotArray[allotCount].course_type=studentregcourseArray[j].course_type;
                strcpy(studentAllotArray[allotCount].faculty_id,studentregcourseArray[j].faculty_id);
                 //printf("\ninside 3a");
                for(int m=0; m<MAX_COURSES; m++)
                {
                    //printf("\ninside 4");
                    if((strcmp(coursemasterArray[m].course_no,studentregcourseArray[j].course_no)==0) && (strcmp(coursemasterArray[m].faculty_id,studentregcourseArray[j].faculty_id)==0))
                    {
                        // printf("\ninside 5");

                        strcpy(studentAllotArray[allotCount].course_name,coursemasterArray[m].course_name);
                        strcpy(studentAllotArray[allotCount].faculty_name,coursemasterArray[m].faculty_name);
                        if(coursemasterArray[m].no_of_seats >0)
                        {
                            //printf("\ninside 6");
                            studentAllotArray[allotCount].alloted='Y';
                            coursemasterArray[m].no_of_seats-=1;
                            studentAllotArray[allotCount].wait_list_no=0;


                        }
                        else
                        {
                           // printf("\ninside 7");
                            studentAllotArray[allotCount].alloted='N';
                            coursemasterArray[m].wait_list_no+=1;
                            studentAllotArray[allotCount].wait_list_no=coursemasterArray[m].wait_list_no;
                        }
                        break;
                    }

                }

                allotCount+=1;
            }

        }
    }

    printf("\nCourse Allotment Details are:");
    printf("\nRoll No\tCourse No\tFaculty Id\tCourse Type\tAlloted\tWait List");
    for(int i=0; i<num_studentsregd; i++)
    {
        printf("\n%s\t%s\t%s\t%c\t%c\t%d",
               studentAllotArray[i].roll_no,
               studentAllotArray[i].course_no,
               studentAllotArray[i].faculty_id,
               studentAllotArray[i].course_type,
               studentAllotArray[i].alloted,
               studentAllotArray[i].wait_list_no
              );
    }

    //writing course allotment details to file

    FILE* outfile;

    // open file for writing
    outfile = fopen("studentallotdetails.txt", "w");
    if (outfile == NULL) {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    for(int i=0; i<num_studentsregd; i++)
    {
          fprintf(outfile,
                    "%s,%s,%s,%c,%c,%d\n",
                    studentAllotArray[i].roll_no,
               studentAllotArray[i].course_no,
               studentAllotArray[i].faculty_id,
               studentAllotArray[i].course_type,
               studentAllotArray[i].alloted,
               studentAllotArray[i].wait_list_no
               );

            // If there is an error writing to the file we identify that it has
            // occurred using ferror(file) and exit with an error message and status
            // if so.
            if (ferror(outfile))
            {
              printf("Error writing to file.\n");
              return 1;
            }
    }
    // close file
    fclose(outfile);

    // open file for writing
    outfile = fopen("studentallotdetails_count.txt", "w");
        if (outfile == NULL) {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    fprintf(outfile,"%d",num_studentsregd);
    if (ferror(outfile))
    {
        printf("Error writing to file.\n");
        return 1;
    }
    // close file
    fclose(outfile);


    printf("\nCourse Details Post Allocation are");
    printf("\nCourse No\t\tCourse Name\t\t\tFaculty Id\t\tFaculty Name\t\t\tCourse Type\t\tNo. Of Seats");
    for (int i = 0; i < MAX_COURSES; i++)
    {

        printf("\n%s\t\t%s\t\t\t%s\t\t%s\t\t\t%c\t\t%d",
               coursemasterArray[i].course_no,
               coursemasterArray[i].course_name,
               coursemasterArray[i].faculty_id,
               coursemasterArray[i].faculty_name,
               coursemasterArray[i].course_type,
               coursemasterArray[i].no_of_seats
              );
    }

    //writing coursemaster details post allocation

      // open file for writing
    outfile = fopen("coursemaster_postallocation.txt", "w");
    if (outfile == NULL) {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    for (int i = 0; i < MAX_COURSES; i++)
    {
        fprintf(outfile,
                    "%s,%s,%s,%s,%c,%d\n",
                    coursemasterArray[i].course_no,
               coursemasterArray[i].course_name,
               coursemasterArray[i].faculty_id,
               coursemasterArray[i].faculty_name,
               coursemasterArray[i].course_type,
               coursemasterArray[i].no_of_seats
               );

            // If there is an error writing to the file we identify that it has
            // occurred using ferror(file) and exit with an error message and status
            // if so.
            if (ferror(outfile))
            {
              printf("Error writing to file.\n");
              return 1;
            }
    }
    fclose(outfile);
    studentmaster student = { "SSN002", "Stu2 First Name", "Stu2 Last Name",6.0 };
    drop_course(student);

}

int drop_course(studentmaster student)
{
    char course_no_to_drop[10];
    printf("\nEnter the course no to be dropped");
    scanf("%[^\n]%*c", course_no_to_drop);
    printf("\nCourse no to be dropped is %s",course_no_to_drop);

    /*char faculty_id_to_drop[10];
    printf("\nEnter the faculty no mapped to the course to be dropped");
    scanf("%[^\n]%*c", faculty_id_to_drop);
    printf("\nCourse no to be dropped is %s",faculty_id_to_drop);*/
    int num_studentsregd=0;
    FILE *studentallotdetailscountFile=fopen("studentallotdetails_count.txt", "r");
    fscanf(studentallotdetailscountFile, "%d", &num_studentsregd);
    fclose(studentallotdetailscountFile);

    FILE *studentallotdetailsFile=fopen("studentallotdetails.txt", "r");
    studentallotedcourse studentallotedcourseArray[num_studentsregd];
    char student_line[MAX_LINE_LENGTH];
    int num_students = 0;
    while (fgets(student_line, sizeof(student_line), studentallotdetailsFile))
    {
        parse_student_allot(student_line, &studentallotedcourseArray[num_students++]);
    }
    fclose(studentallotdetailsFile);
    printf("\ndrop_course called %d.\n",num_studentsregd);
    for(int i=0; i<num_studentsregd; i++)
    {
        printf("\n%s\t%s\t%s\t%c\t%c\t%d",
               studentallotedcourseArray[i].roll_no,
               studentallotedcourseArray[i].course_no,
               studentallotedcourseArray[i].faculty_id,
               studentallotedcourseArray[i].course_type,
               studentallotedcourseArray[i].alloted,
               studentallotedcourseArray[i].wait_list_no
              );
    }


    //Reading and storing course master data to Array
    FILE *courseMasterFile=fopen("coursemaster_postallocation.txt", "r");
    coursemaster coursemasterArray[MAX_COURSES];
    char line[MAX_LINE_LENGTH];
    int num_courses = 0;
    while (fgets(line, sizeof(line), courseMasterFile) && num_courses <
            MAX_COURSES)
    {
        coursemasterArray[num_courses].wait_list_no=0;
        parse_course_master(line, &coursemasterArray[num_courses++]);
    }
    fclose(courseMasterFile);
    printf("\nCourse Details are");
    printf("\nCourse No\t\tCourse Name\t\t\tFaculty Id\t\tFaculty Name\t\t\tCourse Type\t\tNo. Of Seats");
    for (int i = 0; i < MAX_COURSES; i++)
    {

        printf("\n%s\t\t%s\t\t\t%s\t\t%s\t\t\t%c\t\t%d",
               coursemasterArray[i].course_no,
               coursemasterArray[i].course_name,
               coursemasterArray[i].faculty_id,
               coursemasterArray[i].faculty_name,
               coursemasterArray[i].course_type,
               coursemasterArray[i].no_of_seats
              );
    }
    int courseDropCount=0;
    int coreCourseCount=0;
    int electiveCourseCount=0;
    for(int i=0;i<num_studentsregd; i++)
    {
        if((strcmp(studentallotedcourseArray[i].roll_no,student.roll_no)==0))
        {
            if(studentallotedcourseArray[i].course_type=='C' && studentallotedcourseArray[i].alloted=='Y')
            {
                coreCourseCount+=1;

            } else if(studentallotedcourseArray[i].course_type=='E' && studentallotedcourseArray[i].alloted=='Y')
            {
                electiveCourseCount+=1;

            }
        }



    }
    if(coreCourseCount == 3 && electiveCourseCount == 1)
    {

        printf("\nYou have registered the minimum 3 core and 1 Elective course only and hence could not drop the course");
        return -1;
    }
    int courseDropIndex=0;
    int courseFacultyFound = 0;
    for(int i=0;i<num_studentsregd; i++)
    {
        if((strcmp(studentallotedcourseArray[i].roll_no,student.roll_no)==0) && (strcmp(studentallotedcourseArray[i].course_no,course_no_to_drop)==0))
        {
            courseDropCount+=1;
            courseDropIndex = i;

                courseFacultyFound = 0;
                for(int m=0; m<MAX_COURSES; m++)
                {

                    if((strcmp(studentallotedcourseArray[i].course_no,coursemasterArray[m].course_no)==0) && (strcmp(studentallotedcourseArray[i].faculty_id,coursemasterArray[m].faculty_id)==0) && coursemasterArray[m].no_of_seats>0)
                    {
                        coursemasterArray[m].no_of_seats+=1;
                        courseFacultyFound=1;
                        break;

                    }
                }

                if(courseFacultyFound == 0)
                {
                    for(int k=0;k<num_studentsregd; k++)
                    {
                        if((strcmp(course_no_to_drop,studentallotedcourseArray[k].course_no)==0))
                        {

                               if(studentallotedcourseArray[k].wait_list_no >0)
                               {
                                   if(studentallotedcourseArray[k].wait_list_no == 1)
                                   {
                                       studentallotedcourseArray[k].alloted='Y';
                                   }

                                   studentallotedcourseArray[k].wait_list_no-=1;
                               }

                        }
                    }

                }

            //}

        }

    }

    if(courseDropCount==0)
    {

        printf("\Entered course to be dropped is not alloted for you!");
        return -1;
    }else
    {
        for(int i=courseDropIndex;i<num_studentsregd-1; i++)
        {

            studentallotedcourseArray[i] = studentallotedcourseArray[i+1];
        }

            //writing course allotment details to file
        printf("\nUpdating student course allocation details post dropping the course");
        FILE* outfile;

        // open file for writing
        outfile = fopen("studentallotdetails.txt", "w");
        if (outfile == NULL) {
            fprintf(stderr, "\nError opened file\n");
            exit(1);
        }
        for(int i=0; i<num_studentsregd; i++)
        {
              fprintf(outfile,
                        "%s,%s,%s,%c,%c,%d\n",
                        studentallotedcourseArray[i].roll_no,
                   studentallotedcourseArray[i].course_no,
                   studentallotedcourseArray[i].faculty_id,
                   studentallotedcourseArray[i].course_type,
                   studentallotedcourseArray[i].alloted,
                   studentallotedcourseArray[i].wait_list_no
                   );

                // If there is an error writing to the file we identify that it has
                // occurred using ferror(file) and exit with an error message and status
                // if so.
                if (ferror(outfile))
                {
                  printf("Error writing to file.\n");
                  return 1;
                }
        }
        // close file
        fclose(outfile);

        // open file for writing
        outfile = fopen("studentallotdetails_count.txt", "w");
            if (outfile == NULL) {
            fprintf(stderr, "\nError opened file\n");
            exit(1);
        }
        fprintf(outfile,"%d",num_studentsregd);
        if (ferror(outfile))
        {
            printf("Error writing to file.\n");
            return 1;
        }
        // close file
        fclose(outfile);

        if(courseFacultyFound>0)
        {
             //writing coursemaster_postallocation details post allocation
            printf("\nUpdating coursemaster_postallocation.text post dropping the course");
            // open file for writing
            outfile = fopen("coursemaster_postallocation.txt", "w");
            if (outfile == NULL) {
                fprintf(stderr, "\nError opened file\n");
                exit(1);
            }
            for (int i = 0; i < MAX_COURSES; i++)
            {
                fprintf(outfile,
                            "%s,%s,%s,%s,%c,%d\n",
                            coursemasterArray[i].course_no,
                       coursemasterArray[i].course_name,
                       coursemasterArray[i].faculty_id,
                       coursemasterArray[i].faculty_name,
                       coursemasterArray[i].course_type,
                       coursemasterArray[i].no_of_seats
                       );

                    // If there is an error writing to the file we identify that it has
                    // occurred using ferror(file) and exit with an error message and status
                    // if so.
                    if (ferror(outfile))
                    {
                      printf("Error writing to file.\n");
                      return 1;
                    }
            }
            fclose(outfile);
        }

    }



return 1;




 }


int sortByCgpa(const void* a, const void* b)
{
    return (((studentmaster*)b)->cgpa*100)-(((studentmaster*)a)->cgpa*100);

}

void parse_course_master(char *line, coursemaster *coursemasterObj)
{
    sscanf(line,"%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
           &coursemasterObj->course_no,
           &coursemasterObj->course_name,
           &coursemasterObj->faculty_id,
           &coursemasterObj->faculty_name,
           &coursemasterObj->course_type,
           &coursemasterObj->no_of_seats
          );
}


void parse_student_allot(char *line, studentallotedcourse *studentallotedcourseObj)
{
    sscanf(line,"%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
           &studentallotedcourseObj->roll_no,
           &studentallotedcourseObj->course_no,
           &studentallotedcourseObj->faculty_id,
           &studentallotedcourseObj->course_type,
           &studentallotedcourseObj->alloted,
           &studentallotedcourseObj->wait_list_no
          );
}

void parse_course_registered(char *line, studentregcourse *studentregcourseObj)
{
    sscanf(line,"%[^,],%[^,],%[^,],%c\n",
           &studentregcourseObj->roll_no,
           &studentregcourseObj->course_no,
           &studentregcourseObj->faculty_id,
           &studentregcourseObj->course_type
          );
}

void parse_student_master(char *line, studentmaster *student)
{
    sscanf(line,"%[^,],%[^,],%[^,],%lf\n",
           &student->roll_no,
           &student->first_name,
           &student->last_name,
           &student->cgpa
          );

}

