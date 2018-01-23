
#pragma once

/**
* prettyprint.hpp primarily provides the following endpoints:
*
* namespaces: pretty
*
* functions:
*     template<...> print(std::ostream&, <thing>)
*
* usage:
*
*     std::vector<...> thing = ...;
*     pretty::print(std::cout, thing);
*
* or:
*
*     std::vector<...> thing = ...;
*     std::string prettyprintedvector = pretty::to_string(thing);
*
* Obviously, any std::ostream compatible interfaces can be used, such as
* std::stringstream, std::*fstream, etc.
* due to ambiguity that arises with templating, it's not really feasible
* to implement a pretty:print() for -any- stream kind -- in those cases, you're
* best off using pretty::to_string().
*
* if you don't want to have to use pretty::print() or pretty::to_string()
* explicitly, you can import the namespace 'pretty::streamops',
* which defines operator<<() overrides, so you can do:
*
*     std::vector<...> t = ...;
*     std::cout << t << std::endl;
*
*/

#include <typeinfo>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <functional>
#include <cctype>
#include <cstring>
#include <cwchar>

#if defined(__GNUC__)
    #if defined(__GXX_ABI_VERSION)
        #include <cxxabi.h>
        #define CPRETTYPRINT_HAVE_CXXABI
    #endif
#elif defined(_MSCVER)
    #include <dbghelp.h>
    #define CPRETTYPRINT_HAVE_MSVC
#endif

namespace pretty
{
    namespace internal
    {
        template<typename KeyT, typename ValueT>
        struct printmaplike
        {
            template<template<class, class, class...> class ContainerT>
            static void doit(
                std::ostream&,
                const std::string&,
                const ContainerT<KeyT, ValueT>&
            );
        };

        template<typename ValueT>
        struct printlistlike
        {
            template<template<class, class...> class ContainerT>
            static void doit(
                std::ostream&,
                const std::string&,
                const ContainerT<ValueT>&
            );
        };

        /*
        * retrieves the fully qualified name of a type by type only.
        * if available, it'll demangle it as well.
        */
        template<typename Type>
        std::string tname_real()
        {
            #if defined(CPRETTYPRINT_HAVE_CXXABI)
                int status;
                char* demang;
                std::string ret;
                std::string rawtn;
                rawtn = typeid(Type).name();
                demang = abi::__cxa_demangle(rawtn.c_str(), NULL, NULL, &status);
                if(status == 0)
                {
                    ret = demang;
                    std::free(demang);
                }
                else
                {
                    ret = rawtn;
                }
                return ret;
            #elif defined(CPRETTYPRINT_HAVE_MSVC)
                size_t rtsz;
                size_t bufsz;
                char* buffer;
                std::string ret;
                std::string rawtn;
                rawtn = typeid(Type).name();
                bufsz = (rawtn.size() + 50);
                buffer = new char[bufsz];
                rtsz = UndecorateSymbolName(rawtn.c_str(), buffer, bufsz, UNDNAME_COMPLETE);
                if(rtsz > 0)
                {
                    ret = std::string(buffer, rtsz);
                }
                else
                {
                    ret = rawtn;
                }
                delete[] buffer;
                return ret;
            #else
                return typeid(Type).name();
            #endif
        }

        /*
        * forward-decls for a few types to speed up the runtime.
        */
        template<typename Type>
        std::string tname();

        /* --begin generated-- (see gen.rb) */
        template<>
        std::string tname<short>()
        {
            return "short";
        }

        template<>
        std::string tname<int>()
        {
            return "int";
        }

        template<>
        std::string tname<char>()
        {
            return "char";
        }

        template<>
        std::string tname<wchar_t>()
        {
            return "wchar_t";
        }

        template<>
        std::string tname<float>()
        {
            return "float";
        }

        template<>
        std::string tname<double>()
        {
            return "double";
        }

        template<>
        std::string tname<long>()
        {
            return "long";
        }

        template<>
        std::string tname<long long>()
        {
            return "long long";
        }

        template<>
        std::string tname<std::string>()
        {
            return "std::string";
        }

        template<>
        std::string tname<std::wstring>()
        {
            return "std::wstring";
        }

        /* --end generated-- */

        template<typename Type>
        std::string tname()
        {
            return tname_real<Type>();
        }

