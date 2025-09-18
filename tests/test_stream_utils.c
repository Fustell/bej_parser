#include "stream_utils.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_stream_read_int(void) {
  uint8_t buf[] = {0x01, 0x02, 0x03, 0x04};
  InputStream s = {buf, sizeof(buf), 0};

  uint64_t v1 = StreamReadInt(&s, 1);
  TEST_ASSERT_EQUAL_UINT64(0x01, v1);

  uint64_t v2 = StreamReadInt(&s, 2);
  TEST_ASSERT_EQUAL_UINT64(0x0302, v2);
}

void test_stream_read_sint_negative(void) {
  uint8_t buf[] = {0xFE};
  InputStream s = {buf, sizeof(buf), 0};

  int64_t v = stream_read_sint(&s, 1);
  TEST_ASSERT_EQUAL_INT64(-2, v);
}

void test_output_stream_write(void) {
  OutputStream os;
  OutputStreamInit(&os);

  OutputStreamWrite(&os, "abc", 3);
  TEST_ASSERT_EQUAL_STRING("abc", os.data);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_stream_read_int);
  RUN_TEST(test_stream_read_sint_negative);
  RUN_TEST(test_output_stream_write);
  return UNITY_END();
}