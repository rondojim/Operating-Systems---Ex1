#include "ADTList.h"
#include "ADTMap.h"
#include "common_types.h"
#include "secretary.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    sec_init();
    int sdi, zip, year;
    String lastname = malloc(sizeof(*lastname)), firstname = malloc(sizeof(*firstname));
    float gpa;
    if(argc != 0){
        FILE *fp = NULL;
        if(argv[1][1] == 'i')
            fp = fopen(argv[2], "r");
        else if(argc == 5)
            fp = fopen(argv[4], "r");
    
        if(fp != NULL){
            while(fscanf(fp, "%d", &sdi) != EOF){
                fscanf(fp, "%s %s %d %d %f", lastname, firstname, &zip, &year, &gpa);
                sec_insert(sdi, lastname, firstname, zip, year, gpa);
            }
            fclose(fp);
        }
    }
    String inp = malloc(sizeof(*inp));
    while(scanf("%s", inp) != EOF){
        char c = inp[0];
        if(c == 'i'){
            scanf("%d %s %s %d %d %f", &sdi, lastname, firstname, &zip, &year, &gpa);
            sec_insert(sdi, lastname, firstname, zip, year, gpa);
        }
        else if(c == 'l'){
            scanf("%d", &sdi);
            sec_lookup(sdi);
        }
        else if(c == 'd'){
            scanf("%d", &sdi);
            sec_delete(sdi);
        }
        else if(c == 'n'){
            scanf("%d", &year);
            printf("%d student(s) in %d\n", sec_number(year), year);
        } 
        else if(c == 't'){
            scanf("%d %d", &zip, &year);
            List lst = sec_top(zip, year);
            if(list_size(lst) == 0)
                printf("No students enrolled in %d", year);
            for(ListNode nd = list_first(lst); nd != LIST_EOF; nd = list_next(lst, nd))
                printf("%d ", *(int*)list_node_value(lst, nd));
            printf("\n");
        }
        else if(c == 'a'){
            scanf("%d", &year);
            sec_average(year);
        }
        else if(c == 'm'){
            scanf("%d", &year);
            List lst = sec_minimum(year);
            if(list_size(lst))
                printf("No students enrolled in %d", year);
            for(ListNode nd = list_first(lst); nd != LIST_EOF; nd = list_next(lst, nd))
                printf("%d ", *(int*)list_node_value(lst, nd));
            printf("\n");
        }
        else if(c == 'c'){
            List lst = sec_count();
            if(list_size(lst) == 0)
                printf("No students enrolled in %d\n", year);
            for(ListNode nd = list_first(lst); nd != LIST_EOF; nd = list_next(lst, nd)){
                Pair cur = (Pair)list_node_value(lst, nd);
                printf("%d %d\n", cur->year, cur->number);
            }
        }
        else if(c == 'p'){
            scanf("%d", &zip);
            List lst = sec_postal(zip);
            if(list_size(lst) == 0)
                printf("No students enrolled in %d", year);
            for(ListNode nd = list_first(lst); nd != LIST_EOF; nd = list_next(lst, nd))
                printf("%d ", *(int*)list_node_value(lst, nd));
            printf("\n");
        }
        else{
            sec_exit();
            break;
        }
    }
    return 0;
}