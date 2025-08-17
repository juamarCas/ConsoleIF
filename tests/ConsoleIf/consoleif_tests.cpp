#include <gtest/gtest.h>
#include <ConsoleIf.h>

namespace ConsoleIf
{
    ConsoleIf console_if;

    TEST(CheckWrongStructure, CreateCommandStringWrongDescription)
    {
        std::string get_test = "get_test*desc<int>";
        auto res = console_if.create_command(get_test, [](const std::vector<std::any> &args) {});

        ASSERT_FALSE(res);

        get_test = "get_testdesc*<int>";
        res = console_if.create_command(get_test, [](const std::vector<std::any> &args) {});

        ASSERT_FALSE(res);
    }

    TEST(CheckWrongStructure, CreateCommandStringWrongParameterStructure)
    {
        std::string get_test = "get_test*desc*int>";
        bool res = console_if.create_command(get_test, [](const std::vector<std::any> &args) {});

        ASSERT_FALSE(res);

        get_test = "get_test*desc*<int";
        res = console_if.create_command(get_test, [](const std::vector<std::any> &args) {});

        ASSERT_FALSE(res);
    }

    TEST(CheckWrongStructure, CreateCommandStringWrongParameterType)
    {
        std::string get_test = "get_test*desc*<integer>";
        bool res = console_if.create_command(get_test, [](const std::vector<std::any> &args) {});

        ASSERT_FALSE(res);

        get_test = "get_test*desc*<int, str>";
        res = console_if.create_command(get_test, [](const std::vector<std::any> &args) {});

        ASSERT_FALSE(res);

        get_test = "get_test*desc*<int float doublee>";
        res = console_if.create_command(get_test, [](const std::vector<std::any> &args) {});

        ASSERT_FALSE(res);

        get_test = "get_test*desc*<int str>";
        res = console_if.create_command(get_test, [](const std::vector<std::any> &args) {});

        ASSERT_FALSE(res);
    }

    TEST_F(ConsoleIfTest, TestProcessCommand)
    {
        std::string test;
        bool res;

        test = "get_test*desc*<int>";
        res = console_if.create_command(test, [](const std::vector<std::any> &args)
                                             {
            ASSERT_EQ(args.size(), 1);
            ASSERT_TRUE(std::any_cast<int>(args[0]) == 42);

            spdlog::info("Command executed with int: {}", std::any_cast<int>(args[0])); });

        EXPECT_EQ(console_if.process_command("get test 42").is_ok, true);

        test = "get_test<int>_list<int string>";
        res = console_if.create_command(test, [](const std::vector<std::any> &args){
            ASSERT_EQ(args.size(), 3);
            ASSERT_TRUE(std::any_cast<int>(args[0]) == 5);
            ASSERT_TRUE(std::any_cast<int>(args[1]) == 4);
            ASSERT_TRUE(std::any_cast<std::string>(args[2]) == "test");

            spdlog::info("Command executed with int: {}, int: {}, string: {}", std::any_cast<int>(args[0]), std::any_cast<int>(args[1]), std::any_cast<std::string>(args[2]));
        });

        EXPECT_EQ(console_if.process_command("get test 5 list 4 test").is_ok, true);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();
    return result;
}