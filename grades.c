#include <stddef.h>  /* size_t type */
#include <stdio.h>   /* printf etc. */
#include <stdlib.h>  /* malloc etc. */
#include <stdbool.h> /* bool type */
#include <string.h>  /**< Include strcpy etc. */
#include "linked-list.h"
#include "grades.h"
#define MAX_SCORE 100
#define MIN_SCORE 0
enum {INITIALE_NAME_LENGTH = 100};
/**
 * @struct grades
 * @brief Represents a collection of students, each one holds his courses 
 *        & grades.
 */
struct grades {
    struct list *students_list;
};
/**
 * @struct student
 * @brief Represents a student with their name, ID, and courses w/ grades.
 */
struct student{
    char *name;
    int id;
    struct list *student_courses_list;
};
/**
 * @struct course
 * @brief Represents a course with its name and grade.
 */
struct course {
    char *name;
    int grade;
};

/**
 * @brief Clones a course.
 * 
 * @param element The course to be cloned.
 * @param output Pointer to the cloned course.
 * @return int 0 if successful, otherwise 1.
 */
int course_clone (void *element,void **output);
/**
 * @brief Destroys a course.
 * 
 * @param element The course to be destroyed.
 */
void course_destroy (void *element);
/**
 * @brief Clones a student.
 * 
 * @param element The student to be cloned.
 * @param output Pointer to the cloned student.
 * @return int 0 if successful, otherwise 1.
 */
int student_clone (void *element,void **output);
/**
 * @brief Destroys a student.
 * 
 * @param element The student to be destroyed.
 */
void student_destroy (void *element);
/**
 * @brief Finds a student in a list based on ID.
 * 
 * @param grades_it Iterator over the list of students.
 * @param student Pointer to store the found student.
 * @param id ID of the student to find.
 * @return int 0 if found, otherwise 1.
 * @note changes student's pointer to the student with the id if found
 *       if not - to the last course in the list
 */
static int find_student_on_list (struct iterator *grades_it, 
                                 struct student **student, int id);

struct grades* grades_init() {
    struct grades *grades = (struct grades *) malloc(sizeof(struct grades));
    if (grades == NULL){
        return NULL;
    }
    grades->students_list = list_init(student_clone, student_destroy);
    if (grades->students_list == NULL) {
        free (grades);
        return NULL;
    }
    return grades;

    /* since list_init will reutrn an empty list 
    and list_begin will return NULL anyway if the list is empty  */
}

void grades_destroy (struct grades *grades) {
    if (grades == NULL) {
        return;
    }
    /* struct iterator *grades_it = list_begin(grades->students_list); */
    /* while (grades_it != NULL){ */
    /*     void *student = list_get(grades_it); */
    /*      Maybe not need to check */ 
    /*     if (student != NULL) { */
    /*         student_destroy (student); */
    /*     } */
    /*     grades_it = list_next(grades_it);  */
    /* } */
    /* free (grades_it); */
    list_destroy(grades->students_list);
    free (grades);
}

int grades_add_student(struct grades *grades, const char *name, int id) {
    if (grades == NULL) {
        return 1;
    }
    struct iterator *grades_it = list_begin(grades->students_list);
    struct student *student = (struct student *) list_get(grades_it);
    int id_existance = find_student_on_list(grades_it,&student,id);
    if (id_existance == 0) {
        return 1;
    } 
    struct student *new_student = (struct student *) \
    malloc(sizeof(struct student));
    if (new_student == NULL){
        return 1;
    }
    new_student->name = (char *) malloc(sizeof(char)* (strlen(name)+1));
    if (new_student->name == NULL) {
        free (new_student);
        return 1;
    }
    new_student->id = id;
    strcpy(new_student->name, name);
    if (new_student->name == NULL) {
        free(new_student);
        return 1;
    }
    new_student->student_courses_list = list_init(course_clone,course_destroy);
    /* list_push_back gets a void ptr - need casting */
    list_push_back(grades->students_list, (void *) new_student);
    /* free because "list" holds a copy of "element" (clone)
    therefor no excess memory needed */
    free (new_student->name);
    list_destroy(new_student->student_courses_list);
    free (new_student);
    return 0;
}
int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade){
    if (grades ==  NULL){
        return 1;
    }
    if (grade > MAX_SCORE || grade < MIN_SCORE) {
        return 1;
    }
    struct iterator *grades_it = list_begin(grades->students_list);
    struct student *student = (struct student *) list_get(grades_it);
    if (find_student_on_list(grades_it,&student,id) == 1) {
        return 1;
    }
    struct iterator *course_it = list_begin(student->student_courses_list);
    struct course *current_course;
    while (course_it != NULL) {
        current_course = (struct course *) list_get(course_it);
        if (!strcmp(current_course->name, name)){
            return 1;
        }
        course_it = list_next(course_it);
    }
    struct course *new_course = (struct course *) \
    malloc(sizeof(struct course));
    if (new_course == NULL) {
        return 1;
    }
    new_course->name = (char *) malloc(sizeof(char)* (strlen(name)+1));
    if (new_course->name == NULL) {
        free (new_course);
        return 1;
    }
    new_course->grade = grade;
    strcpy(new_course->name, name);
    if (new_course->name == NULL) {
        free (new_course);
        return 1;
    }
    /* list_push_back gets a void ptr - need casting */
    list_push_back(student->student_courses_list, (void *) new_course);
    free (new_course->name);
    free (new_course);
    return 0;
}

