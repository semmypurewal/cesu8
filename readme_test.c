#include "cesu8.h"
#include <stdio.h>

int main(void) {
  const char ok_cesu8[] = {0xED, 0xA0, 0xBC, 0xED, 0xB6, 0x97, '\0'};
  int ok_utf8_len = utf8_len_from_cesu8(ok_cesu8, strlen(ok_cesu8)) + 1;
  char *ok_utf8 = (char *)malloc(ok_utf8_len);
  copy_cesu8_as_utf8(ok_utf8, ok_cesu8);
  printf("%s\n", (char *)ok_utf8); // prints the :ok: emoji
  free(ok_utf8);

  const char thumbs_up_utf8[] = {0xF0, 0x9F, 0x91, 0x8D, '\0'};
  int thumbs_up_cesu8_len =
      cesu8_len_from_utf8(thumbs_up_utf8, strlen(thumbs_up_utf8)) + 1;
  char *thumbs_up_cesu8 = (char *)malloc(thumbs_up_cesu8_len);
  copy_utf8_as_cesu8(thumbs_up_cesu8, thumbs_up_utf8);
  // do something with the thumbs up emoji encoded as cesu8
  free(thumbs_up_cesu8);
}
