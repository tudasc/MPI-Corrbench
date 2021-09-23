#ifndef CORRBENCH_NONDETERMINISM_H
#define CORRBENCH_NONDETERMINISM_H

#define USE_TEMP_COMPARE_BUF
#define SIGNAL_FILE_NAME_ERROR "error_not_present"

//#define USE_DISTURBED_THREAD_ORDER

#ifdef USE_DISTURBED_THREAD_ORDER
#define DISTURB_THREAD_ORDER us_sleep(omp_get_thread_num());
#else
#define DISTURB_THREAD_ORDER
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef USE_TEMP_COMPARE_BUF
#include <stdlib.h>
#endif

// Tell Corrbench if the error has manifested itself
// or if there was no error e.g. "wrong" thread ordering prevented a data race
static inline void has_error_manifested(bool manifested) {
  // else do nothing: we assume that an error was present unless signaled otherwise
  if (!manifested) {
    // just create the signal file
    FILE *file_ptr = fopen(SIGNAL_FILE_NAME_ERROR, "w");
    fclose(file_ptr);
printf("ERROR_NOT_PRESENT\n");
  }
}

#define BUFFER_LENGTH_INT 1000
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))


// init unique message buffer and checks if the content is the expected content without the need for data transfer
// this way we can find "wrong" message matching
// sender and receiver need to use the same pattern_id

const unsigned char pattern_list[8] = {0x0F, 0xF0, 0xAA, 0x55, 0x99, 0xCC, 0x00, 0xFF};

static inline void fill_message_buffer(void *buf, size_t length, int pattern) {
  memset(buf, pattern_list[pattern], length);
}

// check if message buffer is correct
static inline bool has_buffer_expected_content(void *buf, size_t length, int pattern) {
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

static inline void us_sleep(long int microseconds) {
  const long int scale2sec = (long int)1e6;
  const long int scale2nsec = 1000;
  const long int tv_sec = microseconds / scale2sec;
  const long int tv_nano = (microseconds % scale2sec) * scale2nsec;

  struct timespec ts = {tv_sec, tv_nano};
  nanosleep(&ts, NULL);
}

#endif