        /*
        * a special variant of tname() to work with containers
        * the idea is to not print things like comparing types, allocators, etc
        * since they're usually not explicitly specified.
        * this works for things that aren't nested... but
        * fails for stuff like, say, std::vector<std::map<std::string, std::vector<...>>> ..
        * you get the idea.
        * unpacking the types *might* be possible, but what for? :^)
        */
        template<typename ValueT>
        void tname_by_ref(std::ostream& out, const std::vector<ValueT>&)
        {
            out << "std::vector<";
            out << tname<ValueT>();
            out << '>';
        }

        template<typename KeyT, typename ValueT>
        void tname_by_ref(std::ostream& out, const std::map<KeyT, ValueT>&)
        {
            out << "std::map<";
            out << tname<KeyT>();
            out << ',';
            out << tname<ValueT>();
            out << '>';
        }

        template<typename AnyT>
        void tname_by_ref(std::ostream& out, const AnyT&)
        {
            out << tname<AnyT>();
        }

        template<typename CharT>
        void prchar(std::ostream& out, CharT c)
        {
            int ansbyte;
            ansbyte = int(c);
            if((ansbyte >= 32) && (ansbyte <= 126))
            {
                switch(ansbyte)
                {
                    case '"':
                        out << '\\' << '"';
                        break;
                    default:
                        out << char(ansbyte);
                        break;
                }
            }
            else
            {
                switch(ansbyte)
                {
                    case '\0':
                        out << '\\' << '0';
                        break;
                    case '\n':
                        out << '\\' << 'n';
                        break;
                    case '\r':
                        out << '\\' << 'r';
                        break;
                    case '\t':
                        out << '\\' << 't';
                        break;
                    case '\a':
                        out << '\\' << 'a';
                        break;
                    default:
                        out << '\\' << 'x' << std::hex << int(ansbyte);
                        break;
                }
                
            }
        }

        template<typename CharT>
        std::ostream& prstring_real(std::ostream& out, const CharT* str, size_t len)
        {
            int c;
            size_t i;
            out << '"';
            for(i=0; i<len; i++)
            {
                c = str[i];
                prchar(out, c);
            }
            out << '"';
            return out;
        }

        /*
        * a prstring that includes the size of the string as well.
        * not actually used - it's too pedantic :^)
        */
        template<typename CharT>
        std::ostream& prstring_pedantic(std::ostream& out, const std::basic_string<CharT>& str)
        {
            out << "std::basic_string<" << internal::tname<CharT>() << ">(";
            prstring_real(out, str.c_str(), str.size());
            out << ", " << str.size() << ")";
            return out;
        }

        std::ostream& prstring(std::ostream& out, const char* str, size_t len)
        {
            return prstring_real(out, str, len);
        }

        std::ostream& prstring(std::ostream& out, const wchar_t* str, size_t len)
        {
            out << 'L';
            return prstring_real(out, str, len);
        }

        template<typename Type>
        void prunknown(std::ostream& out, const Type& tval)
        {
            const void* ptr;
            ptr = (const void*)(&tval);
            out << "(";
            tname_by_ref<Type>(out, tval);
            out << ")" << ptr;
        }
    }

    template<typename Type>
    std::ostream& print(std::ostream& out)
    {
        out << internal::tname<Type>();
        return out;
    }

    std::ostream& print(std::ostream& out, int ival)
    {
        out << ival;
        return out;
    }

    std::ostream& print(std::ostream& out, float fval)
    {
        out << fval;
        return out;
    }

    std::ostream& print(std::ostream& out, double dval)
    {
        out << dval;
        return out;
    }

    std::ostream& print(std::ostream& out, char cval)
    {
        out << '\'';
        internal::prchar(out, cval);
        out << '\'';
        return out;
    }

    std::ostream& print(std::ostream& out, wchar_t cval)
    {
        out << "L'";
        internal::prchar(out, cval);
        out << "'";
        return out;
    }

    std::ostream& print(std::ostream& out, const char* str, size_t len)
    {
        internal::prstring(out, str, len);
        return out;
    }

    std::ostream& print(std::ostream& out, const std::string& str)
    {
        internal::prstring(out, str.c_str(), str.size());
        return out;
    }

    std::ostream& print(std::ostream& out, const wchar_t* str, size_t len)
    {
        internal::prstring(out, str, len);
        return out;
    }

    std::ostream& print(std::ostream& out, const std::wstring& str)
    {
        internal::prstring(out, str.c_str(), str.size());
        return out;
    }

