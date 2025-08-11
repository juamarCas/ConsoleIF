#include <gtest/gtest.h>
#include <ConsoleIf.h>

ConsoleIf::ConsoleIf console_if;

TEST(CheckWrongStructure, CreateCommandStringWrongDescription) {
    std::string get_test = "get_test*desc<int>";
    auto res = console_if.create_command(get_test, [](const std::vector<std::any>& args) {
    });

    ASSERT_FALSE(res);

    get_test = "get_testdesc*<int>";
    res = console_if.create_command(get_test, [](const std::vector<std::any>& args) {
    });

    ASSERT_FALSE(res);
}

TEST(CheckWrongStructure, CreateCommandStringWrongParameterType) {
    std::string get_test = "get_test*desc*<in>";
    auto res = console_if.create_command(get_test, [](const std::vector<std::any>& args) {
    });

    ASSERT_FALSE(res);

    get_test = "get_test*desc*int>";
    res = console_if.create_command(get_test, [](const std::vector<std::any>& args) {
    });

    ASSERT_FALSE(res);

    get_test = "get_test*desc*<int";
    res = console_if.create_command(get_test, [](const std::vector<std::any>& args) {
    });

    ASSERT_FALSE(res);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();
    return result;
}