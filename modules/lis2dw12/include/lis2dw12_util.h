#ifndef LIS2DW12_UTIL_H_
#define LIS2DW12_UTIL_H_

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "lis2dw12.h"

struct accelerometer {
  float x_mg;
  float y_mg;
  float z_mg;
  float t_c;
};

typedef struct accelerometer accelerometer;

struct rotation_matrix {
  float pitch;
  float roll;
  float yaw;
};

typedef struct rotation_matrix rotation_matrix;

void rotation_matrix_print(rotation_matrix *rotation_matrix) {
  printf("[ROTATION] PITCH, ROLL, YAW [°]: % 3.2f, % 3.2f, % 3.2f\n",
         rotation_matrix->pitch, rotation_matrix->roll, rotation_matrix->yaw);
}

void accelerometer_print(accelerometer *acc) {
  printf("[ACCELEROMETER] X, Y, Z [mg]: %4.2f, %4.2f, %4.2f\n", acc->x_mg,
         acc->y_mg, acc->z_mg);
}

float accelerometer_magnitude(accelerometer *acc_data) {
  return sqrt(pow(acc_data->x_mg, 2) + pow(acc_data->y_mg, 2) +
              pow(acc_data->z_mg, 2));
}

_Bool variance_error(accelerometer *acc_mean, accelerometer *acc, int acc_size,
                     float variance) {
  for (int i = 0; i < acc_size; i++) {
    if (fabs(accelerometer_magnitude(acc) - accelerometer_magnitude(acc_mean)) >
        variance) {
      return true;
    }
  }
  return false;
}

void acceleration_mean(accelerometer *acc_mean, accelerometer *acc_data,
                       int acc_data_lenght) {
  for (int i = 0; i < acc_data_lenght; i++) {
    acc_mean->x_mg += acc_data[i].x_mg / acc_data_lenght;
    acc_mean->y_mg += acc_data[i].y_mg / acc_data_lenght;
    acc_mean->z_mg += acc_data[i].z_mg / acc_data_lenght;
  }
}

void calculate_rotation(rotation_matrix *rot_matrix, accelerometer acc,
                        float g_total) {
#define M_PI (3.14159265358979323846)
  float pitch_ = acc.y_mg / g_total;
  float roll_ = acc.x_mg / g_total;
  float yaw_ = acc.z_mg / g_total;

  rot_matrix->pitch = asin(pitch_) * 180 / M_PI;
  rot_matrix->roll = asin(roll_) * 180 / M_PI;
  rot_matrix->yaw = asin(yaw_) * 180 / M_PI;
}

void read_accelerometer(rotation_matrix *rotation_matrix) {
#define WINDOW 5
#define VARIANCE 7
#define MAX_RETRY 5

  accelerometer data[WINDOW];

  accelerometer acc_mean = {.x_mg = 0, .y_mg = 0, .z_mg = 0};
  float g_force = 0;

  _Bool ok_variance = false;

  int retry = 0;

  do {
    for (int i = 0; i < WINDOW; i++) {
      if (lis2dw12_read(&(data[i].x_mg), &(data[i].y_mg), &(data[i].z_mg), &(data[i].t_c))) {
        puts("ERROR: reading acceleration");
        return;
      }
    }

    acceleration_mean(&acc_mean, data, WINDOW);

    for (int i = 0; i < WINDOW; i++) {
      printf("Acceleration [mg]: %4.2f, %4.2f, %4.2f\n", data[i].x_mg,
             data[i].y_mg, data[i].z_mg);
    }

    g_force = accelerometer_magnitude(&acc_mean);

    printf("Acceleration MEAN [mg]: %4.2f, %4.2f, %4.2f, %4.2f\n",
           acc_mean.x_mg, acc_mean.y_mg, acc_mean.z_mg, g_force);

    if (variance_error(&acc_mean, data, WINDOW, VARIANCE)) {
      ok_variance = false;

      acc_mean.x_mg = 0;
      acc_mean.y_mg = 0;
      acc_mean.z_mg = 0;
      g_force = 0;
      retry += 1;
    } else {
      ok_variance = true;
    }

  } while (!ok_variance && retry < MAX_RETRY);

  calculate_rotation(rotation_matrix, acc_mean, g_force);

  rotation_matrix_print(rotation_matrix);

  return;
}

#endif  // LIS2DW12_UTIL_H_
