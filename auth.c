#include "auth.h"
#include <ctype.h>

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h> /* getch available on Windows */
#endif

/* Helpers */
void clearStdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void getInputAuth(char *buf, int size)
{
    if (!fgets(buf, size, stdin))
    {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = 0;
}

/* Very simple password policy:
   - at least 6 chars
   - contains at least one digit and one letter */
int validatePassword(const char *pwd)
{
    int len = (int)strlen(pwd);
    if (len < 6)
        return 0;
    int hasDigit = 0, hasAlpha = 0;
    for (int i = 0; pwd[i]; ++i)
    {
        if (isdigit((unsigned char)pwd[i]))
            hasDigit = 1;
        if (isalpha((unsigned char)pwd[i]))
            hasAlpha = 1;
    }
    return hasDigit && hasAlpha;
}

/* Masked input for password.
   - On Windows: use getch() to provide masked input.
   - On other platforms: fallback to visible input via fgets (simple, portable). */
void maskInput(char *buf, int size)
{
    int idx = 0;
#if defined(_WIN32) || defined(_WIN64)
    int ch;
    while (idx < size - 1)
    {
        ch = _getch();
        if (ch == '\r' || ch == '\n')
        {
            putchar('\n');
            break;
        }
        if (ch == 127 || ch == 8)
        { /* backspace */
            if (idx > 0)
            {
                idx--;
                fputs("\b \b", stdout);
            }
            continue;
        }
        buf[idx++] = (char)ch;
        putchar('*');
    }
    buf[idx] = '\0';
#else
    /* Fallback: visible input. Keep user experience simple and portable. */
    getInputAuth(buf, size);
#endif
}

/* Credential file helpers */
static int usernameExists(const char *file, const char *username)
{
    FILE *fp = fopen(file, "rb");
    if (!fp)
        return 0;
    Credential c;
    while (fread(&c, sizeof(c), 1, fp))
    {
        if (strcmp(c.username, username) == 0)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

static int idExistsInCredentials(const char *file, int id)
{
    FILE *fp = fopen(file, "rb");
    if (!fp)
        return 0;
    Credential c;
    while (fread(&c, sizeof(c), 1, fp))
    {
        if (c.id == id)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Read patient/doctor existence by id from data files using structs in crud.h */
static int idExistsInPatients(int id)
{
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp)
        return 0;
    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (p.id == id)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

static int idExistsInDoctors(int id)
{
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    if (!fp)
        return 0;
    Doctor d;
    while (fread(&d, sizeof(Doctor), 1, fp))
    {
        if (d.id == id)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Save credential */
static int saveCredential(const char *file, const Credential *c)
{
/* Ensure data directory exists */
#if defined(_WIN32) || defined(_WIN64)
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif

    FILE *fp = fopen(file, "ab");
    if (!fp)
    {
        printf("Failed to open credential file: %s\n", file);
        return 0;
    }
    size_t written = fwrite(c, sizeof(*c), 1, fp);
    fclose(fp);
    if (written != 1)
    {
        printf("Failed to write credential\n");
        return 0;
    }
    return 1;
}

/* Authenticate */
static int checkCredential(const char *file, const char *username, const char *password, int *outId)
{
    FILE *fp = fopen(file, "rb");
    if (!fp)
    {
        printf("Warning: No credential file found at %s\n", file);
        return 0;
    }

    Credential c;
    size_t read;
    int found = 0;

    while ((read = fread(&c, sizeof(c), 1, fp)) == 1)
    {
        if (strcmp(c.username, username) == 0)
        {
            found = 1;
            if (strcmp(c.password, password) == 0)
            {
                if (outId)
                    *outId = c.id;
                fclose(fp);
                return 1;
            }
            /* Username found but wrong password */
            break;
        }
    }

    fclose(fp);
    if (!found)
    {
        printf("Username not found.\n");
    }
    else
    {
        printf("Incorrect password.\n");
    }
    return 0;
}

/* Small UI helpers */
static void printBoxed(const char *title)
{
    int len = (int)strlen(title) + 4;
    for (int i = 0; i < len; ++i)
        putchar('*');
    putchar('\n');
    printf("* %s *\n", title);
    for (int i = 0; i < len; ++i)
        putchar('*');
    putchar('\n');
}

/* lightweight screen clear (portable) */
static void clearScreen(void)
{
    for (int i = 0; i < 40; ++i)
        putchar('\n');
}

/* breadcrumb header */
static void showBreadcrumb(const char *role, int id, const char *section)
{
    printBoxed(" Portal ");
    printf("%s > ID:%d > %s\n", role, id, section ? section : "Home");
    putchar('-');
    for (int i = 0; i < 50; i++)
        putchar('-');
    putchar('\n');
}

/* yes/no confirmation helper */
static int confirmYesNo(const char *prompt)
{
    char ans[8];
    while (1)
    {
        printf("%s (y/n): ", prompt);
        getInputAuth(ans, sizeof(ans));
        if (ans[0] == 'y' || ans[0] == 'Y')
            return 1;
        if (ans[0] == 'n' || ans[0] == 'N')
            return 0;
        printf("Please answer y or n.\n");
    }
}

/* ---------------- Signups & Logins ---------------- */

int patientSignup(void)
{
    clearScreen();
    printBoxed("Patient Signup");
    Credential c = {0};
    char pwd1[64], pwd2[64];

    printf("Choose numeric Patient ID: ");
    if (scanf("%d", &c.id) != 1)
    {
        clearStdin();
        printf("Invalid ID.\n");
        return 0;
    }
    clearStdin();

    if (idExistsInCredentials(USER_CRED_FILE, c.id) || idExistsInPatients(c.id))
    {
        printf("ID already in use. Try signing in or pick another ID.\n");
        return 0;
    }

    printf("Choose username: ");
    getInputAuth(c.username, sizeof(c.username));
    if (strlen(c.username) == 0)
    {
        printf("Username cannot be empty.\n");
        return 0;
    }
    if (usernameExists(USER_CRED_FILE, c.username))
    {
        printf("Username taken. Try another.\n");
        return 0;
    }

    /* Password */
    do
    {
        printf("Choose password (min 6 chars, must include letters & digits): ");
        maskInput(pwd1, sizeof(pwd1));
        printf("Confirm password: ");
        maskInput(pwd2, sizeof(pwd2));
        if (strcmp(pwd1, pwd2) != 0)
        {
            printf("Passwords do not match. Try again.\n");
            continue;
        }
        if (!validatePassword(pwd1))
        {
            printf("Password does not meet policy. Try again.\n");
            continue;
        }
        break;
    } while (1);
    strncpy(c.password, pwd1, sizeof(c.password) - 1);

    /* Create patient profile details */
    Patient p;
    p.id = c.id;
    printf("\nNow enter patient profile details (these will be saved to %s)\n", PATIENT_FILE);
    printf("Name: ");
    getInputAuth(p.name, sizeof(p.name));
    printf("Age: ");
    if (scanf("%d", &p.age) != 1)
    {
        clearStdin();
        p.age = 0;
    }
    clearStdin();
    printf("Gender: ");
    getInputAuth(p.gender, sizeof(p.gender));
    printf("Phone: ");
    getInputAuth(p.phone, sizeof(p.phone));
    printf("Disease: ");
    getInputAuth(p.disease, sizeof(p.disease));

    /* Save patient record */
    FILE *pf = fopen(PATIENT_FILE, "ab+");
    if (!pf)
    {
        perror("Failed to open patient file");
        return 0;
    }
    fwrite(&p, sizeof(Patient), 1, pf);
    fclose(pf);

    /* Save credential */
    if (!saveCredential(USER_CRED_FILE, &c))
    {
        printf("Failed to save credentials.\n");
        return 0;
    }

    printf("Signup successful. You can now login.\n");
    return 1;
}

int doctorSignup(void)
{
    clearScreen();
    printBoxed("Doctor Signup");
    Credential c = {0};
    char pwd1[64], pwd2[64];

    printf("Choose numeric Doctor ID: ");
    if (scanf("%d", &c.id) != 1)
    {
        clearStdin();
        printf("Invalid ID.\n");
        return 0;
    }
    clearStdin();

    if (idExistsInCredentials(DOCTOR_CRED_FILE, c.id) || idExistsInDoctors(c.id))
    {
        printf("ID already in use. Try signing in or pick another ID.\n");
        return 0;
    }

    printf("Choose username: ");
    getInputAuth(c.username, sizeof(c.username));
    if (strlen(c.username) == 0)
    {
        printf("Username cannot be empty.\n");
        return 0;
    }
    if (usernameExists(DOCTOR_CRED_FILE, c.username))
    {
        printf("Username taken. Try another.\n");
        return 0;
    }

    /* Password */
    do
    {
        printf("Choose password (min 6 chars, must include letters & digits): ");
        maskInput(pwd1, sizeof(pwd1));
        printf("Confirm password: ");
        maskInput(pwd2, sizeof(pwd2));
        if (strcmp(pwd1, pwd2) != 0)
        {
            printf("Passwords do not match. Try again.\n");
            continue;
        }
        if (!validatePassword(pwd1))
        {
            printf("Password does not meet policy. Try again.\n");
            continue;
        }
        break;
    } while (1);
    strncpy(c.password, pwd1, sizeof(c.password) - 1);

    /* Create doctor profile */
    Doctor d;
    d.id = c.id;
    printf("\nNow enter doctor profile details (these will be saved to %s)\n", DOCTOR_FILE);
    printf("Name: ");
    getInputAuth(d.name, sizeof(d.name));
    printf("Specialization: ");
    getInputAuth(d.specialization, sizeof(d.specialization));
    printf("Phone: ");
    getInputAuth(d.phone, sizeof(d.phone));

    /* Save doctor record */
    FILE *df = fopen(DOCTOR_FILE, "ab+");
    if (!df)
    {
        perror("Failed to open doctor file");
        return 0;
    }
    fwrite(&d, sizeof(Doctor), 1, df);
    fclose(df);

    /* Save credential */
    if (!saveCredential(DOCTOR_CRED_FILE, &c))
    {
        printf("Failed to save credentials.\n");
        return 0;
    }

    printf("Doctor signup successful. You can now login.\n");
    return 1;
}

int patientLogin(void)
{
    clearScreen();
    printBoxed("Patient Login");
    char username[64], password[64];
    int patientId = 0;
    printf("Username: ");
    getInputAuth(username, sizeof(username));
    printf("Password: ");
    maskInput(password, sizeof(password));
    if (checkCredential(USER_CRED_FILE, username, password, &patientId))
    {
        printf("\nFound account: %s (Patient ID: %d)\n", username, patientId);
        if (!confirmYesNo("Is this you?"))
        {
            printf("Login cancelled.\n");
            return 0;
        }
        patientPortal(patientId);
        return 1;
    }
    else
    {
        printf("Invalid username/password.\n");
        return 0;
    }
}

int doctorLogin(void)
{
    clearScreen();
    printBoxed("Doctor Login");
    char username[64], password[64];
    int doctorId = 0;
    printf("Username: ");
    getInputAuth(username, sizeof(username));
    printf("Password: ");
    maskInput(password, sizeof(password));
    if (checkCredential(DOCTOR_CRED_FILE, username, password, &doctorId))
    {
        printf("\nFound account: Dr.%s (Doctor ID: %d)\n", username, doctorId);
        if (!confirmYesNo("Is this you?"))
        {
            printf("Login cancelled.\n");
            return 0;
        }
        doctorPortal(doctorId);
        return 1;
    }
    else
    {
        printf("Invalid username/password.\n");
        return 0;
    }
}

/* ---------------- Role Portals ---------------- */

/* Show patient profile by id */
static void showPatientProfile(int id)
{
    showBreadcrumb("Patient", id, "Profile");
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp)
    {
        printf("No patients found.\n");
        return;
    }
    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (p.id == id)
        {
            printf("\nID: %d\nName: %s\nAge: %d\nGender: %s\nPhone: %s\nDisease: %s\n",
                   p.id, p.name, p.age, p.gender, p.phone, p.disease);
            fclose(fp);
            return;
        }
    }
    fclose(fp);
    printf("Profile not found.\n");
}

/* List appointments for a patient */
static void listAppointmentsForPatient(int pid)
{
    showBreadcrumb("Patient", pid, "My Appointments");
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp)
    {
        printf("No appointments found.\n");
        return;
    }
    Appointment a;
    int found = 0;
    printf("\n%-5s %-10s %-20s %-25s %-10s\n", "ID", "Doctor", "Date/Time", "Reason", "Status");
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.patient_id == pid)
        {
            printf("%-5d %-10d %-20s %-25s %-10s\n",
                   a.id, a.doctor_id, a.appointment_date, a.reason, a.status);
            found = 1;
        }
    }
    fclose(fp);
    if (!found)
        printf("No appointments found.\n");
}

/* Patient portal - menu-driven with explicit Back (0) */
void patientPortal(int patientId)
{
    int choice = -1;
    while (1)
    {
        clearScreen();
        showBreadcrumb("Patient", patientId, "Home");
        printf("1) Manage Appointments\n");
        printf("2) Reports\n");
        printf("3) Billing\n");
        printf("4) Profile\n");
        printf("5) Logout\n");
        printf("0) Back\n");
        printf("Choose: ");
        if (scanf("%d", &choice) != 1)
        {
            clearStdin();
            choice = -1;
        }
        clearStdin();

        switch (choice)
        {
        case 1:
        {
            int sub = -1;
            while (sub != 0)
            {
                clearScreen();
                showBreadcrumb("Patient", patientId, "Manage Appointments");
                printf("1) Book Appointment\n");
                printf("2) View Appointments\n");
                printf("3) Update Appointment (reschedule/cancel)\n");
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
                    printf("Booking appointment. When prompted for Patient ID, enter your ID: %d\n", patientId);
                    scheduleAppointment();
                    break;
                case 2:
                    listAppointmentsForPatient(patientId);
                    break;
                case 3:
                {
                    int up = -1;
                    clearScreen();
                    showBreadcrumb("Patient", patientId, "Update Appointment");
                    printf("1) Cancel Appointment\n");
                    printf("2) Reschedule (not implemented)\n");
                    printf("0) Back\n");
                    printf("Choose: ");
                    if (scanf("%d", &up) != 1)
                    {
                        clearStdin();
                        up = -1;
                    }
                    clearStdin();
                    if (up == 1)
                    {
                        cancelAppointment();
                    }
                    else if (up == 2)
                    {
                        printf("Rescheduling from UI is not implemented. Use the booking module or reschedule via source code.\n");
                    }
                }
                break;
                case 0:
                    break;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
                if (sub != 0)
                {
                    printf("\nPress Enter to continue...");
                    getchar();
                }
            }
        }
        break;
        case 2:
            clearScreen();
            showBreadcrumb("Patient", patientId, "Reports");
            viewPatientMedicalHistory();
            printf("\nPress Enter to continue...");
            getchar();
            break;
        case 3:
            clearScreen();
            showBreadcrumb("Patient", patientId, "Billing");
            printf("Billing features are not implemented yet.\n");
            printf("\nPress Enter to continue...");
            getchar();
            break;
        case 4:
        {
            int psub = -1;
            while (psub != 0)
            {
                clearScreen();
                showBreadcrumb("Patient", patientId, "Profile");
                printf("1) View Details\n");
                printf("2) Update Details (not implemented)\n");
                printf("0) Back\n");
                printf("Choose: ");
                if (scanf("%d", &psub) != 1)
                {
                    clearStdin();
                    psub = -1;
                }
                clearStdin();
                switch (psub)
                {
                case 1:
                    showPatientProfile(patientId);
                    break;
                case 2:
                    printf("Update profile via UI is not implemented.\n");
                    break;
                case 0:
                    break;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
                if (psub != 0)
                {
                    printf("\nPress Enter to continue...");
                    getchar();
                }
            }
        }
        break;
        case 5:
            printf("Logging out...\n");
            return;
        case 0:
            /* Return to previous screen (e.g., auth menu) */
            return;
        default:
            printf("Invalid choice.\n");
            break;
        }
        printf("\nPress Enter to continue...");
        getchar();
    }
}

/* Show doctor profile */
static void showDoctorProfile(int id)
{
    showBreadcrumb("Doctor", id, "Profile");
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    if (!fp)
    {
        printf("No doctors found.\n");
        return;
    }
    Doctor d;
    while (fread(&d, sizeof(Doctor), 1, fp))
    {
        if (d.id == id)
        {
            printf("\nID: %d\nName: %s\nSpecialization: %s\nPhone: %s\n",
                   d.id, d.name, d.specialization, d.phone);
            fclose(fp);
            return;
        }
    }
    fclose(fp);
    printf("Profile not found.\n");
}

/* List appointments for doctor */
static void listAppointmentsForDoctor(int did)
{
    showBreadcrumb("Doctor", did, "My Appointments");
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp)
    {
        printf("No appointments found.\n");
        return;
    }
    Appointment a;
    int found = 0;
    printf("\n%-5s %-10s %-20s %-25s %-10s\n", "ID", "Patient", "Date/Time", "Reason", "Status");
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.doctor_id == did)
        {
            printf("%-5d %-10d %-20s %-25s %-10s\n",
                   a.id, a.patient_id, a.appointment_date, a.reason, a.status);
            found = 1;
        }
    }
    fclose(fp);
    if (!found)
        printf("No appointments found.\n");
}

