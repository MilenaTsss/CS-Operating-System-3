//
// Created by milena on 22.05.2023.
//

#ifndef OSI3__WAR_H_
#define OSI3__WAR_H_

#include <time.h>

int num_of_guns;
int field_size;

#define EMPTY_POINT 0
#define USED_POINT 1
#define ALIVE_GUN 2
#define DEAD_GUN 3
#define FINISH 4

typedef struct {
    int type;
    int target_coordinate;
} point_t;

point_t *own_field = NULL;
point_t *foreign_field = NULL;

char *get_point_type(int type) {
    switch (type) {
        case 0:
            return "empty";
        case 1:
            return "used";
        case 2:
            return "alive gun";
        default:
            return "dead gun";
    }
}

char get_point_char(int type) {
    switch (type) {
        case 0:
            return '*';
        case 1:
            return 'x';
        case 2:
            return 'G';
        default:
            return 'D';
    }
}

int check_status() {
    int result = 0;
    for (int i = 0; i < field_size * field_size; ++i) {
        if (own_field[i].type == ALIVE_GUN) {
            result++;
        }
    }
    return result != 0;
}

void generate_targets(int** shots) {
    int j = 0;
    for (int i = 0; i < field_size * field_size; ++i) {
        if (own_field[i].type == ALIVE_GUN) {
            int num = (int) rand() % (field_size * field_size);
            while (foreign_field[num].type == USED_POINT) {
                num = rand() % (field_size * field_size);
            }
            own_field[i].target_coordinate = num;
            *shots[j] = num;
            j++;
        }
    }
}

void simple_fill_field(point_t *field) {
    for (int i = 0; i < field_size * field_size; ++i) {
        field[i].type = EMPTY_POINT;
        field[i].target_coordinate = -1;
    }
}

void fill_field(point_t *field) {
    simple_fill_field(field);

    for (int i = 0; i < num_of_guns; ++i) {
        long num = rand() % (field_size * field_size);// NOLINT(cert-msc50-cpp)
        while (field[num].type != EMPTY_POINT) {
            num = rand() % (field_size * field_size);// NOLINT(cert-msc50-cpp)
        }
        field[num].type = ALIVE_GUN;
    }
}

#endif//OSI3__WAR_H_
