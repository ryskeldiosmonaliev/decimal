#include"s21_decimal.h"

int s21_cmp(s21_decimal *a, s21_decimal *b) {
  int result;
  int sign1 = s21_get_sign(a);
  int sign2 = s21_get_sign(b);
  if (a->bits[0] == 0 && a->bits[1] == 0 && a->bits[2] == 0 &&
      b->bits[0] == 0 && b->bits[1] == 0 && b->bits[2] == 0)
    result = 0;
  else if (sign1 == 1 && sign2 == 0)  // -+
    result = -1;
  else if (sign1 == 0 && sign2 == 1)  // +-
    result = 1;
  else {
    s21_big_num ba, bb;
    s21_from_decimal_to_big_num(a, &ba);
    s21_from_decimal_to_big_num(b, &bb);
    if (a->bits[3] != b->bits[3]) {
      int pow1 = s21_get_pow(a);
      int pow2 = s21_get_pow(b);
      if (pow1 < pow2)
        s21_bn_mul_10_n_times(&ba, pow2 - pow1);
      else if (pow1 > pow2)
        s21_bn_mul_10_n_times(&bb, pow1 - pow2);
    }
    result = s21_bn_comp_num(&ba, &bb);
    if (sign1 == 1 && sign2 == 1)  // --
      result *= -1;
  }
  return result;
}

int s21_bn_comp_num(s21_big_num *ba, s21_big_num *bb) {
  int i = 5;
  int result = s21_bn_comp_parts(ba, bb, i--);
  while (result == 0 && i >= 0) result = s21_bn_comp_parts(ba, bb, i--);
  return result;
}

int s21_bn_comp_parts(s21_big_num *ba, s21_big_num *bb, int i) {
  int result = 0;
  if (ba->bits[i] < bb->bits[i])
    result = -1;
  else if (ba->bits[i] > bb->bits[i])
    result = 1;
  return result;
}

int s21_bit(int num, int idx) { return (num >> idx) & 1; }

int s21_bn_bit(unsigned int num, int idx) { return (int)(num >> idx) & 1; }

void s21_set_bit(int *num, int idx, int value) {
  if (value == 1)
    *num |= (1 << idx);
  else
    *num &= ~(1 << idx);
}

void s21_bn_set_bit(unsigned int *num, int idx, int value) {
  if (value == 1)
    *num |= (1 << idx);
  else
    *num &= ~(1 << idx);
}

int s21_get_sign(s21_decimal *d) { return s21_bit(d->bits[3], 31); }

void s21_set_sign(s21_decimal *d, int sign) {
  s21_set_bit(&d->bits[3], 31, sign);
}

int s21_get_pow(s21_decimal *d) {
  int result = 0;
  int x = 1;
  for (int i = 16; i <= 23; i++) {
    if (s21_bit(d->bits[3], i)) result += x;
    x *= 2;
  }
  return result;
}

void s21_set_pow(s21_decimal *d, int power) {
  for (int i = 16; i <= 23 && power != 0; i++) {
    s21_set_bit(&d->bits[3], i, power % 2);
    power /= 2;
  }
}

void s21_from_decimal_to_big_num(s21_decimal *d, s21_big_num *b) {
  b->bits[0] = d->bits[0];
  b->bits[1] = d->bits[1];
  b->bits[2] = d->bits[2];
  b->bits[3] = 0;
  b->bits[4] = 0;
  b->bits[5] = 0;
  b->bits[6] = d->bits[3];
}

void s21_bn_set_pow(s21_big_num *d, int power) {
  for (int i = 16; i <= 23 && power != 0; i++) {
    s21_bn_set_bit(&d->bits[6], i, power % 2);
    power /= 2;
  }
}

int s21_bn_get_pow(s21_big_num *d) {
  int result = 0;
  int x = 1;
  for (int i = 16; i <= 23; i++) {
    if (s21_bn_bit(d->bits[6], i)) result += x;
    x *= 2;
  }
  return result;
}

int s21_bn_len(s21_big_num *b) {
  int len = 192;
  char flag = 1;
  for (int i = 5; flag && i >= 0; i--) {
    for (int j = 31; flag && j >= 0; j--) {
      if (s21_bn_bit(b->bits[i], j) == 0)
        len--;
      else
        flag = 0;
    }
  }
  return len;
}

