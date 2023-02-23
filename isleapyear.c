// Is leap year - xl4708
#include <stdio.h>
int main () {
    int year;
    sscanf("%d",&year);
    if (year%4==0){
        if (year%100==0){
            if (year%400==0) printf("%d was a leap year\n",year);
            else printf("%d was not a leap year\n",year);
        }
        else printf("%d was a leap year\n",year);
    }
    else printf("%d was not a leap year\n",year);
}
