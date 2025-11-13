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

/* ---------------- Profile-based username helpers ---------------- */

/* Return 1 if username exists in patient profiles */
static int usernameInPatients(const char *username)
{
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp) return 0;
    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (strcmp(p.username, username) == 0)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Return 1 if username exists in doctor profiles */
static int usernameInDoctors(const char *username)
{
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    if (!fp) return 0;
    Doctor d;
    while (fread(&d, sizeof(Doctor), 1, fp))
    {
        if (strcmp(d.username, username) == 0)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Return 1 if username exists in admin profiles */
static int usernameInAdmins(const char *username)
{
    FILE *fp = fopen(ADMIN_FILE, "rb");
    if (!fp) return 0;
    Admin a;
    while (fread(&a, sizeof(Admin), 1, fp))
    {
        if (strcmp(a.username, username) == 0)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Check username across all profile files to enforce global uniqueness */
static int usernameExistsGlobally(const char *username)
{
    if (usernameInPatients(username)) return 1;
    if (usernameInDoctors(username)) return 1;
    if (usernameInAdmins(username)) return 1;
    return 0;
}

/* Generic checkCredential reading username/password from a profile file.
   Returns 1 on success and sets outId, 0 otherwise. */
static int checkCredentialProfile(const char *file, const char *username, const char *password, int *outId, int role)
{
    /* role: 0=patient,1=doctor,2=admin */
    FILE *fp = fopen(file, "rb");
    if (!fp) return 0;

    if (role == 0)
    {
        Patient p;
        while (fread(&p, sizeof(Patient), 1, fp))
        {
            if (strcmp(p.username, username) == 0)
            {
                if (strcmp(p.password, password) == 0)
                {
                    *outId = p.id;
                    fclose(fp);
                    return 1;
                }
                fclose(fp);
                return 0;
            }
        }
    }
    else if (role == 1)
    {
        Doctor d;
        while (fread(&d, sizeof(Doctor), 1, fp))
        {
            if (strcmp(d.username, username) == 0)
            {
                if (strcmp(d.password, password) == 0)
                {
                    *outId = d.id;
                    fclose(fp);
                    return 1;
                }
                fclose(fp);
                return 0;
            }
        }
    }
    else /* admin */
    {
        Admin a;
        while (fread(&a, sizeof(Admin), 1, fp))
        {
            if (strcmp(a.username, username) == 0)
            {
                if (strcmp(a.password, password) == 0)
                {
                    *outId = a.id;
                    fclose(fp);
                    return 1;
                }
                fclose(fp);
                return 0;
            }
        }
    }

    fclose(fp);
    return 0;
}

/* ---------------- Signups & Logins (profile-backed) ---------------- */

/* Helper: write patient profile (includes username/password) */
static int savePatientProfile(const Patient *p)
{
#if defined(_WIN32) || defined(_WIN64)
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif
    FILE *pf = fopen(PATIENT_FILE, "ab");
    if (!pf) return 0;
    size_t w = fwrite(p, sizeof(Patient), 1, pf);
    fclose(pf);
    return w == 1;
}

/* Helper: write doctor profile */
static int saveDoctorProfile(const Doctor *d)
{
#if defined(_WIN32) || defined(_WIN64)
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif
    FILE *df = fopen(DOCTOR_FILE, "ab");
    if (!df) return 0;
    size_t w = fwrite(d, sizeof(Doctor), 1, df);
    fclose(df);
    return w == 1;
}

/* Helper: write admin profile */
static int saveAdminProfile(const Admin *a)
{
#if defined(_WIN32) || defined(_WIN64)
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif
    FILE *af = fopen(ADMIN_FILE, "ab");
    if (!af) return 0;
    size_t w = fwrite(a, sizeof(Admin), 1, af);
    fclose(af);
    return w == 1;
}

int patientSignup(void)
{
    printf("\n=== Patient Signup ===\n");
    Patient p = {0};
    char pwd1[64], pwd2[64];

    /* Auto-generate Patient ID from patient file so profile and credentials share same id */
    p.id = getNextId(PATIENT_FILE, sizeof(Patient));
    printf("Your Patient ID: %d\n", p.id);

    printf("Choose username: ");
    getInputAuth(p.username, sizeof(p.username));
    if (strlen(p.username) == 0)
    {
        printf("Username cannot be empty.\n");
        return 0;
    }
    if (usernameExistsGlobally(p.username))
    {
        printf("Username already exists. Choose a different username.\n");
        return 0;
    }

    do
    {
        printf("Choose password (min 6 chars, letters + digits): ");
        getInputAuth(pwd1, sizeof(pwd1));
        printf("Confirm password: ");
        getInputAuth(pwd2, sizeof(pwd2));
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
    strncpy(p.password, pwd1, sizeof(p.password)-1);

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

    if (!savePatientProfile(&p))
    {
        printf("Failed to save patient profile.\n");
        return 0;
    }

    printf("Signup successful!\n");
    return 1;
}

int doctorSignup(void)
{
    printf("\n=== Doctor Signup ===\n");
    Doctor d = {0};
    char pwd1[64], pwd2[64];

    d.id = getNextId(DOCTOR_FILE, sizeof(Doctor));
    printf("Your Doctor ID: %d\n", d.id);

    printf("Choose username: ");
    getInputAuth(d.username, sizeof(d.username));
    if (strlen(d.username) == 0)
    {
        printf("Username cannot be empty.\n");
        return 0;
    }
    if (usernameExistsGlobally(d.username))
    {
        printf("Username already exists. Choose a different username.\n");
        return 0;
    }

    do
    {
        printf("Choose password (min 6 chars, letters + digits): ");
        getInputAuth(pwd1, sizeof(pwd1));
        printf("Confirm password: ");
        getInputAuth(pwd2, sizeof(pwd2));
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
    strncpy(d.password, pwd1, sizeof(d.password)-1);

    printf("\nEnter doctor profile details:\n");
    printf("Name: ");
    getInputAuth(d.name, sizeof(d.name));
    printf("Specialization: ");
    getInputAuth(d.specialization, sizeof(d.specialization));
    printf("Phone: ");
    getInputAuth(d.phone, sizeof(d.phone));

    if (!saveDoctorProfile(&d))
    {
        printf("Failed to save doctor profile.\n");
        return 0;
    }

    printf("Signup successful!\n");
    return 1;
}

int adminSignup(void)
{
    printf("\n=== Admin Signup ===\n");
    Admin a = {0};
    char pwd1[64], pwd2[64];

    a.id = getNextId(ADMIN_FILE, sizeof(Admin));
    printf("Your Admin ID: %d\n", a.id);

    printf("Choose username: ");
    getInputAuth(a.username, sizeof(a.username));
    if (strlen(a.username) == 0)
    {
        printf("Username cannot be empty.\n");
        return 0;
    }
    if (usernameExistsGlobally(a.username))
    {
        printf("Username already exists. Choose a different username.\n");
        return 0;
    }

    do
    {
        printf("Choose password (min 6 chars, letters + digits): ");
        getInputAuth(pwd1, sizeof(pwd1));
        printf("Confirm password: ");
        getInputAuth(pwd2, sizeof(pwd2));
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
    strncpy(a.password, pwd1, sizeof(a.password)-1);

    printf("Name: ");
    getInputAuth(a.name, sizeof(a.name));

    if (!saveAdminProfile(&a))
    {
        printf("Failed to save admin profile.\n");
        return 0;
    }

    printf("Signup successful!\n");
    return 1;
}

/* ---------------- Logins ---------------- */

int patientLogin(void)
{
    char username[64], password[64];
    int id = 0;

    printf("\n=== Patient Login ===\n");
    printf("Username: ");
    getInputAuth(username, sizeof(username));
    printf("Password: ");
    getInputAuth(password, sizeof(password));

    if (checkCredentialProfile(PATIENT_FILE, username, password, &id, 0))
    {
        /* profile contains ID already */
        printf("Login successful.\n");
        patientPortal(id);
        return 1;
    }
    printf("Login failed.\n");
    return 0;
}

int doctorLogin(void)
{
    char username[64], password[64];
    int id = 0;

    printf("\n=== Doctor Login ===\n");
    printf("Username: ");
    getInputAuth(username, sizeof(username));
    printf("Password: ");
    getInputAuth(password, sizeof(password));

    if (checkCredentialProfile(DOCTOR_FILE, username, password, &id, 1))
    {
        printf("Login successful.\n");
        doctorPortal(id);
        return 1;
    }
    printf("Login failed.\n");
    return 0;
}

int adminLogin(void)
{
    char username[64], password[64];
    int id = 0;

    printf("\n=== Admin Login ===\n");
    printf("Username: ");
    getInputAuth(username, sizeof(username));
    printf("Password: ");
    getInputAuth(password, sizeof(password));

    if (checkCredentialProfile(ADMIN_FILE, username, password, &id, 2))
    {
        printf("Login successful.\n");
        adminPortal(id);
        return 1;
    }
    printf("Login failed.\n");
    return 0;
}

/* ---------------- Portals (minimal menus calling crud functions) ---------------- */

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
            /* show appointments for this patient */
            {
                FILE *fp = fopen(APPOINTMENT_FILE, "rb");
                if (!fp) { printf("No appointments found.\n"); break; }
                Appointment a;
                printf("\n%-5s %-10s %-20s %-25s %-10s\n",
                       "ID", "Doctor", "Date/Time", "Reason", "Status");
                int found = 0;
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
                if (!found) printf("No appointments found.\n");
            }
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
            {
                FILE *fp = fopen(APPOINTMENT_FILE, "rb");
                if (!fp) { printf("No appointments found.\n"); break; }
                Appointment a;
                printf("\n%-5s %-10s %-20s %-25s %-10s\n",
                       "ID", "Patient", "Date/Time", "Reason", "Status");
                int found = 0;
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
                if (!found) printf("No appointments found.\n");
            }
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
            }
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
        printf("6) Billing Management\n");
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
            }
            break;
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
                        {
                            FILE *fp = fopen(APPOINTMENT_FILE, "rb");
                            if (!fp) { printf("No appointments found.\n"); break; }
                            Appointment a;
                            printf("\n%-5s %-10s %-10s %-20s %-25s %-10s\n",
                                   "ID", "Patient", "Doctor", "Date/Time", "Reason", "Status");
                            while (fread(&a, sizeof(Appointment), 1, fp))
                                printf("%-5d %-10d %-10d %-20s %-25s %-10s\n",
                                       a.id, a.patient_id, a.doctor_id, a.appointment_date, a.reason, a.status);
                            fclose(fp);
                        }
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
            }
            break;
        case 5:
            {
                int sub = -1;
                while (sub != 0)
                {
                    printf("\n=== Analytics & Reports ===\n");
                    printf("1) Total Patients & Doctors\n");
                    printf("2) Total Appointments\n");
                    printf("3) Doctor Appointment Count\n");
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
                        doctorAppointmentCount();
                        break;
                    case 0:
                        break;
                    default:
                        printf("Invalid choice.\n");
                        break;
                    }
                }
            }
            break;
        case 6:
            manageBilling();
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