#include <Arduino.h>
#include <unity.h>
#include "main.h"

void test_single_argument_parsing(void)
{
    char * expected_single = "3123.123asdf";

    String all_args = F("3123.123asdf");
    char * args[1];
    uint8_t ret = 0;
  
    ret = parse_arguments(args, all_args, 1);
    TEST_ASSERT_EQUAL_STRING(expected_single, args[0]);
    TEST_ASSERT_EQUAL(1, ret);
    free(args[0]);

    char * args2[1];
    all_args = F("3123.123asdf fail");
    ret = parse_arguments(args2, all_args, 1);
    TEST_ASSERT_EQUAL_STRING(expected_single, args2[0]);
    TEST_ASSERT_EQUAL(1, ret);
    free(args2[0]);

    char * args3[1];
    all_args = F(" 3123.123asdf fail");
    ret = parse_arguments(args3, all_args, 1);
    TEST_ASSERT_EQUAL_STRING(expected_single, args3[0]);
    TEST_ASSERT_EQUAL(1, ret);
    free(args3[0]);
}

void test_multiple_argument_parsing(void)
{
    char * expected[] = {"3123.123asdf", "second", "321./"};

    String all_args = F("3123.123asdf second 321./");
    char * args[3];
    int i;
    uint8_t ret;
    ret = parse_arguments(args, all_args, 3);
    TEST_ASSERT_EQUAL(3, ret);

    for (i = 0; i < 3; i++)
    {
        TEST_ASSERT_EQUAL_STRING(expected[i], args[i]);
        free(args[i]);
    }

    all_args = F("3123.123asdf second 321./ fail");
    ret = parse_arguments(args, all_args, 3);
    TEST_ASSERT_EQUAL(3, ret);
    for (i = 0; i < 3; i++)
    {
        TEST_ASSERT_EQUAL_STRING(expected[i], args[i]);
        free(args[i]);
    }

    all_args = F(" 3123.123asdf second");
    ret = parse_arguments(args, all_args, 3);
    TEST_ASSERT_EQUAL(2, ret);

    for (i = 0; i < ret; i++)
    {
        TEST_ASSERT_EQUAL_STRING(expected[i], args[i]);
        free(args[i]);
    }
}


void setup() {

    Serial.begin(115200);
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    UNITY_BEGIN();    // IMPORTANT LINE!
    //bizzare string issues caused this to fail in specific sequences, so we run it multiple times
    RUN_TEST(test_multiple_argument_parsing);
    RUN_TEST(test_single_argument_parsing);
    RUN_TEST(test_multiple_argument_parsing);
    RUN_TEST(test_single_argument_parsing);
    RUN_TEST(test_single_argument_parsing);
    RUN_TEST(test_multiple_argument_parsing);

    UNITY_END();
}

void loop() {
    ;
}
