#ifndef C5_S21_DECIMAL_1_S21_DECIMAL_H
#define C5_S21_DECIMAL_1_S21_DECIMAL_H
#define NEGATIVE 0x80000000  // 2^32
#define POSITIVE 0
#define MAX_FLOAT_TO_CONVERT 79228157791897854723898736640.0f
#define MIN_FLOAT_TO_CONVERT \
  0.00000000000000000000000000010000000031710768509710513471352647538147514756461109f

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int bits[4];
} s21_decimal;

typedef struct {
  unsigned int bits[7];
  //  char scale;
} s21_big_num;

int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);


// Helpers
int s21_get_sign(s21_decimal *d);
void s21_set_sign(s21_decimal *d, int sign);
int s21_get_pow(s21_decimal *d);
void s21_set_pow(s21_decimal *d, int power);
int s21_bit(int num, int idx);
void s21_set_bit(int *num, int idx, int value);
int s21_cmp(s21_decimal *a, s21_decimal *b);
int s21_add_handler(s21_decimal *value_1, s21_decimal *value_2);
int s21_sub_handler(s21_decimal *value_1, s21_decimal *value_2);
int s21_mul_handler(s21_decimal *value_1, s21_decimal *value_2);
int s21_div_handler(s21_decimal *value_1, s21_decimal *value_2);
int s21_get_exp_from_string(char *str);
void s21_float_string_to_decimal(char *str, s21_decimal *res);

#endif  // C5_S21_DECIMAL_1_S21_DECIMAL_H