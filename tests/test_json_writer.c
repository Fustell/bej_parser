#include <stdio.h>

#include "json_writer.h"
#include "stream_utils.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) { remove("test.json"); }

void test_json_indent(void) {
  OutputStream os;
  OutputStreamInit(&os);

  JsonWriteIndent(&os, 2);
  TEST_ASSERT_EQUAL_STRING("\n        ", os.data);
}

void test_json_flush_to_file(void) {
  OutputStream os;
  OutputStreamInit(&os);

  OutputStreamWrite(&os, "{\"a\":1}", 7);
  bool ok = JsonWriterFlushToFile(&os, "test.json");
  TEST_ASSERT_TRUE(ok);

  FILE *f = fopen("test.json", "r");
  TEST_ASSERT_NOT_NULL(f);

  char buf[16];
  char *res = fgets(buf, sizeof(buf), f);
  fclose(f);

  TEST_ASSERT_NOT_NULL(res);
  TEST_ASSERT_EQUAL_STRING("{\"a\":1}", buf);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_json_indent);
  RUN_TEST(test_json_flush_to_file);
  return UNITY_END();
}