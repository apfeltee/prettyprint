<!--
update this file with the header from prettyprint.hpp. kinda like a lazy doxygen.
-->

```
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
```