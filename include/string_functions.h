#ifndef STRING_FUNCTIONS_H
#define STRING_FUNCTIONS_H

#include <string_view>

constexpr bool is_slash(char ch) noexcept { return ch == '/' || ch == '\\'; }

constexpr bool is_number(std::string_view str) noexcept
{
    for (char ch : str)
    {
        if ((ch < '0' || ch > '9')
            && ch != '-'
            && ch != '.'
            && ch != 'e'
            && ch != 'E')
        {
            return false;
        }
    }

    return true;
}

#endif // STRING_FUNCTIONS_H
