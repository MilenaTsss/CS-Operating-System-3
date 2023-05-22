//
// Created by milena on 22.05.2023.
//

#ifndef OS3__WAR_H_
#define OS3__WAR_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EMPTY_POINT 0
#define USED_POINT 1
#define ALIVE_GUN 2
#define DEAD_GUN 3

typedef struct {
    int type;
    int target_coordinate;
} point_t;

int num_of_guns;
int field_size;
point_t *own_field = NULL;
point_t *foreign_field = NULL;

int check_status() {
    int result = 0;
    for (int i = 0; i < field_size * field_size; ++i) {
        if (own_field[i].type == ALIVE_GUN) {
            result++;
        }
    }
    return result != 0;
}

int generate_targets(int *shots) {
    int num_of_shots = 0;
    for (int i = 0; i < field_size * field_size; ++i) {
        if (own_field[i].type == ALIVE_GUN) {
            int num = (int) rand() % (field_size * field_size);
            while (foreign_field[num].type == USED_POINT) {
                num = rand() % (field_size * field_size);
            }
            own_field[i].target_coordinate = num;
            foreign_field[num].type = USED_POINT;
            shots[num_of_shots] = num;
            num_of_shots++;
        }
    }
    return num_of_shots;
}

void simple_fill_field(point_t *field) {
    for (int i = 0; i < field_size * field_size; ++i) {
        field[i].type = EMPTY_POINT;
        field[i].target_coordinate = -1;
    }
}

void fill_field(point_t *field) {
    simple_fill_field(field);
    printf("Preparing shoot into: ");

    for (int i = 0; i < num_of_guns; ++i) {
        int num = rand() % (field_size * field_size);
        while (field[num].type != EMPTY_POINT) {
            num = rand() % (field_size * field_size);
        }
        field[num].type = ALIVE_GUN;
        printf("%d ", num);
    }
    printf("\n");
}

#endif// OS3__WAR_H_