void s21_bn_shift_left(s21_big_num *b) {
  for (int i = 5; i > 0; i--) {
    b->bits[i] <<= 1;
    s21_bn_set_bit(&b->bits[i], 0, s21_bn_bit(b->bits[i - 1], 31));
  }
  b->bits[0] <<= 1;
}

void s21_bn_shift_left_n_times(s21_big_num *b, int n) {
  for (int i = 0; i < n; i++) {
    s21_bn_shift_left(b);
  }
}

void s21_bn_shift_right(s21_big_num *b) {
  for (int i = 0; i < 5; i++) {
    b->bits[i] >>= 1;
    s21_bn_set_bit(&b->bits[i], 31, s21_bn_bit(b->bits[i + 1], 0));
  }
  b->bits[5] >>= 1;
}

void s21_bn_mul_10(s21_big_num *b) {
  s21_big_num tmp;
  s21_bn_shift_left(b);
  tmp = *b;
  s21_bn_shift_left(&tmp);
  s21_bn_shift_left(&tmp);
  s21_bn_add(b, &tmp);
}

void s21_from_big_num_to_decimal(s21_big_num *b, s21_decimal *d) {
  d->bits[0] = (int)b->bits[0];
  d->bits[1] = (int)b->bits[1];
  d->bits[2] = (int)b->bits[2];
  d->bits[3] = (int)b->bits[6];
}

void s21_bn_add(s21_big_num *dst, s21_big_num *src) {
  int carry = 0;
  for (int i = 0; i <= 5; i++) {
    for (int j = 0; j <= 31; j++) {
      int bit_a = s21_bn_bit(dst->bits[i], j);
      int bit_b = s21_bn_bit(src->bits[i], j);
      int sum = bit_a + bit_b + carry;
      carry = sum / 2;
      s21_bn_set_bit(&dst->bits[i], j, sum % 2);
    }
  }
}

void s21_bn_mul_10_n_times(s21_big_num *b, int n) {
  for (int i = 0; i < n; i++) {
    s21_bn_mul_10(b);
  }
}

int s21_add_handler(s21_decimal *value_1, s21_decimal *value_2) {
  int flag;

  int sign1 = s21_get_sign(value_1);
  int sign2 = s21_get_sign(value_2);

  if (sign1 != sign2) {  // +- or -+
    if (sign1 == 1) {    // -+
      s21_set_sign(value_1, 0);
      flag = s21_sub_handler(value_2, value_1);
      *value_1 = *value_2;
    } else {  // +-
      s21_set_sign(value_2, 0);
      flag = s21_sub_handler(value_1, value_2);
    }
  } else {  // ++ or --
    int res_sign = 0;

    if (sign1 == 1) {  // --
      res_sign = 1;
    }

    s21_big_num ba, bb;
    s21_from_decimal_to_big_num(value_1, &ba);
    s21_from_decimal_to_big_num(value_2, &bb);
    s21_bn_equalize_pow(&ba, &bb);  // Выравниваем порядки

    s21_bn_add(&ba, &bb);  // Складываем

    // Округление
    flag = s21_bn_round_or_fail(&ba);
    if (flag == 0) {
      s21_from_big_num_to_decimal(&ba, value_1);
      s21_set_sign(value_1, res_sign);
    } else {
      flag += res_sign;
    }
  }

  return flag;
}

int s21_bn_round_or_fail(s21_big_num *b) {
  int len = s21_bn_len(b);
  int power = s21_bn_get_pow(b);
  int rest = 0;
  while ((len > 96 && power > 0) || power > 28) {
    rest = s21_bn_div_10(b);
    power -= 1;
    len = s21_bn_len(b);
  }
  int fail = 0;
  if (len > 96) {
    fail = 1;
  } else {
    s21_bn_set_pow(b, power);
    if ((rest > 5 || (rest == 5 && s21_bn_bit(b->bits[0], 0) == 1))) {
      s21_big_num tmp = {{1, 0, 0, 0, 0, 0, 0}};
      s21_bn_add(b, &tmp);
      fail = (s21_bn_len(b) > 96);
    }
  }
  return fail;
}

