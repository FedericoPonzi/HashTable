//
// Created by fponzi on 20/05/22.
//
#include <gtest/gtest.h>

extern "C" {
    #include <hash_table.h>
}

class MyAppTestSuite : public testing::Test {
    void SetUp() override {
    }

    void TearDown() override {}
};

TEST_F(MyAppTestSuite, test_hash_table_simple_insert_search_remove) {
    HashMap* ht = HashMap_new();
    const char* KEY = "hello";
    const char* VALUE = "world";

    ht_hash_table_HashMap_insert(ht, KEY, VALUE);
    const char* ret = ht_hash_table_HashMap_search(ht, KEY);
    EXPECT_STREQ(ret, VALUE);
    ht_hash_table_HashMap_remove(ht, KEY);
    EXPECT_EQ(nullptr, ht_hash_table_HashMap_search(ht, KEY));

    ht_hash_table_HashMap_destroy(ht);
}

TEST_F(MyAppTestSuite, test_hashmap_insert_overwrite){
    HashMap* ht = HashMap_new();
    const char* KEY = "hello";
    const char* VALUE = "world";
    const char* NEW_VALUE = "planet";

    ht_hash_table_HashMap_insert(ht, KEY, VALUE);
    ht_hash_table_HashMap_insert(ht, KEY, NEW_VALUE);
    EXPECT_STREQ(NEW_VALUE, ht_hash_table_HashMap_search(ht, KEY));

    ht_hash_table_HashMap_destroy(ht);
}

char* gen(int i) {
    char* base = static_cast<char *>(malloc(5));
    strcpy(base, "basew");
    base[4] = i;
    return base;
}

TEST_F(MyAppTestSuite, test_hashmap_resize){
    HashMap* ht = HashMap_new();
    const char* VALUE = "world";
    for(int i = 1; i < 100; i++){
        char* b = gen(i);
        ht_hash_table_HashMap_insert(ht, b, VALUE);
    }
    // Verify the resize up happened correctly:
    for(int i = 1; i < 100; i++){
        char* b = gen(i);
        EXPECT_STREQ(VALUE, ht_hash_table_HashMap_search(ht, b));
    }
    // Verify that the resize down happened correctly:
    for(int i = 1; i < 100; i++){
        char* b = gen(i);
        EXPECT_STREQ(VALUE, ht_hash_table_HashMap_search(ht, b));
        ht_hash_table_HashMap_remove(ht, b);
        free(b);
    }

    ht_hash_table_HashMap_destroy(ht);
}




TEST_F(MyAppTestSuite, test_hashing_function){
    EXPECT_EQ(ht_hash_table_HashMap_single_hash("cat", 163, 53), 18);
    EXPECT_EQ(ht_hash_table_HashMap_single_hash("cat", 151, 53), 2);
}