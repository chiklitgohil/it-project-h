#include <stdio.h>
#include "auth.h"
#include "crud.h"

int main(void)
{
    /* initialize data folder/files if you want (optional) */
    authMenu(); /* launches the guided UI/auth flows */
    printf("\nGoodbye.\n");
    return 0;
}
