# CESU-8 ANSI C header library

This is an ANSI C header library that implements some basic utility functions that converts c-style strings from [CESU-8](https://www.unicode.org/reports/tr26/) to [UTF-8](https://en.wikipedia.org/wiki/UTF-8) and vice-versa.

From the [associated technical report](https://www.unicode.org/reports/tr26/):

> CESU-8 documents an obsolete internal-use encoding scheme for Unicode identical to UTF-8 except for its representation of supplementary characters. In CESU-8, supplementary characters are represented as six-byte sequences rather than four-byte sequences.

I've been playing around with the [Duktape](https://duktape.org) JavaScript runtime which uses CESU-8 for its internal representation of unicode characters. I wasn't able to find a small library to convert between UTF-8 and CESU-8, so I wrote this one.

## API

The main API assumes C-style null-terminated strings and provides helper functions to help you allocate enough memory to do the conversions. To convert a C-style string from cesu-8 to utf-8:

```c
  const char ok_cesu8[] = {0xED, 0xA0, 0xBC, 0xED, 0xB6, 0x97, '\0'};
  int ok_utf8_len = utf8_len_from_cesu8(ok_cesu8, strlen(ok_cesu8)) + 1;
  char *ok_utf8 = (char *)malloc(ok_utf8_len);
  copy_cesu8_as_utf8(ok_utf8, ok_cesu8);
  printf("%s\n", (char *)ok_utf8); // prints the :ok: emoji
  free(ok_utf8);
```

To convert a C-style string from utf-8 to cesu-8:

```c
  const char thumbs_up_utf8[] = {0xF0, 0x9F, 0x91, 0x8D, '\0'};
  int thumbs_up_cesu8_len =
      cesu8_len_from_utf8(thumbs_up_utf8, strlen(thumbs_up_utf8)) + 1;
  char *thumbs_up_cesu8 = (char *)malloc(thumbs_up_cesu8_len);
  copy_utf8_as_cesu8(thumbs_up_cesu8, thumbs_up_utf8);
  // do something with the thumbs up emoji encoded as cesu8
  free(thumbs_up_cesu8);
```

Take a look at [test.c](test.c) for example `to_cesu8` and `to_utf8` helper functions which allocate memory for you.

If you'd prefer to work with raw buffers (non-null terminated `char` buffers), you can use the buffer APIs directly. To do this, you'll need to specify the length of the originating buffer:

```c

static void copy_cesu8_as_utf8_buffer(char *dest, const char *cesu8,
                                      size_t cesu8_length);

static void copy_utf8_as_cesu8_buffer(char *dest, const char *utf8,
                                      size_t utf8_length);
```

Note that if a string does not contain non-supplementary characters, you might not want to allocate additional memory or make a copy since the cesu8 and utf8 representations will be identical. Currently, the best way to determine that is to check the resulting length and see if its the same as the original string.

```c
  if (strlen(cesu8) == utf8_len_from_cesu8(cesu8, strlen(cesu8))) {
      // no need to copy since the utf8 and cesu8 strings are equivalent
  } else {
      copy_cesu8_as_utf8(utf8, cesu8, strlen(cesu8));
  }
```

There's a related `cesu8_len_from_utf8` function, along with a handful of other functions that may be useful in other contexts.