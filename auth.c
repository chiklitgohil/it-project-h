#include "auth.h"
#include <ctype.h>

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h> /* getch available on Windows */
#endif

/* Migrate legacy credential filenames (if present) to the new names.
   This helps preserve existing data after we renamed credential files.
   Called once at program startup. */
void migrateCredentialFiles(void)
{
    /* Map old -> new */
    const char *old_new[][2] = {
        {"data/users.dat", "data/patients_credentials.dat"},
        {"data/doctor_creds.dat", "data/doctors_credentials.dat"},
        {"data/admin_creds.dat", "data/admins_credentials.dat"},
    };

    for (size_t i = 0; i < sizeof(old_new) / sizeof(old_new[0]); ++i)
    {
        const char *oldp = old_new[i][0];
        const char *newp = old_new[i][1];
        FILE *fold = fopen(oldp, "rb");
        if (fold)
        {
            fclose(fold);
            /* if new file already exists, skip rename to avoid overwrite */
            FILE *fnew = fopen(newp, "rb");
            if (fnew)
            {
                fclose(fnew);
                continue;
            }
            /* perform rename; if it fails, just continue */
            rename(oldp, newp);
        }
    }
}

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

    if (checkCredential(PATIENTS_CRED_FILE, username, password, &patientId))
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

    if (checkCredential(DOCTORS_CRED_FILE, username, password, &doctorId))
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
        printf("5) View Doctor Reports\n");
        printf("6) View Your Bills\n");
        printf("0) Logout\n");
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
            scheduleAppointmentWithConflictCheck(patientId);
            break;
        case 3:
            rescheduleAppointment();
            break;
        case 4:
            cancelAppointment();
            break;
        case 5:
            viewDoctorReports(patientId);
            break;
        case 6:
            viewPatientBills(patientId);
            break;
        case 0:
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
        printf("2) Add Medical Record/Report\n");
        printf("3) Search Patient by Name\n");
        printf("0) Logout\n");
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
        {
            char name[50];
            printf("Enter patient name to search: ");
            getInputAuth(name, sizeof(name));
            searchPatientByName(name);
            break;
        }
        case 0:
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

    /* Auto-generate Patient ID */
    c.id = getNextId(PATIENTS_CRED_FILE, sizeof(Credential));
    printf("Your Patient ID: %d\n", c.id);

    printf("Choose username: ");
    getInputAuth(c.username, sizeof(c.username));
    if (strlen(c.username) == 0 || usernameExists(PATIENTS_CRED_FILE, c.username))
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
    if (fwrite(&p, sizeof(Patient), 1, pf) != 1)
    {
        perror("Failed to write patient profile");
        fclose(pf);
        return 0;
    }
    fflush(pf);
    fclose(pf);

    if (!saveCredential(PATIENTS_CRED_FILE, &c))
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

    /* Auto-generate Doctor ID */
    c.id = getNextId(DOCTORS_CRED_FILE, sizeof(Credential));
    printf("Your Doctor ID: %d\n", c.id);

    printf("Choose username: ");
    getInputAuth(c.username, sizeof(c.username));
    if (strlen(c.username) == 0 || usernameExists(DOCTORS_CRED_FILE, c.username))
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
    if (fwrite(&d, sizeof(Doctor), 1, df) != 1)
    {
        perror("Failed to write doctor profile");
        fclose(df);
        return 0;
    }
    fflush(df);
    fclose(df);

    /* Save to DOCTORS_CRED_FILE */
    if (!saveCredential(DOCTORS_CRED_FILE, &c))
    {
        printf("Failed to save credentials.\n");
        return 0;
    }

    printf("Signup successful!\n");
    return 1;
}

int adminLogin(void)
{
    printf("\n=== Admin Login ===\n");
    char username[64], password[64];
    int adminId = 0;

    printf("Username: ");
    getInputAuth(username, sizeof(username));
    printf("Password: ");
    getInputAuth(password, sizeof(password));

    if (checkCredential(ADMINS_CRED_FILE, username, password, &adminId))
    {
        printf("Login successful. Admin ID: %d\n", adminId);
        adminPortal(adminId);
        return 1;
    }
    else
    {
        printf("Invalid username/password.\n");
        return 0;
    }
}

int adminSignup(void)
{
    printf("\n=== Admin Signup ===\n");
    Credential c = {0};
    char pwd1[64], pwd2[64];

    /* Auto-generate Admin ID */
    c.id = getNextId(ADMINS_CRED_FILE, sizeof(Credential));
    printf("Your Admin ID: %d\n", c.id);

    printf("Choose username: ");
    getInputAuth(c.username, sizeof(c.username));
    if (strlen(c.username) == 0 || usernameExists(ADMINS_CRED_FILE, c.username))
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

    if (!saveCredential(ADMINS_CRED_FILE, &c))
    {
        printf("Failed to save credentials.\n");
        return 0;
    }

    printf("Signup successful!\n");
    return 1;
}

/* Helper: View all appointments for admin */
static void viewAllAppointmentsAdmin(void)
{
    printf("\n--- All Appointments ---\n");
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp)
    {
        printf("No appointments found.\n");
        return;
    }
    Appointment a;
    int found = 0;
    printf("\n%-5s %-10s %-10s %-20s %-25s %-10s\n",
           "ID", "Patient", "Doctor", "Date/Time", "Reason", "Status");
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        printf("%-5d %-10d %-10d %-20s %-25s %-10s\n",
               a.id, a.patient_id, a.doctor_id, a.appointment_date, a.reason, a.status);
        found = 1;
    }
    fclose(fp);
    if (!found)
        printf("No appointments found.\n");
}

void adminPortal(int adminId)
{
    int choice = -1;
    while (1)
    {
        printf("\n=== Admin Portal (ID: %d) ===\n", adminId);
        printf("1) View All Patients\n");
        printf("2) View All Doctors\n");
        printf("3) Search Patient by Name\n");
        printf("4) View All Appointments & Assign Doctor\n");
        printf("5) View Analytics & Reports\n");
        printf("0) Logout\n");
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
            viewPatients();
            break;
        case 2:
            viewDoctors();
            break;
        case 3:
        {
            char name[50];
            printf("Enter patient name to search: ");
            getInputAuth(name, sizeof(name));
            searchPatientByName(name);
            break;
        }
        case 4:
        {
            int sub = -1;
            while (sub != 0)
            {
                printf("\n--- Appointment Management ---\n");
                printf("1) View All Appointments\n");
                printf("2) Assign Doctor to Patient\n");
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
                    viewAllAppointmentsAdmin();
                    break;
                case 2:
                    assignDoctorToPatient();
                    break;
                case 0:
                    break;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
            }
            break;
        }
        case 5:
        {
            int sub = -1;
            while (sub != 0)
            {
                printf("\n=== Analytics & Reports ===\n");
                printf("1) Total Patients & Doctors\n");
                printf("2) Total Appointments\n");
                printf("3) View All Bills & Revenue\n");
                printf("4) Doctor Appointment Count\n");
                printf("0) Back to Menu\n");
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
                    totalPatientsAndDoctors();
                    break;
                case 2:
                    totalAppointments();
                    break;
                case 3:
                    viewAllBills();
                    break;
                case 4:
                    doctorAppointmentCount();
                    break;
                case 0:
                    break;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
            }
            break;
        }
        case 0:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice.\n");
            break;
        }
    }
}