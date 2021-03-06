#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dataio.h"

#define EAN_MAXLEN 16
#define NAME_MAXLEN 64
#define PERSON_MAXLEN 16
#define ITEMS_MAXCOUNT 512
#define PEOPLE_MAXCOUNT 128
#define BUFSIZE 128

struct item {
    char ean[EAN_MAXLEN];
    char name[NAME_MAXLEN];
    int price;
};

struct item items[ITEMS_MAXCOUNT];
char people[PEOPLE_MAXCOUNT][PERSON_MAXLEN];

int items_count;
int people_count;
int last_item = -1;

char buf[BUFSIZE];

void fill_items()
{
    FILE *f = fopen("items.txt", "r");

    items_count = 0;

    while (fgets(buf, BUFSIZE, f)) {
        int i = 0;
        int begin = 0;

        //Initialize the item
        items[items_count].ean[0] = 0;
        items[items_count].name[0] = 0;
        items[items_count].price = 0;

        //Read the item EAN
        while (i < BUFSIZE) {
            if (buf[i] == ';') {
                buf[i] = 0;
                strcpy(items[items_count].ean, &buf[begin]);
                buf[i] = ';';
                begin = i + 1;
                break;
            }
            i++;
        }

        //Read the item name
        i = begin;
        while (i < BUFSIZE) {
            if (buf[i] == ';') {
                buf[i] = 0;
                strcpy(items[items_count].name, &buf[begin]);
                buf[i] = ';';
                begin = i+1;
                break;
            }
            i++;
        }

        //Use scanf to read the item price
        i = begin;
        if (i < BUFSIZE) {
            sscanf(&buf[i], "%d", &items[items_count].price);
        }

        items_count++;
    }
    fclose(f);
}

void fill_people()
{
    FILE *f = fopen("people.txt", "r");
    people_count = 0;
    while (fgets(people[people_count], PERSON_MAXLEN , f)) {
        char *c = strpbrk(people[people_count], " \t\r\n");
        if (c) *c = 0;
        people[people_count][PERSON_MAXLEN-1] = 0;
        ++people_count;
    }
    fclose(f);
}

void read_input()
{
    int i, balance;
    printf("> ");
    if (!fgets(buf, BUFSIZE, stdin)) return;

    // scan items
    for (i = 0; i < items_count; ++i) {
        if (!strncmp(buf, items[i].ean, strlen(items[i].ean)) && strlen(items[i].ean)+1 == strlen(buf)) {
            if (items[i].price == 30000) {
                last_item = -2;
                printf("\n%s\n\n", items[i].name);
            } else
            if (items[i].price == 0) {
                last_item = -1;
                printf("\n%s\n\n", items[i].name);
            } else {
                last_item = i;
                printf("\n%s    %d Kc\n\n", items[i].name, abs(items[i].price));
            }
            return;
        }
    }

    // scan people
    for (i = 0; i < people_count; ++i) {
        if (!strncmp( buf, people[i], 3 )) {
            printf("\nMember %s ", people[i]);
            if (last_item == -1) {
                balance = modify_credit(people[i], 0);
                printf("has %d Kc.\n\n", balance);
            } else
            if (last_item == -2) {
                balance = modify_credit(people[i], 0);
                modify_credit(people[i], -balance);
                printf("has credit set to 0 Kc.\n\n");
            } else {
                balance = modify_credit(people[i], items[last_item].price);
                printf("has ordered %s for %d Kc and now has %d Kc.\n\n", items[last_item].name, abs(items[last_item].price), balance);
                last_item = -1;
            }
            return;
        }
    }

    // error
    printf("\nUnknown code %s\n", buf);
}

int main()
{
    fill_items();
    fill_people();
    for (;;) {
        read_input();
    }
    return 0;
}
