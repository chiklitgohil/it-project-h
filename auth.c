#include "auth.h"
#include <ctype.h>

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h> /* getch available on Windows */
#endif

/* getInputAuth - use this for auth input, distinct from crud.getInput */
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
    getInputAuth(buf, size);
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

/* Save credential - FIXED */
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
        perror("Failed to open credential file");
        return 0;
    }

    size_t written = fwrite(c, sizeof(Credential), 1, fp);
    fclose(fp);

    if (written != 1)
    {
        printf("[ERROR] Failed to write credential to file.\n");
        return 0;
    }

    printf("[SUCCESS] Credential saved to %s\n", file);
    return 1;
}

/* Authenticate */
static int checkCredential(const char *file, const char *username, const char *password, int *outId)
{
    FILE *fp = fopen(file, "rb");
    if (!fp)
    {
        printf("[ERROR] Credential file not found: %s\n", file);
        return 0;
    }

    Credential c;
    int found = 0;

    while (fread(&c, sizeof(Credential), 1, fp))
    {
        if (strcmp(c.username, username) == 0)
        {
            found = 1;
            if (strcmp(c.password, password) == 0)
            {
                *outId = c.id;
                fclose(fp);
                return 1;
            }
            /* Username found but password wrong */
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

int patientLogin(void)
{
    printf("\n=== Patient Login ===\n");
    char username[64], password[64];
    int patientId = 0;

    printf("Username: ");
    getInputAuth(username, sizeof(username));
    printf("Password: ");
    getInputAuth(password, sizeof(password));

    if (checkCredential(USER_CRED_FILE, username, password, &patientId))
    {
        printf("Login successful. Patient ID: %d\n", patientId);
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
    printf("\n=== Doctor Login ===\n");
    char username[64], password[64];
    int doctorId = 0;

    printf("Username: ");
    getInputAuth(username, sizeof(username));
    printf("Password: ");
    getInputAuth(password, sizeof(password));

    if (checkCredential(DOCTOR_CRED_FILE, username, password, &doctorId))
    {
        printf("Login successful. Doctor ID: %d\n", doctorId);
        doctorPortal(doctorId);
        return 1;
    }
    else
    {
        printf("Invalid username/password.\n");
        return 0;
    }
}

/* Helper: view appointments for a specific patient */
static void viewPatientAppointments(int patientId)
{
    printf("\n--- Your Appointments ---\n");
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp)
    {
        printf("No appointments found.\n");
        return;
    }
    Appointment a;
    int found = 0;
    printf("\n%-5s %-10s %-20s %-25s %-10s\n",
           "ID", "Doctor", "Date/Time", "Reason", "Status");
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.patient_id == patientId)
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

/* Helper: view appointments for a specific doctor */
static void viewDoctorAppointments(int doctorId)
{
    printf("\n--- Your Appointments ---\n");
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp)
    {
        printf("No appointments found.\n");
        return;
    }
    Appointment a;
    int found = 0;
    printf("\n%-5s %-10s %-20s %-25s %-10s\n",
           "ID", "Patient", "Date/Time", "Reason", "Status");
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.doctor_id == doctorId)
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

/* Simplified portals - remove UI extras, keep core functionality */
void patientPortal(int patientId)
{
    int choice = -1;
    while (1)
    {
        printf("\n=== Patient Portal (ID: %d) ===\n", patientId);
        printf("1) View Your Appointments\n");
        printf("2) Schedule Appointment\n");
        printf("3) Reschedule Appointment\n");
        printf("4) Cancel Appointment\n");
        printf("5) View Medical History\n");
        printf("6) View Your Bills\n");
        printf("7) Search Patient by Name\n");
        printf("8) Logout\n");
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
            viewPatientAppointments(patientId);
            break;
        case 2:
            scheduleAppointmentWithConflictCheck();
            break;
        case 3:
            rescheduleAppointment();
            break;
        case 4:
            cancelAppointment();
            break;
        case 5:
            viewPatientMedicalHistory();
            break;
        case 6:
            viewPatientBills(patientId);
            break;
        case 7:
        {
            char name[50];
            printf("Enter patient name to search: ");
            getInputAuth(name, sizeof(name));
            searchPatientByName(name);
            break;
        }
        case 8:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice.\n");
            break;
        }
    }
}

void doctorPortal(int doctorId)
{
    int choice = -1;
    while (1)
    {
        printf("\n=== Doctor Portal (ID: %d) ===\n", doctorId);
        printf("1) View Your Appointments\n");
        printf("2) Add Medical Record\n");
        printf("3) View All Appointments\n");
        printf("4) View Analytics\n");
        printf("5) Logout\n");
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
            viewDoctorAppointments(doctorId);
            break;
        case 2:
            addMedicalRecord();
            break;
        case 3:
            viewAppointments();
            break;
        case 4:
            analyticsMenu();
            break;
        case 5:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice.\n");
            break;
        }
    }
}

