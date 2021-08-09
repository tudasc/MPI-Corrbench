#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define USE_TEMP_COMPARE_BUF
#define SIGNAL_FILE_NAME "error_not_present"

// Tell Corrbench if the error has manifested itself
// or if there was no error e.g. "wrong" thread ordering prevented a data race
inline void has_error_manifested(bool manifested) {
  // else do nothing: we assume that an error was present unless signaled otherwise
  if (!manifested) {
    // just create the signal file
    FILE *file_ptr = fopen(SIGNAL_FILE_NAME, "w");
    fclose(file_ptr);
  }
}

// init unique message buffer and checks if the content is the expected content without the need for data transfer
// this way we can find "wrong" message matching
// sender and receiver need to use the same pattern_id

const char pattern_list[8] = {0x0F, 0xF0, 0xAA, 0x55, 0x99, 0xCC, 0x00, 0xFF};

inline void fill_message_buffer(void *buf, size_t length, int pattern) { memset(buf, pattern_list[pattern], length); }

// check if message buffer is correct
inline bool has_buffer_expected_content(void *buf, size_t length, int pattern) {
#ifdef USE_TEMP_COMPARE_BUF
  void *tmp_buf = malloc(length);
  memset(tmp_buf, pattern_list[pattern], length);
  int memcmp_result = memcmp(tmp_buf, buf, length);
  free(tmp_buf);
  return memcmp_result == 0;
#else
  char *buf_ptr = (char *)buf;  // make it clear that we iterate through the single bytes
  for (size_t i = 0; i < length; ++i) {
    if (buf_ptr[i] != pattern_list[pattern])
      return false;
  }
  return true;

#endif
}
