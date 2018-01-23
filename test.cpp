
#include <iostream>
#include "prettyprint.hpp"

#define block(...) \
    { \
        __VA_ARGS__; \
        std::cout << std::endl << "------------" << std::endl; \
    }

template<typename Type>
std::vector<Type>& operator<<(std::vector<Type>& vec, Type&& val)
{
    vec.push_back(val);
    return vec;
}



void test(int argc, char** argv)
{
    using namespace pretty::streamops;
    block({
        std::vector<std::string> args(argv + 0, argv + argc);
        std::cout << args << std::endl;
    });

    block({
        std::vector<char> vec;
        vec << 'h' << 'e' << 'l' << 'o' << ' ' << 'w' << 'o' << 'r' << 'l' << 'd' << '!';
        std::cout << vec << std::endl;
    });

    block({
        std::vector<wchar_t> vec;
        vec << L'g' << L'o' << L'd' << L'b' << L'y' << L'e' << L'w' << L'o' << L'r' << L'l' << L'd';
        std::cout << vec << std::endl;
    });

    block({
        std::cout << pretty::to_string("hello world!") << std::endl;
        std::cout << pretty::to_string(L"goodbye world!") << std::endl;
    });

    block({
        struct Foo
        {
            int a;
            int b;
            int c;
        };
        std::map<std::string, Foo> stuff;
        stuff["blorp"] = Foo{42, 7, 3};
        stuff["dengs"] = Foo{53, 94, 20};
        stuff["wolpo"] = Foo{94, 29, 20};
        std::cout << stuff << std::endl;
    });

    block({
        std::map<std::string, std::vector<std::map<float, std::string>>> st;
        auto mk = [](int howmany, float fval, const std::string& str)
        {
            int i;
            std::map<float, std::string> map;
            std::vector<std::map<float, std::string>> vec;
            for(i=0; i<howmany; i++)
            {
                map[fval * i] = str;
            }
            vec.push_back(map);
            return vec;
        };
        st["funk"] = mk(3, 4.881, "shpork");
        st["pork"] = mk(7, 9.001, "giraffes");
        st["huli"] = mk(2, 6.032, "matey");
        std::cout << st << std::endl;
    });

    block({
        std::function<bool(std::array<long long, 666>)> whatever;
        std::cout << whatever << std::endl;
    });
}

int main(int argc, char* argv[])
{
    test(argc, argv);
    return 0;
}