/* Doctor portal - menu-driven with explicit Back (0) */
void doctorPortal(int doctorId)
{
    int choice = -1;
    while (1)
    {
        clearScreen();
        showBreadcrumb("Doctor", doctorId, "Home");
        printf("1) Manage Appointments\n");
        printf("2) Profile\n");
        printf("3) Logout\n");
        printf("0) Back\n");
        printf("Choose: ");
        if (scanf("%d", &choice) != 1)
        {
            clearStdin();
            choice = -1;
        }
        clearStdin();

        switch (choice)
        {
        case 1:
        {
            int sub = -1;
            while (sub != 0)
            {
                clearScreen();
                showBreadcrumb("Doctor", doctorId, "Manage Appointments");
                printf("1) View Appointments\n");
                printf("2) Update Appointment (postpone/reassign/cancel)\n");
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
                    listAppointmentsForDoctor(doctorId);
                    break;
                case 2:
                {
                    int up = -1;
                    clearScreen();
                    showBreadcrumb("Doctor", doctorId, "Update Appointment");
                    printf("1) Cancel Appointment\n");
                    printf("2) Postpone/Reschedule (not implemented)\n");
                    printf("0) Back\n");
                    printf("Choose: ");
                    if (scanf("%d", &up) != 1)
                    {
                        clearStdin();
                        up = -1;
                    }
                    clearStdin();
                    if (up == 1)
                    {
                        cancelAppointment();
                    }
                    else if (up == 2)
                    {
                        printf("Rescheduling from UI is not implemented.\n");
                    }
                }
                break;
                case 0:
                    break;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
                if (sub != 0)
                {
                    printf("\nPress Enter to continue...");
                    getchar();
                }
            }
        }
        break;
        case 2:
        {
            int psub = -1;
            while (psub != 0)
            {
                clearScreen();
                showBreadcrumb("Doctor", doctorId, "Profile");
                printf("1) View Details\n");
                printf("2) Update Details (not implemented)\n");
                printf("0) Back\n");
                printf("Choose: ");
                if (scanf("%d", &psub) != 1)
                {
                    clearStdin();
                    psub = -1;
                }
                clearStdin();
                switch (psub)
                {
                case 1:
                    showDoctorProfile(doctorId);
                    break;
                case 2:
                    printf("Update profile via UI is not implemented.\n");
                    break;
                case 0:
                    break;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
                if (psub != 0)
                {
                    printf("\nPress Enter to continue...");
                    getchar();
                }
            }
        }
        break;
        case 3:
            printf("Logging out...\n");
            return;
        case 0:
            return;
        default:
            printf("Invalid choice.\n");
            break;
        }
        printf("\nPress Enter to continue...");
        getchar();
    }
}

/* Top-level auth menu - minimal choices, guided flow */
void authMenu(void)
{
    int choice = -1;
    do
    {
        clearScreen();
        printBoxed("Authentication");
        printf("1) Patient Login\n");
        printf("2) Patient Signup\n");
        printf("3) Doctor Login\n");
        printf("4) Doctor Signup\n");
        printf("0) Exit\n");
        printf("Choose: ");
        if (scanf("%d", &choice) != 1)
        {
            clearStdin();
            choice = -1;
        }
        clearStdin();
        switch (choice)
        {
        case 1:
            patientLogin();
            break;
        case 2:
            patientSignup();
            break;
        case 3:
            doctorLogin();
            break;
        case 4:
            doctorSignup();
            break;
        case 0:
            printf("Exiting authentication.\n");
            break;
        default:
            printf("Invalid choice.\n");
            break;
        }
        if (choice != 0)
        {
            printf("\nPress Enter to continue...");
            getchar();
        }
    } while (choice != 0);
}