int s21_bn_div_10(s21_big_num *b) {  // делим на 1010
  s21_big_num res = {0};
  s21_big_num ten = {{10, 0, 0, 0, 0, 0, 0}};

  int len = s21_bn_len(b);

  if (len >= 4) {
    int counter = len - 4 + 1;
    s21_bn_shift_left_n_times(&ten, len - 4);
    while (counter) {
      s21_big_num tmp = *b;
      int minus = s21_bn_sub(&tmp, &ten);
      s21_bn_shift_left(&res);
      if (minus == 0) {
        res.bits[0] |= 1;
        *b = tmp;
      }
      --counter;
      s21_bn_shift_right(&ten);
    }
  }
  int rest = s21_bn_digit_to_10_num_sys(b->bits[0]);
  *b = res;
  return rest;
}

int s21_bn_div_10_n_times(s21_big_num *b, int n) {
  int rest = 0;
  for (int i = 0; i < n; i++) {
    rest = s21_bn_div_10(b);
  }
  return rest;
}

int s21_bn_digit_to_10_num_sys(unsigned int num) {
  int result = 0;
  int x = 1;
  for (int i = 0; i < 4; i++) {
    result += s21_bn_bit(num, i) * x;
    x *= 2;
  }
  return result;
}

int s21_bn_sub(s21_big_num *a, s21_big_num *b) {  // a = |a - b|
  int minus = s21_bn_comp_num(a, b) == -1;
  int negate_coefficient = minus ? -1 : 1;
  int carry = 0;
  for (int i = 0; i <= 5; i++) {
    for (int j = 0; j <= 31; j++) {
      int bit_a = s21_bn_bit(a->bits[i], j);
      int bit_b = s21_bn_bit(b->bits[i], j);
      int sub = (bit_a - bit_b) * negate_coefficient - carry;
      carry = sub < 0;
      s21_bn_set_bit(&a->bits[i], j, (sub + 2) % 2);
    }
  }
  return minus;
}

int s21_sub_handler(s21_decimal *value_1, s21_decimal *value_2) {
  int flag;

  int sign1 = s21_get_sign(value_1);
  int sign2 = s21_get_sign(value_2);

  if (sign1 != sign2) {  // +- or -+
    if (sign1 == 1) {    // -+
      s21_set_sign(value_1, 0);
      flag = s21_add_handler(value_1, value_2);
      s21_set_sign(value_1, 1);
    } else {  // +-
      s21_set_sign(value_2, 0);
      flag = s21_add_handler(value_1, value_2);
    }
  } else {             // ++ or --
    if (sign1 == 1) {  // --
      s21_set_sign(value_2, 0);
      s21_set_sign(value_1, 0);
      s21_decimal tmp = *value_1;
      *value_1 = *value_2;
      *value_2 = tmp;
    }

    s21_big_num ba, bb;
    s21_from_decimal_to_big_num(value_1, &ba);
    s21_from_decimal_to_big_num(value_2, &bb);

    s21_bn_equalize_pow(&ba, &bb);  // Выравниваем порядки

    int res_sign = s21_bn_sub(&ba, &bb);  // Вычитаем

    // Округление
    flag = s21_bn_round_or_fail(&ba);
    if (flag == 0) {
      s21_from_big_num_to_decimal(&ba, value_1);
      s21_set_sign(value_1, res_sign);
    } else
      flag += res_sign;
  }
  return flag;
}

void s21_bn_equalize_pow(s21_big_num *ba, s21_big_num *bb) {
  int pow1 = s21_bn_get_pow(ba);
  int pow2 = s21_bn_get_pow(bb);
  if (pow1 < pow2) {
    s21_bn_mul_10_n_times(ba, pow2 - pow1);
    s21_bn_set_pow(ba, pow2);
  } else if (pow1 > pow2) {
    s21_bn_mul_10_n_times(bb, pow1 - pow2);
    s21_bn_set_pow(bb, pow1);
  }
}

int s21_mul_handler(s21_decimal *value_1, s21_decimal *value_2) {
  int flag;

  int res_sign = 0;                                      // ++ or --
  if (s21_get_sign(value_1) != s21_get_sign(value_2)) {  // +- or -+
    res_sign = 1;
  }

  int res_pow = s21_get_pow(value_1) + s21_get_pow(value_2);

  s21_big_num ba, bb;
  s21_from_decimal_to_big_num(value_1, &ba);
  s21_from_decimal_to_big_num(value_2, &bb);

  // Умножаем
  s21_bn_mul(&ba, &bb);

  // Округляем
  s21_bn_set_pow(&ba, res_pow);
  flag = s21_bn_round_or_fail(&ba);
  if (flag == 0) {
    s21_from_big_num_to_decimal(&ba, value_1);
    s21_set_sign(value_1, res_sign);
  } else
    flag += res_sign;

  return flag;
}

