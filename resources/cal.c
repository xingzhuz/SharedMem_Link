#include "stuscore.h"

void calculate(struct student *d, int n)
{
    for (int i = 0; i < n; i++)
    {
        d[i].sum = d[i].ch + d[i].math;
        d[i].ave = d[i].sum / 2.0;
    }
}