    template<size_t Size>
    std::ostream& print(std::ostream& out, const char (&str)[Size])
    {
        out << '(' << internal::tname<char>() << '[' << Size << ']' << ')';
        internal::prstring(out, str, Size-1);
        return out;
    }

    template<size_t Size>
    std::ostream& print(std::ostream& out, const wchar_t (&str)[Size])
    {
        out << '(' << internal::tname<wchar_t>() << '[' << Size << ']' << ')';
        internal::prstring(out, str, Size-1);
        return out;
    }

    std::ostream& print(std::ostream& out, const char* str)
    {
        internal::prstring(out, str, std::strlen(str));
        return out;
    }

    std::ostream& print(std::ostream& out, const wchar_t* str)
    {
        internal::prstring(out, str, std::wcslen(str));
        return out;
    }

    template<typename ValueT>
    std::ostream& print(std::ostream& out, const std::vector<ValueT>& vec);

    template<typename KeyT, typename ValueT>
    std::ostream& print(std::ostream& out, const std::map<KeyT, ValueT>& map);

    template<typename Type>
    std::ostream& print(std::ostream& out, const Type& val)
    {
        internal::prunknown<Type>(out, val);
        return out;
    }

    template<typename ValueT>
    std::ostream& print(std::ostream& out, const std::vector<ValueT>& vec)
    {
        internal::printlistlike<ValueT>::doit(out, "std::vector", vec);
        return out;
    }

    template<typename KeyT, typename ValueT>
    std::ostream& print(std::ostream& out, const std::map<KeyT, ValueT>& map)
    {
        internal::printmaplike<KeyT, ValueT>::doit(out, "std::map", map);
        return out;
    }

    template<typename KeyT, typename ValueT>
    std::ostream& print(std::ostream& out, const std::multimap<KeyT, ValueT>& map)
    {
        internal::printmaplike<KeyT, ValueT>::doit(out, "std::multimap", map);
        return out;
    }

    template<typename Mystery>
    std::string to_string(const Mystery& val)
    {
        std::stringstream buf;
        print(buf, val);
        return buf.str();
    }

    /* --- this is where the guts of the functions that depend on other functions are defined */
    namespace internal
    {
        template<typename KeyT, typename ValueT>
        template<template<class, class, class...> class ContainerT>
        void printmaplike<KeyT, ValueT>::doit(
            std::ostream& out,
            const std::string& name,
            const ContainerT<KeyT, ValueT>& map
        )
        {
            (void)name;
            internal::tname_by_ref(out, map);
            out << '{';
            for(auto it=map.begin(); it!=map.end(); it++)
            {
                print(out, it->first);
                out << "=";
                print(out, it->second);
                if(std::next(it) != map.end())
                {
                    out << ',' << ' ';
                }
            }
            out << '}';
        }

        template<typename ValueT>
        template<template<class, class...> class ContainerT>
        void printlistlike<ValueT>::doit(
            std::ostream& out,
            const std::string& name,
            const ContainerT<ValueT>& lst
        )
        {
            (void)name;
            internal::tname_by_ref(out, lst);
            out << '[';
            for(auto it=lst.begin(); it!=lst.end(); it++)
            {
                print(out, *it);
                if(std::next(it) != lst.end())
                {
                    out << ',' << ' ';
                }
            }
            out << ']';
        }
    }

    /*
    * this namespace is deliberately encapsulated -
    * it provides operator<<() overloads for std::ostream.
    */
    namespace streamops
    {
        template<typename CharT, typename ValueT>
        std::basic_ostream<CharT>& operator<<(
            std::basic_ostream<CharT>& strm,
            const std::vector<ValueT>& vec
        )
        {
            pretty::print(strm, vec);
            return strm;
        }

        template<typename CharT, typename KeyT, typename ValueT>
        std::basic_ostream<CharT>& operator<<(
            std::basic_ostream<CharT>& strm,
            const std::map<KeyT, ValueT>& map
        )
        {
            pretty::print(strm, map);
            return strm;
        }

        template<typename CharT, typename RetT, typename... ArgsT>
        std::basic_ostream<CharT>& operator<<(
            std::basic_ostream<CharT>& strm,
            std::function<RetT(ArgsT...)> fn
        )
        {
            pretty::print(strm, fn);
            return strm;
        }
    }
}