float grades_calc_avg(struct grades *grades, int id, char **out){
    if (grades == NULL) {
        out = NULL;
        return -1;
    }
    struct iterator *grades_it = list_begin(grades->students_list);
    struct student *student = (struct student *) list_get(grades_it);
    if (find_student_on_list(grades_it,&student,id) == 1) {
        out = NULL;
        return -1;
    }
    /* allocate memory for **out */
    char *current_name = (char *) malloc(sizeof(char)*(strlen(student->name)+1));
    /* copy the student's name */
    strcpy(current_name,student->name);
    /* make out point to current_name address */
    *out = current_name; // why out = &current_name not working?
    /* In case there are no courses in the student's list */
    if (list_size(student->student_courses_list) == 0){
        return 0;
    }
    float avg = 0;
    struct iterator *course_it = list_begin(student->student_courses_list);
    while (course_it != NULL) {
        struct course *current_course = (struct course *) list_get(course_it);
        avg += current_course->grade;
        course_it = list_next(course_it);
    }
    return (avg / list_size(student->student_courses_list));
}

int grades_print_student(struct grades *grades, int id) {
    if (grades == NULL) {
        return 1;
    }
    struct iterator *grades_it = list_begin(grades->students_list);
    struct student *student = (struct student *) list_get(grades_it);
    if (find_student_on_list(grades_it,&student,id) == 1) {
        return 1;
    }
    struct iterator *course_it = list_begin(student->student_courses_list);
    printf ("%s %d:%s", student->name, student->id, \
    (course_it == NULL) ? "" : " ");
    if (course_it == NULL) {
        printf ("\n");
    }
    while (course_it != NULL) {
        struct course *current_course = (struct course *) list_get(course_it);
        printf ("%s %d%s", current_course->name, current_course->grade,\
        (list_next(course_it) == NULL) ? "\n" : ", ");
        course_it = list_next(course_it);
    }
    return 0;
}

int grades_print_all(struct grades *grades) {
    if (grades == NULL) {
    return 1;
    }
    struct iterator *grades_it = list_begin(grades->students_list);
    struct student *student;
    while (grades_it != NULL) {
        student = (struct student *) list_get(grades_it);
        grades_print_student(grades, student->id);
        grades_it = list_next(grades_it);
    }
    return 0;
}

static int find_student_on_list (struct iterator *grades_it, 
                                 struct student **student, int id){
    
    while (grades_it != NULL) {
        *student = (struct student *) list_get(grades_it);
        if ((*student)->id == id) {
            /* In that case *student points to the matching student  */
            break;
        }
        grades_it = list_next(grades_it);
    }
    /* Means we got to the end of the list without anyone with this id */
    if (grades_it == NULL) {
        return 1;
    }
    return 0;
}

int course_clone (void *element,void **output){
    struct course *input_course = (struct course *) element;
    struct course *cloned_course = (struct course *) \
    malloc(sizeof(struct course));
    if (cloned_course == NULL) {
        return 1;
    }
    cloned_course->name = (char *) \
    malloc(sizeof(char)* (strlen(input_course->name)+1));
    if (cloned_course->name == NULL) {
        free (cloned_course);
        return 1;
    }
    strcpy(cloned_course->name,input_course->name);
    if (cloned_course->name == NULL) {
        return 1;
    }
    cloned_course->grade = input_course->grade;
    *output = cloned_course;
    return 0;
};

void course_destroy (void *element) {
    struct course *input_course = (struct course *) element;
    if (input_course->name != NULL){
        free (input_course->name);
    }
    if (input_course != NULL){
        free (input_course);
    }
}

int student_clone (void *element,void **output) {
    struct student *input_student = (struct student *)element;
    struct student *cloned_student = (struct student *) \
    malloc(sizeof(struct student));
    if (cloned_student == NULL) {
        /* error */
        return 1;
    }
    cloned_student->name = (char *) \
    malloc(sizeof(char)* (strlen(input_student->name)+1));
    if (cloned_student->name == NULL) {
        free (cloned_student);
        return 1;
    }   
    strcpy(cloned_student->name,input_student->name);
    if (cloned_student->name == NULL) {
        free (cloned_student);
        return 1;
    }
    cloned_student->id = input_student->id;
    cloned_student->student_courses_list = list_init(course_clone,\
    course_destroy);
    if (cloned_student->student_courses_list == NULL) {
        free (cloned_student->name);
        free (cloned_student);
        return 1;
    }
    struct iterator *student_course_it = \
    list_begin(input_student->student_courses_list);
    while (student_course_it != NULL) {
        /* get the elements from input_student's courses list */
        /* returns void* so casting is needed */
        struct course *course = (struct course *) list_get(student_course_it);
        if (course != NULL) {
            struct course *cloned_course = NULL;
            /* cloning a single course failed */
            if (course_clone(course, (void **)&cloned_course) != 0) {
                list_destroy(cloned_student->student_courses_list);
                free (cloned_student->name);
                free (cloned_student);
                return 1;    
            }
            /* If course_clone succeeded we wish to push in out list
            this element*/
            list_push_back(cloned_student->student_courses_list,cloned_course);
        }
        student_course_it = list_next(student_course_it);
    }
    *output = cloned_student;
    return 0;
}

void student_destroy (void *element) {
    struct student *input_student = (struct student *)element;
    if (input_student->name != NULL) {
        free (input_student->name);
    }
    /* list_begin(input_student->student_courses_list) */
    if (input_student->student_courses_list != NULL) {
        list_destroy (input_student->student_courses_list);
    }
    if (input_student != NULL) {
       free (input_student);
    }
}