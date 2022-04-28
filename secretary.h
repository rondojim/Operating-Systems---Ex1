#include "common_types.h"
#include "ADTList.h"
#include "ADTMap.h"

struct student{
    int sdi, zip, year;
    String lastname, firstname;
    float gpa;
};

struct count_struct{
    int year, number;
};

typedef struct count_struct* Pair;

typedef struct student* Student;

void sec_init();

void sec_insert(int sdi, String lastname, String firstname, int zip, int year, float gpa);

void sec_lookup(int sdi);

void sec_delete(int sdi);

int sec_number(int year);

List sec_top(int year, int num);

void sec_average(int year);

List sec_minimum(int year);

List sec_count();

List sec_postal(int rank);

void sec_exit();

