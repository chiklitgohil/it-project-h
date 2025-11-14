/*
 * appointments.c - Appointment Scheduling & Management
 * Handles scheduling, rescheduling, assigning doctors, and conflict detection
 */

#include "common.h"
#include "appointments.h"
#include "patients.h"
#include "doctors.h"

/* Truncate string for display */
static void truncateStr(const char *src, int maxlen, char *dst)
{
    if (maxlen <= 0)
        return;
    if ((int)strlen(src) < maxlen)
    {
        strncpy(dst, src, maxlen);
        dst[maxlen - 1] = '\0';
        return;
    }
    if (maxlen <= 4)
    {
        strncpy(dst, src, maxlen - 1);
        dst[maxlen - 1] = '\0';
        return;
    }
    int copy = maxlen - 4;
    strncpy(dst, src, copy);
    dst[copy] = '.';
    dst[copy + 1] = '.';
    dst[copy + 2] = '.';
    dst[copy + 3] = '\0';
}

/* Appointment comparator: sort by appointment_date */
static int cmpAppointmentByDate(const void *x, const void *y)
{
    const Appointment *a = x, *b = y;
    return strcmp(a->appointment_date, b->appointment_date);
}

/* Parse datetime string "YYYY-MM-DD HH:MM" into time_t */
static int parseDateTime(const char *s, time_t *out_time)
{
    struct tm tm_info = {0};
    if (sscanf(s, "%d-%d-%d %d:%d",
               &tm_info.tm_year, &tm_info.tm_mon, &tm_info.tm_mday,
               &tm_info.tm_hour, &tm_info.tm_min) != 5)
        return 0;

    tm_info.tm_year -= 1900;
    tm_info.tm_mon -= 1;
    tm_info.tm_sec = 0;
    tm_info.tm_isdst = -1;

    *out_time = mktime(&tm_info);
    if (*out_time == -1)
        return 0;

    time_t now = time(NULL);
    if (*out_time < now)
        return 0;

    return 1;
}

/* Check if doctor is already booked at same date/time */
static int hasConflict(int doctorId, const char *dateTime, int excludeAppointmentId)
{
    if (doctorId == 0)
        return 0;

    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp)
        return 0;

    Appointment a;
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.id == excludeAppointmentId)
            continue;
        if (a.doctor_id == doctorId && strcmp(a.appointment_date, dateTime) == 0)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void scheduleAppointmentWithConflictCheck(int patientId)
{
    FILE *fp = fopen(APPOINTMENT_FILE, "ab+");
    if (!fp)
    {
        perror("Error opening appointment file");
        return;
    }

    Appointment a;
    a.id = getNextId(APPOINTMENT_FILE, sizeof(Appointment));
    a.patient_id = patientId;
    a.doctor_id = 0;

    while (1)
    {
        printf("Enter appointment date/time (YYYY-MM-DD HH:MM): ");
        getInput(a.appointment_date, sizeof(a.appointment_date));
        time_t t;
        if (!parseDateTime(a.appointment_date, &t))
        {
            printf("Invalid date/time or date is in the past. Try again.\n");
            continue;
        }
        break;
    }

    printf("Enter reason for appointment: ");
    getInput(a.reason, sizeof(a.reason));
    strcpy(a.status, "Scheduled");

    fwrite(&a, sizeof(Appointment), 1, fp);
    fclose(fp);
    printf("Appointment scheduled successfully. Appointment ID: %d\n", a.id);
}

void scheduleAppointment()
{
    int patientId;
    printf("Enter Patient ID: ");
    scanf("%d", &patientId);
    clearStdin();

    if (!patientExists(patientId))
    {
        printf("Patient not found.\n");
        return;
    }

    scheduleAppointmentWithConflictCheck(patientId);
}

void viewAppointments()
{
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp)
        return;

    Appointment *arr = NULL;
    size_t n = 0, cap = 0;
    Appointment a;
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (n == cap)
        {
            cap = cap ? cap * 2 : 16;
            arr = realloc(arr, cap * sizeof(Appointment));
        }
        arr[n++] = a;
    }
    fclose(fp);

    if (n == 0)
        return;

    qsort(arr, n, sizeof(Appointment), cmpAppointmentByDate);

    printf("\n%-5s %-10s %-10s %-20s %-20s %-12s\n",
           "ID", "PatID", "DocID", "Date/Time", "Reason", "Status");

    for (size_t i = 0; i < n; ++i)
    {
        char reason_trunc[15];
        truncateStr(arr[i].reason, 14, reason_trunc);
        printf("%-5d %-10d %-10d %-20s %-20s %-12s\n",
               arr[i].id, arr[i].patient_id, arr[i].doctor_id,
               arr[i].appointment_date, reason_trunc, arr[i].status);
    }
    free(arr);
}

