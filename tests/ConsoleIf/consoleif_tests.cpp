#include <gtest/gtest.h>
#include <ConsoleIf.h>

ConsoleIf::ConsoleIf console_if;

TEST(CheckWrongStructure, CreateCommandString){
    std::string get_test = "get_test*desc<int>";
    auto res = console_if.create_command(get_test, [](const std::vector<std::any>& args) {
    });

    ASSERT_FALSE(res);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();
    return result;
}