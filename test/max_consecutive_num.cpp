#include <iostream>
#include <unordered_set>
#include <algorithm>


int main()
{
    int array[] = {1, 94, 93, 1000, 5, 92, 78};
    int len = sizeof(array)/sizeof(int);
    int counter;
    int max_len = 0;
    int current_val;

    std::unordered_set<int> S;

    // Copy the array to unordered map.
    for (counter = 0;
            counter < len;
            counter++)
        S.insert(array[counter]);

    for (counter = 0;
            counter < len;
            counter++)
    {
        if (S.find(array[counter] - 1) == S.end())
        {
            current_val = array[counter];

            while(S.find(current_val) != S.end())
                current_val++;

            max_len =
                std::max(max_len,current_val - array[counter]);
        }
    }
    
    printf("%d\n",max_len);

    return 0;
}