void cancelAppointment()
{
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    FILE *temp = fopen("data/temp.dat", "wb");
    if (!fp || !temp)
        return;

    int id, found = 0;
    printf("Enter Appointment ID to cancel: ");
    scanf("%d", &id);
    clearStdin();

    Appointment a;
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.id != id)
            fwrite(&a, sizeof(Appointment), 1, temp);
        else
            found = 1;
    }

    fclose(fp);
    fclose(temp);

    remove(APPOINTMENT_FILE);
    rename("data/temp.dat", APPOINTMENT_FILE);

    if (found)
        printf("Appointment cancelled.\n");
    else
        printf("Appointment not found.\n");
}

void rescheduleAppointment()
{
    FILE *fp = fopen(APPOINTMENT_FILE, "rb+");
    if (!fp)
        return;

    int id, found = 0;
    printf("Enter Appointment ID to reschedule: ");
    scanf("%d", &id);
    clearStdin();

    Appointment a;
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.id == id)
        {
            char new_dt[20];
            while (1)
            {
                printf("Enter new date/time (YYYY-MM-DD HH:MM): ");
                getInput(new_dt, sizeof(new_dt));
                time_t t;
                if (!parseDateTime(new_dt, &t))
                {
                    printf("Invalid date/time or date is in the past. Try again.\n");
                    continue;
                }
                if (a.doctor_id != 0 && hasConflict(a.doctor_id, new_dt, a.id))
                {
                    printf("Doctor already has appointment at this time.\n");
                    continue;
                }
                break;
            }
            strcpy(a.appointment_date, new_dt);

            if (fseek(fp, -(long)sizeof(Appointment), SEEK_CUR) != 0)
                perror("fseek");
            fwrite(&a, sizeof(Appointment), 1, fp);
            found = 1;
            printf("Appointment rescheduled successfully.\n");
            break;
        }
    }

    fclose(fp);
    if (!found)
        printf("Appointment not found.\n");
}

void assignDoctorToPatient()
{
    FILE *fp = fopen(APPOINTMENT_FILE, "rb+");
    if (!fp)
        return;

    int appointmentId, doctorId, found = 0;
    printf("Enter Appointment ID: ");
    scanf("%d", &appointmentId);
    clearStdin();

    printf("Enter Doctor ID to assign: ");
    scanf("%d", &doctorId);
    clearStdin();

    if (!doctorExists(doctorId))
    {
        printf("Doctor not found.\n");
        return;
    }

    Appointment a;
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.id == appointmentId)
        {
            if (hasConflict(doctorId, a.appointment_date, a.id))
            {
                printf("Doctor %d is already booked at this date/time.\n", doctorId);
                fclose(fp);
                return;
            }

            a.doctor_id = doctorId;

            if (fseek(fp, -(long)sizeof(Appointment), SEEK_CUR) != 0)
                perror("fseek");
            fwrite(&a, sizeof(Appointment), 1, fp);
            found = 1;
            printf("Doctor %d assigned to appointment %d.\n", doctorId, appointmentId);
            break;
        }
    }

    fclose(fp);
    if (!found)
        printf("Appointment not found.\n");
}

void searchPatientsByDoctor(int doctorId)
{
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp)
        return;

    Appointment a;
    int first = 1;
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.doctor_id == doctorId)
        {
            if (first)
            {
                printf("Patients assigned to Doctor %d:\n", doctorId);
                printf("Appointment ID: %d, Patient ID: %d, Date: %s\n",
                       a.id, a.patient_id, a.appointment_date);
                first = 0;
            }
            else
            {
                printf("Appointment ID: %d, Patient ID: %d, Date: %s\n",
                       a.id, a.patient_id, a.appointment_date);
            }
        }
    }
    fclose(fp);

    if (first)
        printf("No appointments found for doctor %d.\n", doctorId);
}
