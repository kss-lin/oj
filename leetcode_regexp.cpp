#include <stdio.h>

bool is_match(const char* str, const char* match_str, char prev, int i, int j)
{
    if(match_str[j] == '\0' || str[j] == '\0')
    {
        return match_str[j] == str[j] || match_str[j] == '*';
    }

    switch (match_str[j])
    {
        case '.':
            return is_match(str, match_str, '.', i + 1, j + 1);
        case '*':
            if( str[i] == prev && is_match(str,match_str,prev,i+1,j))
            {
               return true;
            }
            return is_match(str, match_str, '*', i, j+1);
        default:
            if(str[i] == match_str[j])
                return is_match(str, match_str, match_str[j], i+1, j+1);
            else if(match_str[j+1] == '*')
                return is_match(str, match_str, prev, i, j+2);
    }

    return false;
}

int main(int argc, char** argv)
{
    if( argc < 3 )
    {
        printf("%s str match_pattern\n", argv[0]);
        return 1;
    }

    printf("Is Match:%d\n", is_match(argv[1], argv[2], '\0', 0, 0));

    return 0;
    

}
