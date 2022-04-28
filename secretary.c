#include "ADTList.h"
#include "ADTMap.h"
#include "secretary.h"
#include "common_types.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int* create_int(int x){
    int *p = malloc(sizeof(int));
    *p = x;
    return p;
}

static int compare_ints(Pointer a, Pointer b){
    return *(int*)a - *(int*)b;
}

struct monitor{
    Map id_to_student;
    List year_to_students;
    Map zip_to_num;
};

static int compare(Pointer a, Pointer b){
    Student sa = (Student)a, sb = (Student)b;
    if(sa->sdi != sb->sdi) return -1;
    return 0;
}

typedef struct monitor* Monitor;

static Monitor monitor;

void zip_add(int zip, int v){
    int *pzip = create_int(zip);
    int *p = (int*)map_find(monitor->zip_to_num, pzip);
    int newval = 0;
    if(p != NULL) newval = *p;
    newval += v;

    map_remove(monitor->zip_to_num, pzip);
    map_insert(monitor->zip_to_num, pzip, create_int(newval));
}

void sec_init(){
    monitor = malloc(sizeof(*monitor));
    monitor->id_to_student = map_create(compare_ints, free, free);
    map_set_hash_function(monitor->id_to_student, hash_int);

    monitor->year_to_students = list_create(free);

    monitor->zip_to_num = map_create(compare_ints, free, free);
    map_set_hash_function(monitor->zip_to_num, hash_int);
}

void sec_insert(int sdi, String lastname, String firstname, int zip, int year, float gpa){
    int *cid = create_int(sdi);
    if(map_find(monitor->id_to_student, cid) != NULL){
        printf("Student %d exists\n", sdi);
        return;
    }
    Student cur = malloc(sizeof(*cur));
    cur->lastname = malloc(sizeof(*cur->lastname)), cur->firstname = malloc(sizeof(*cur->firstname));
    cur->sdi = sdi, cur->zip = zip, cur->year = year, cur->gpa = gpa;
    strcpy(cur->lastname, lastname);
    strcpy(cur->firstname, firstname);

    map_insert(monitor->id_to_student, cid, cur);
    

    int ok = false;
    for(ListNode nd = list_first(monitor->year_to_students); nd != LIST_EOF; nd = list_next(monitor->year_to_students, nd)){
        List lst = list_node_value(monitor->year_to_students, nd);
        Student std = (Student)list_node_value(lst, list_first(lst));
        if(std->year == year){
            list_insert_next(lst, list_last(lst), cur);
            ok = true;
            break;
        }
    }
    if(ok == false){
        List lst = list_create(free);
        list_insert_next(lst, list_last(lst), cur);
        list_insert_next(monitor->year_to_students, list_last(monitor->year_to_students), lst);
    }
    zip_add(cur->zip, +1);

    printf("Student %d inserted\n", sdi);
}

void sec_lookup(int sdi){
    Student cur = (Student)map_find(monitor->id_to_student, create_int(sdi));
    if(cur == NULL){
        printf("Student %d does not exist\n", sdi);
        return;
    }
    printf("%d %s %s %d %d %f\n", cur->sdi, cur->lastname, cur->firstname, cur->zip, cur->year, cur->gpa);
}

void sec_delete(int sdi){
    int *cid = create_int(sdi);
    Student cur = (Student)map_find(monitor->id_to_student, cid);
    
    if(cur == NULL){
        printf("Student %d does not exist\n", sdi);
        return;
    }

    zip_add(cur->zip, -1);
    map_remove(monitor->id_to_student, cur);

    for(ListNode nd = list_first(monitor->year_to_students); nd != LIST_EOF; nd = list_next(monitor->year_to_students, nd)){
        List lst = (List)list_node_value(monitor->year_to_students, nd);
        Student std = (Student)list_node_value(lst, list_first(lst));
        if(std->year == cur->year){
            list_remove(lst, list_find_node(lst, cur, compare));
            break;
        }
    }

    printf("Record %d deleted\n", sdi);
}

int sec_number(int year){
    for(ListNode nd = list_first(monitor->year_to_students); nd != LIST_EOF; nd = list_next(monitor->year_to_students, nd)){
        List lst = (List)list_node_value(monitor->year_to_students, nd);
        Student std = (Student)list_node_value(lst, list_first(lst));
        if(std->year == year)
            return list_size(lst);
    }
    return 0;
}

void sec_average(int year){
    float avg = 0;
    for(ListNode nd = list_first(monitor->year_to_students); nd != LIST_EOF; nd = list_next(monitor->year_to_students, nd)){
        List lst = (List)list_node_value(monitor->year_to_students, nd);
        Student std = (Student)list_node_value(lst, list_first(lst));
        if(std->year == year){
            for(ListNode snd = list_first(lst); snd != LIST_EOF; snd = list_next(lst, snd)){
                std = (Student)list_node_value(lst, snd);
                avg += std->gpa;
            }
            avg /= list_size(lst);
            break;
        }
    }
    if(avg == 0) 
        printf("No students enrolled in %d\n", year);
    else
        printf("%lf\n", avg);
}

