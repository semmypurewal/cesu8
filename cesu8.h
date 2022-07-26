#ifndef CESU8_H
#define CESU8_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *utf16_pair_to_cesu8(uint16_t *pair) {
  uint8_t *result = (uint8_t *)malloc(sizeof(uint8_t) * 6);
  uint16_t high = pair[0] - 0xD800;
  uint16_t low = pair[1] - 0xDC00;

  result[0] = 0xED;

  result[1] = (high >> 6) + 0xA0;
  result[2] = (high - (high >> 6 << 6)) + 0x80;
  result[3] = 0xED;

  result[4] = (low >> 6) + 0xB0;
  result[5] = (low - (low >> 6 << 6)) + 0x80;
  return result;
}

static uint16_t *utf32_to_utf16_pair(uint32_t code_point) {
  uint16_t *result = (uint16_t *)malloc(sizeof(uint16_t) * 2);
  uint32_t temp = code_point - 0x10000;
  uint16_t high = temp >> 10;
  uint16_t low = temp - (high << 10);
  result[0] = high + 0xD800;
  result[1] = low + 0xDC00;
  return result;
}

static uint32_t utf8_to_utf32(uint8_t *bytes) {
  uint32_t result = 0;
  result += ((uint32_t)bytes[0] - 0xF0) << 18;
  result += ((uint32_t)bytes[1] - 0x80) << 12;
  result += ((uint32_t)bytes[2] - 0x80) << 6;
  result += ((uint32_t)bytes[3] - 0x80);
  return result;
}

static uint8_t *utf32_to_utf8(uint32_t code_point) {
  uint8_t *result = (uint8_t *)malloc(sizeof(uint8_t) * 4);
  result[0] = 0xF0 + (code_point >> 18);
  result[1] = 0x80 + ((code_point - ((result[0] - 0xF0) << 18)) >> 12);
  result[2] = 0x80 + ((code_point - ((result[1] - 0x80) << 12)) >> 6);
  result[3] = 0x80 + ((code_point - ((result[2] - 0x80) << 6)));
  return result;
}

static uint32_t utf16_pair_to_utf32(uint16_t *pair) {
  return (pair[0] - 0xD800) * 0x400 + (pair[1] - 0xdc00) + 0x10000;
}

static uint16_t *cesu8_to_utf16_pairs(uint8_t *bytes, int offset) {
  uint16_t *result = (uint16_t *)malloc(sizeof(uint16_t) * 2);

  uint16_t high = (uint16_t)0xD800 + ((bytes[offset + 1] - 0xA0) << 6) +
                  (bytes[offset + 2] - 0x80);

  uint16_t low = (uint16_t)0xDC00 + ((bytes[offset + 4] - 0xB0) << 6) +
                 (bytes[offset + 5] - 0x80);

  result[0] = high;
  result[1] = low;

  return result;
}

static size_t cesu8_len_from_utf8(const char *utf8, size_t length) {
  int non_bmp_char_count = 0;
  size_t i;

  for (i = 0; i < length; i++) {
    if ((uint8_t)utf8[i] >> 4 == 0xf) {
      non_bmp_char_count++;
    }
  }

  return sizeof(char) * length + (2 * non_bmp_char_count);
}

static size_t utf8_len_from_cesu8(const char *cesu8, size_t length) {
  int non_bmp_char_count = 0;
  size_t i;

  for (i = 0; i < length; i++) {
    if ((uint8_t)cesu8[i] == 0xED) {
      non_bmp_char_count++;
      i += 5;
    }
  }

  return sizeof(char) * length - (2 * non_bmp_char_count);
}

static void copy_cesu8_as_utf8_buffer(char *dest, const char *cesu8,
                                      size_t cesu8_length) {
  size_t dest_index = 0;
  size_t utf8_index;
  uint16_t *pairs;
  uint8_t *utf8_char;

  for (utf8_index = 0; utf8_index < cesu8_length; utf8_index++) {
    if ((uint8_t)cesu8[utf8_index] != 0xED) {
      dest[dest_index++] = cesu8[utf8_index];
    } else {
      pairs = cesu8_to_utf16_pairs((uint8_t *)cesu8, utf8_index);
      utf8_index += 5;
      utf8_char = utf32_to_utf8(utf16_pair_to_utf32(pairs));
      free(pairs);
      memcpy(dest + dest_index, utf8_char, 4);
      dest_index += 4;
      free(utf8_char);
    }
  }
}

static void copy_cesu8_as_utf8(char *dest, const char *cesu8) {
  size_t length = strlen(cesu8);
  copy_cesu8_as_utf8_buffer(dest, cesu8, length);
  dest[utf8_len_from_cesu8(cesu8, length)] = '\0';
}

static void copy_utf8_as_cesu8_buffer(char *dest, const char *utf8,
                                      size_t utf8_length) {
  int dest_index = 0;
  size_t i;

  for (i = 0; i < utf8_length; i++) {
    if ((uint8_t)utf8[i] >> 4 == 0xf) {
      uint32_t utf32_temp = utf8_to_utf32((uint8_t *)utf8 + i);
      uint16_t *utf16_temp = utf32_to_utf16_pair(utf32_temp);
      uint8_t *cesu8_temp = utf16_pair_to_cesu8(utf16_temp);
      free(utf16_temp);

      memcpy(dest + dest_index, cesu8_temp, 6);
      free(cesu8_temp);
      i += 3;
      dest_index += 6;
    } else {
      dest[dest_index++] = utf8[i];
    }
  }
}

static void copy_utf8_as_cesu8(char *dest, const char *utf8) {
  size_t length = strlen(utf8);
  copy_utf8_as_cesu8_buffer(dest, utf8, length);
  dest[cesu8_len_from_utf8(utf8, length)] = '\0';
}

#endif /* CESU8_H */