void s21_bn_mul(s21_big_num *a, s21_big_num *b) {
  s21_big_num res = {0};
  s21_big_num tmp = *a;
  int len = s21_bn_len(b);
  for (int i = 0; i < len; i++) {
    if (s21_bn_bit(b->bits[0], 0)) {
      s21_bn_add(&res, &tmp);
    }
    s21_bn_shift_left(&tmp);
    s21_bn_shift_right(b);
  }
  *a = res;
}

int s21_div_handler(s21_decimal *value_1, s21_decimal *value_2) {
  int flag;
  int res_sign = 0;                                      // ++ or --
  if (s21_get_sign(value_1) != s21_get_sign(value_2)) {  // +- or -+
    res_sign = 1;
  }
  s21_big_num ba, bb;
  s21_from_decimal_to_big_num(value_1, &ba);
  s21_from_decimal_to_big_num(value_2, &bb);
  s21_bn_equalize_pow(&ba, &bb);
  // Делим
  int res_pow = s21_bn_div(&ba, &bb);
  // Округляем
  s21_bn_set_pow(&ba, res_pow);
  flag = s21_bn_round_or_fail(&ba);
  if (flag == 0) {
    s21_from_big_num_to_decimal(&ba, value_1);
    s21_set_sign(value_1, res_sign);
  } else
    flag += res_sign;
  return flag;
}

void s21_bn_div_with_remainder(s21_big_num *a, s21_big_num *divisor,
                               s21_big_num *rem) {
  s21_big_num res = {0};
  s21_big_num b = *divisor;
  if (s21_bn_comp_num(a, &b) != -1) {
    int counter = s21_bn_len(a) - s21_bn_len(&b) + 1;
    s21_bn_shift_left_n_times(&b, counter - 1);
    while (counter) {
      s21_big_num tmp = *a;
      int minus = s21_bn_sub(&tmp, &b);
      s21_bn_shift_left(&res);
      if (minus == 0) {
        res.bits[0] |= 1;
        *a = tmp;
      }
      --counter;
      s21_bn_shift_right(&b);
    }
  }
  *rem = *a;
  *a = res;
}

int s21_bn_div(s21_big_num *a, s21_big_num *b) {
  s21_big_num rem = {0};
  s21_bn_div_with_remainder(a, b, &rem);
  int res_pow = 0;
  s21_big_num zero = {0};
  while (s21_bn_comp_num(&rem, &zero) != 0 && res_pow < 29) {
    s21_big_num tmp = rem;
    s21_bn_mul_10(&tmp);
    s21_bn_div_with_remainder(&tmp, b, &rem);
    s21_bn_mul_10(a);
    s21_bn_add(a, &tmp);
    res_pow++;
  }
  return res_pow;
}

int s21_get_exp_from_string(char *str) {
  int result = 0;
  char *ptr = str;
  while (*ptr) {
    if (*ptr == 'E') {
      ++ptr;
      result = (int)strtol(ptr, NULL, 10);
      break;
    }
    ++ptr;
  }
  return result;
}

void s21_float_string_to_decimal(char *str, s21_decimal *res) {
  int digits_counter = 6;
  char *ptr = str;
  int exp = s21_get_exp_from_string(str) - 6;
  int val = 0;
  while (*ptr) {
    if (*ptr == '.') {
      ++ptr;
      continue;
    } else if (*ptr >= '0' && *ptr <= '9') {
      val += ((*ptr) - '0') * (int)pow(10, digits_counter);
      --digits_counter;
      ++ptr;
    } else {
      break;
    }
  }
  s21_from_int_to_decimal(val, res);

  if (exp > 0) {
    s21_big_num bn;
    s21_from_decimal_to_big_num(res, &bn);
    s21_bn_mul_10_n_times(&bn, exp);
    s21_from_big_num_to_decimal(&bn, res);
  } else if (exp < 0) {
    if (exp < -28) {
      s21_big_num bn;
      s21_from_decimal_to_big_num(res, &bn);
      s21_bn_div_10_n_times(&bn, -exp - 28);
      s21_from_big_num_to_decimal(&bn, res);
      exp = -28;
    }
    s21_set_pow(res, -exp);
  }
}
