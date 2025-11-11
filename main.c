#include <stdio.h>
#include "auth.h"
#include "crud.h"

/* Top-level Main Menu
   - Login
   - Signup
   - Exit
   This main presents explicit Login/Signup submenus so auth flows from auth.c are invoked
   and the user can always go Back to the Main Menu. */
int main(void)
{
    /* Migrate any legacy credential files to the new names before starting */
    migrateCredentialFiles();
    int choice = -1;
    while (1)
    {
        printf("\n==== Main Menu ====\n");
        printf("1) Login\n");
        printf("2) Signup\n");
        printf("0) Exit\n");
        printf("Choose: ");
        if (scanf("%d", &choice) != 1)
        {
            clearStdin();
            choice = -1;
        }
        clearStdin();

        if (choice == 1)
        {
            int sub = -1;
            while (sub != 0)
            {
                printf("\n-- Login --\n");
                printf("1) Patient Login\n");
                printf("2) Doctor Login\n");
                printf("3) Admin Login\n");
                printf("0) Back\n");
                printf("Choose: ");
                if (scanf("%d", &sub) != 1)
                {
                    clearStdin();
                    sub = -1;
                }
                clearStdin();
                switch (sub)
                {
                case 1:
                    patientLogin();
                    break;
                case 2:
                    doctorLogin();
                    break;
                case 3:
                    adminLogin();
                    break;
                case 0:
                    break;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
            }
        }
        else if (choice == 2)
        {
            int sub = -1;
            while (sub != 0)
            {
                printf("\n-- Signup --\n");
                printf("1) Patient Signup\n");
                printf("2) Doctor Signup\n");
                printf("3) Admin Signup\n");
                printf("0) Back\n");
                printf("Choose: ");
                if (scanf("%d", &sub) != 1)
                {
                    clearStdin();
                    sub = -1;
                }
                clearStdin();
                switch (sub)
                {
                case 1:
                    patientSignup();
                    break;
                case 2:
                    doctorSignup();
                    break;
                case 3:
                    adminSignup();
                    break;
                case 0:
                    break;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
            }
        }
        else if (choice == 0)
        {
            printf("Exiting. Goodbye.\n");
            break;
        }
        else
        {
            printf("Invalid choice.\n");
        }
    }
    return 0;
}