List sec_top(int num, int year){
    List ranked = list_create(free);
    for(ListNode nd = list_first(monitor->year_to_students); nd != LIST_EOF; nd = list_next(monitor->year_to_students, nd)){
        List lst = (List)list_node_value(monitor->year_to_students, nd);
        Student std = (Student)list_node_value(lst, list_first(lst));
        if(std->year == year){
            for(ListNode tp = list_first(lst); tp != LIST_EOF; tp = list_next(lst, tp)){
                std = (Student)list_node_value(lst, tp);
                ListNode prv = LIST_EOF;
                Student cur;
                for(ListNode cnd = list_first(ranked); cnd != LIST_EOF; cnd = list_next(ranked, cnd)){
                    cur = (Student)list_node_value(ranked, cnd);
                    if(std->gpa > cur->gpa) break;
                    prv = cnd;
                }
                list_insert_next(ranked, prv, std);
            }
            break;
        }
    }
    List result = list_create(free);
    int i = 0;
    for(ListNode nd = list_first(ranked); nd != LIST_EOF; nd = list_next(ranked, nd)){
        i++;
        if(i > num) break;
        Student std = (Student)list_node_value(ranked, nd);
        list_insert_next(result, list_last(result), create_int(std->sdi));
    }
    return result;
}

List sec_minimum(int year){
    List result = list_create(free);
    for(ListNode nd = list_first(monitor->year_to_students); nd != LIST_EOF; nd = list_next(monitor->year_to_students, nd)){
        List lst = (List)list_node_value(monitor->year_to_students, nd);
        Student std = (Student)list_node_value(lst, list_first(lst));
        if(std->year == year){
            float minimum = 9999999;
            for(ListNode snd = list_first(lst); snd != LIST_EOF; snd = list_next(lst, snd)){
                std = (Student)list_node_value(lst, snd);
                if(minimum > std->gpa)
                    minimum = std->gpa;
            }
            for(ListNode snd = list_first(lst); snd != LIST_EOF; snd = list_next(lst, snd)){
                std = (Student)list_node_value(lst, snd);
                if(std->gpa == minimum)
                    list_insert_next(result, list_last(result), create_int(std->sdi));
            }
        }
    }
    return result;
}

List sec_count(){
    List result = list_create(free);
    for(ListNode nd = list_first(monitor->year_to_students); nd != LIST_EOF; nd = list_next(monitor->year_to_students, nd)){
        List lst = (List)list_node_value(monitor->year_to_students, nd);
        Student std = (Student)list_node_value(lst, list_first(lst));

        int year = std->year, number = list_size(lst);

        Pair cur = malloc(sizeof(*cur));
        cur->year = year, cur->number = number;
        
        list_insert_next(result, list_last(result), cur);
    }
    return result;
}

List sec_postal(int rank){
    List lst = list_create(free);
    for(MapNode nd = map_first(monitor->zip_to_num); nd != MAP_EOF; nd = map_next(monitor->zip_to_num, nd)){
        int zip = *(int*)map_node_key(monitor->zip_to_num, nd);
        int num = *(int*)map_node_value(monitor->zip_to_num, nd);
        if(num == 0) continue;
        ListNode prv = LIST_EOF;
        Pair cur;
        for(ListNode pt = list_first(lst); pt != LIST_EOF; pt = list_next(lst, pt)){
            cur = (Pair)list_node_value(lst, pt);
            if(num > cur->number){
                cur->year = zip, cur->number = num;
                break;
            }
            prv = pt;
        }
        cur = malloc(sizeof(*cur));
        cur->number = num, cur->year = zip;
        list_insert_next(lst, prv, cur);
    }
    int number = -1, pr = -1, cnt = 0;
    for(ListNode nd = list_first(lst); nd != LIST_EOF; nd = list_next(lst, nd)){
        Pair cur = (Pair)list_node_value(lst, nd);
        if(pr == cur->number) continue;
        pr = cur->number, cnt++;
        if(cnt == rank){
            number = cur->number;
            break;
        }
    }
    List result = list_create(free);
    for(ListNode nd = list_first(lst); nd != LIST_EOF; nd = list_next(lst, nd)){
        Pair cur = (Pair)list_node_value(lst, nd);
        if(cur->number == number)
            list_insert_next(result, list_last(result), create_int(cur->year));
    }
    return result;
}

void sec_exit(){
    map_destroy(monitor->id_to_student);
    list_destroy(monitor->year_to_students);
    map_destroy(monitor->zip_to_num);
    free(monitor);
}