/* ---------------- Signups & Logins ---------------- */

int patientSignup(void)
{
    printf("\n=== Patient Signup ===\n");
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
        printf("ID already in use.\n");
        return 0;
    }

    printf("Choose username: ");
    getInputAuth(c.username, sizeof(c.username));
    if (strlen(c.username) == 0 || usernameExists(USER_CRED_FILE, c.username))
    {
        printf("Username invalid or taken.\n");
        return 0;
    }

    do
    {
        printf("Choose password (min 6 chars, letters + digits): ");
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
            printf("Password does not meet policy.\n");
            continue;
        }
        break;
    } while (1);
    strncpy(c.password, pwd1, sizeof(c.password) - 1);

    Patient p;
    p.id = c.id;
    printf("\nEnter patient profile details:\n");
    printf("Name: ");
    getInputAuth(p.name, sizeof(p.name));
    printf("Age: ");
    scanf("%d", &p.age);
    clearStdin();
    printf("Gender: ");
    getInputAuth(p.gender, sizeof(p.gender));
    printf("Phone: ");
    getInputAuth(p.phone, sizeof(p.phone));
    printf("Disease: ");
    getInputAuth(p.disease, sizeof(p.disease));

    FILE *pf = fopen(PATIENT_FILE, "ab+");
    if (!pf)
    {
        perror("Failed to open patient file");
        return 0;
    }
    fwrite(&p, sizeof(Patient), 1, pf);
    fclose(pf);

    if (!saveCredential(USER_CRED_FILE, &c))
    {
        printf("Failed to save credentials.\n");
        return 0;
    }

    printf("Signup successful!\n");
    return 1;
}

int doctorSignup(void)
{
    printf("\n=== Doctor Signup ===\n");
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

    /* Check if ID already exists in DOCTOR_CRED_FILE (not DOCTOR_FILE) */
    if (idExistsInCredentials(DOCTOR_CRED_FILE, c.id))
    {
        printf("ID already in use.\n");
        return 0;
    }

    printf("Choose username: ");
    getInputAuth(c.username, sizeof(c.username));
    if (strlen(c.username) == 0 || usernameExists(DOCTOR_CRED_FILE, c.username))
    {
        printf("Username invalid or taken.\n");
        return 0;
    }

    do
    {
        printf("Choose password (min 6 chars, letters + digits): ");
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
            printf("Password does not meet policy.\n");
            continue;
        }
        break;
    } while (1);
    strncpy(c.password, pwd1, sizeof(c.password) - 1);

    Doctor d;
    d.id = c.id;
    printf("\nEnter doctor profile details:\n");
    printf("Name: ");
    getInputAuth(d.name, sizeof(d.name));
    printf("Specialization: ");
    getInputAuth(d.specialization, sizeof(d.specialization));
    printf("Phone: ");
    getInputAuth(d.phone, sizeof(d.phone));

    FILE *df = fopen(DOCTOR_FILE, "ab+");
    if (!df)
    {
        perror("Failed to open doctor file");
        return 0;
    }
    fwrite(&d, sizeof(Doctor), 1, df);
    fclose(df);

    /* Save to DOCTOR_CRED_FILE, not USER_CRED_FILE */
    if (!saveCredential(DOCTOR_CRED_FILE, &c))
    {
        printf("Failed to save credentials.\n");
        return 0;
    }

    printf("Signup successful!\n");
    return 1;
}