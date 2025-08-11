#include <gtest/gtest.h>
#include <ConsoleIf/ConsoleIf.h>


TEST(Sample, TestSample){
    ASSERT_EQ(1, 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    
    int result = RUN_ALL_TESTS();
    return result;
}