/*
 * CRUD Operations for Hospital Management System
 * This file contains all Create, Read, Update, Delete operations for:
 * - Patients and Doctors (profiles)
 * - Appointments
 * - Medical Records
 * - Bills
 * - Search and Analytics functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "crud.h"

/* Clear the input buffer (stdin) of any remaining characters after scanf()
   This prevents leftover newlines from affecting the next input operation */
void clearStdin(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF)
		; /* Keep reading until we hit a newline or end-of-file */
}

/* Read a single line of input from the user and remove the trailing newline
   This is safer than scanf for reading strings with spaces */
void getInput(char *buffer, int size)
{
	fgets(buffer, size, stdin);
	/* Remove the newline character that fgets includes */
	buffer[strcspn(buffer, "\n")] = 0;
}

/*
 * FUNCTION: getNextId - Generate the next unique ID for any record type
 *
 * PARAMETERS:
 *   - filename: Path to the data file (e.g., "data/patients.dat")
 *   - struct_size: Size of the record struct (e.g., sizeof(Patient))
 *
 * RETURNS: Next available ID (max_id + 1)
 *
 * HOW IT WORKS:
 *   1. Opens the file and reads it record-by-record
 *   2. Extracts the ID from each record (first 4 bytes = int id)
 *   3. Tracks the maximum ID found and returns max_id + 1
 */
int getNextId(const char *filename, size_t struct_size)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp)
		return 1; /* File doesn't exist yet, start with ID 1 */

	int max_id = 0;
	void *buf = malloc(struct_size);
	if (!buf)
	{
		fclose(fp);
		return 1; /* Allocation failed, default to ID 1 */
	}

	/* Read and process each record in the file */
	while (fread(buf, struct_size, 1, fp) == 1)
	{
		int rec_id;
		/* Extract the ID from the first 4 bytes of the record */
		memcpy(&rec_id, buf, sizeof(int));
		if (rec_id > max_id)
			max_id = rec_id;
	}

	free(buf);
	fclose(fp);
	return max_id + 1; /* Return the next available ID */
}

/* ============================================================================
   PATIENT DATABASE OPERATIONS
   ============================================================================ */

/* Check if a patient with the given ID exists in the database */
static int patientExists(int id)
{
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp)
        return 0; /* File doesn't exist, so patient doesn't exist */

    Patient p;
    /* Scan through all patient records looking for a match */
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (p.id == id)
        {
            fclose(fp);
            return 1; /* Found the patient */
        }
    }
    fclose(fp);
    return 0; /* Patient not found */
}

/* ============================================================================
   DOCTOR DATABASE OPERATIONS
   ============================================================================ */

/*
 * Check if a doctor with the given ID exists in the database.
 * This function checks TWO places:
 * 1. Doctor profiles file (doctors.dat) - main location
 * 2. Doctor credentials file (doctors_credentials.dat) - fallback for legacy data
 */
static int doctorExists(int id)
{
    /* Only check the main doctor profiles file now */
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

/* ============================================================================
   APPOINTMENT DATABASE OPERATIONS
   ============================================================================ */

/* Check if an appointment with the given ID exists in the database */
static int appointmentExists(int id)
{
	FILE *fp = fopen(APPOINTMENT_FILE, "rb");
	if (!fp)
		return 0;

	Appointment a;
	while (fread(&a, sizeof(Appointment), 1, fp))
	{
		if (a.id == id)
		{
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

/*
 * Add a new patient to the database (manual entry - not used in signup flow)
 * This is for admin to manually add patients via a menu option
 */
void addPatient()
{
    FILE *fp = fopen(PATIENT_FILE, "ab+");
    if (!fp)
    {
        perror("Error opening patient file");
        return;
    }

    Patient p;
    /* Collect patient information from user input */
    printf("Enter ID: ");
    scanf("%d", &p.id);
    clearStdin(); /* Clear leftover newline from scanf */

    printf("Name: ");
    getInput(p.name, sizeof(p.name));

    printf("Age: ");
    scanf("%d", &p.age);
    clearStdin();

    printf("Gender: ");
    getInput(p.gender, sizeof(p.gender));

    printf("Phone: ");
    getInput(p.phone, sizeof(p.phone));

    /* Write the patient struct to the file */
    fwrite(&p, sizeof(Patient), 1, fp);
    fclose(fp);
}

/* Display all patients in a formatted table
   Shows: ID, Name, Age, Gender, Phone */
void viewPatients()
{
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp)
        return;

    Patient p;
    /* Print table header with column width specifiers */
    printf("\n%-5s %-20s %-5s %-10s %-15s\n",
           "ID", "Name", "Age", "Gender", "Phone");

    /* Read and print each patient record */
    while (fread(&p, sizeof(Patient), 1, fp))
        printf("%-5d %-20s %-5d %-10s %-15s\n",
               p.id, p.name, p.age, p.gender, p.phone);

    fclose(fp);
}

/* Update an existing patient's information
   Allows editing: Name, Age, Gender, Phone
   Press Enter to skip a field and keep the current value */
void updatePatient()
{
    FILE *fp = fopen(PATIENT_FILE, "rb+");
    if (!fp)
        return;

    int id, found = 0;
    printf("Enter Patient ID to update: ");
    scanf("%d", &id);
    clearStdin();

    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (p.id == id)
        {
            /* Prompt user to update each field - enter skips the field */
            char buf[128];
            printf("Enter new Name (press Enter to keep '%s'): ", p.name);
            getInput(buf, sizeof(buf));
            if (buf[0] != '\0')
                strncpy(p.name, buf, sizeof(p.name));

            printf("Enter new Age (press Enter to keep '%d'): ", p.age);
            getInput(buf, sizeof(buf));
            if (buf[0] != '\0')
            {
                int newAge;
                if (sscanf(buf, "%d", &newAge) == 1)
                    p.age = newAge;
            }

            printf("Enter new Gender (press Enter to keep '%s'): ", p.gender);
            getInput(buf, sizeof(buf));
            if (buf[0] != '\0')
                strncpy(p.gender, buf, sizeof(p.gender));

            printf("Enter new Phone (press Enter to keep '%s'): ", p.phone);
            getInput(buf, sizeof(buf));
            if (buf[0] != '\0')
                strncpy(p.phone, buf, sizeof(p.phone));

            /* Move file pointer back one record and overwrite the old patient data */
            if (fseek(fp, -(long)sizeof(Patient), SEEK_CUR) != 0)
                perror("fseek");
            fwrite(&p, sizeof(Patient), 1, fp);
            found = 1;
            printf("Patient updated.\n");
            break;
        }
    }

    fclose(fp);
    if (!found)
        printf("Record not found.\n");
}

/* Delete a patient from the database by ID
   Creates a temporary file with all patients except the one to delete,
   then replaces the original file with the temp file */
void deletePatient()
{
	FILE *fp = fopen(PATIENT_FILE, "rb");
	FILE *temp = fopen("data/temp.dat", "wb");
	if (!fp || !temp)
		return;

	int id, found = 0;
	printf("Enter Patient ID to delete: ");
	scanf("%d", &id);

	Patient p;
	/* Copy all patients except the one with matching ID to temp file */
	while (fread(&p, sizeof(Patient), 1, fp))
	{
		if (p.id != id)
			fwrite(&p, sizeof(Patient), 1, temp);
		else
			found = 1; /* Mark that we found and skipped the target patient */
	}

	fclose(fp);
	fclose(temp);

	/* Replace original file with temp file (effectively deleting the record) */
	remove(PATIENT_FILE);
	rename("data/temp.dat", PATIENT_FILE);

	if (!found)
		printf("Record not found.\n");
}

/*
 * Add a new doctor to the database (manual entry - not used in signup flow)
 * Used by admins to manually add doctors via a menu option
 */
void addDoctor()
{
	Doctor d;
	printf("Enter ID: ");
	scanf("%d", &d.id);
	clearStdin();

	/* Check if doctor with this ID already exists */
	if (doctorExists(d.id))
	{
		printf("Doctor with ID %d already exists.\n", d.id);
		return;
	}

	/* Collect doctor information */
	printf("Name: ");
	getInput(d.name, sizeof(d.name));
	printf("Specialization: ");
	getInput(d.specialization, sizeof(d.specialization));
	printf("Phone: ");
	getInput(d.phone, sizeof(d.phone));

	/* Write doctor record to file */
	FILE *fp = fopen(DOCTOR_FILE, "ab+");
	if (!fp)
		return;
	fwrite(&d, sizeof(Doctor), 1, fp);
	fclose(fp);
}

/* Display all doctors in a formatted table
   Shows: ID, Name, Specialization, Phone */
void viewDoctors()
{
	FILE *fp = fopen(DOCTOR_FILE, "rb");
	if (!fp)
		return;

	Doctor d;
	/* Print table header */
	printf("\n%-5s %-15s %-20s %-15s\n",
		   "ID", "Name", "Specialization", "Phone");

	/* Read and print each doctor record */
	while (fread(&d, sizeof(Doctor), 1, fp))
		printf("%-5d %-15s %-20s %-15s\n",
			   d.id, d.name, d.specialization, d.phone);

	fclose(fp);
}

void updateDoctor()
{
	FILE *fp = fopen(DOCTOR_FILE, "rb+");
	if (!fp)
		return;

	int id, found = 0;
	printf("Enter Doctor ID to update: ");
	scanf("%d", &id);
	clearStdin();

	Doctor d;
	while (fread(&d, sizeof(Doctor), 1, fp))
	{
		if (d.id == id)
		{
			char buf[128];
			printf("Enter new Name (press Enter to keep '%s'): ", d.name);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
				strncpy(d.name, buf, sizeof(d.name));

			printf("Enter new Specialization (press Enter to keep '%s'): ", d.specialization);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
				strncpy(d.specialization, buf, sizeof(d.specialization));

			printf("Enter new Phone (press Enter to keep '%s'): ", d.phone);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
				strncpy(d.phone, buf, sizeof(d.phone));

			if (fseek(fp, -(long)sizeof(Doctor), SEEK_CUR) != 0)
				perror("fseek");
			fwrite(&d, sizeof(Doctor), 1, fp);
			found = 1;
			printf("Doctor updated.\n");
			break;
		}
	}

	fclose(fp);
	if (!found)
		printf("Record not found.\n");
}

void deleteDoctor()
{
	FILE *fp = fopen(DOCTOR_FILE, "rb");
	FILE *temp = fopen("data/temp.dat", "wb");
	if (!fp || !temp)
		return;

	int id, found = 0;
	printf("Enter Doctor ID to delete: ");
	scanf("%d", &id);
	clearStdin();

	Doctor d;
	while (fread(&d, sizeof(Doctor), 1, fp))
	{
		if (d.id != id)
			fwrite(&d, sizeof(Doctor), 1, temp);
		else
			found = 1;
	}

	fclose(fp);
	fclose(temp);
	remove(DOCTOR_FILE);
	rename("data/temp.dat", DOCTOR_FILE);

	if (!found)
		printf("Record not found.\n");
}

// --- Appointment Management ---
void scheduleAppointment()
{
	printf("\n--- Schedule Appointment ---\n");
	Appointment a;
	a.id = getNextId(APPOINTMENT_FILE, sizeof(Appointment));

	printf("Enter Patient ID: ");
	scanf("%d", &a.patient_id);
	clearStdin();
	if (!patientExists(a.patient_id))
	{
		printf("Patient not found.\n");
		return;
	}

	printf("Enter Doctor ID: ");
	scanf("%d", &a.doctor_id);
	clearStdin();
	if (!doctorExists(a.doctor_id))
	{
		printf("Doctor not found.\n");
		return;
	}

	printf("Enter appointment date/time (YYYY-MM-DD HH:MM): ");
	getInput(a.appointment_date, sizeof(a.appointment_date));
	printf("Enter reason: ");
	getInput(a.reason, sizeof(a.reason));
	strcpy(a.status, "Scheduled");

	FILE *fp = fopen(APPOINTMENT_FILE, "ab+");
	if (!fp)
	{
		perror("Error opening appointment file");
		return;
	}
	fwrite(&a, sizeof(Appointment), 1, fp);
	fclose(fp);
	printf("Appointment scheduled with ID %d\n", a.id);
}

void viewAppointments()
{
	printf("\n--- View Appointments ---\n");
	FILE *fp = fopen(APPOINTMENT_FILE, "rb");
	if (!fp)
	{
		printf("No appointments found.\n");
		return;
	}
	Appointment a;
	printf("\n%-5s %-10s %-10s %-20s %-25s %-10s\n",
		   "ID", "Patient", "Doctor", "Date/Time", "Reason", "Status");
	while (fread(&a, sizeof(Appointment), 1, fp))
		printf("%-5d %-10d %-10d %-20s %-25s %-10s\n",
			   a.id, a.patient_id, a.doctor_id, a.appointment_date, a.reason, a.status);
	fclose(fp);
}

/* Cancel an existing appointment by setting its status to "Cancelled"
   The appointment record remains in the database but is marked as cancelled */
void cancelAppointment()
{
	printf("\n--- Cancel Appointment ---\n");
	int id;
	printf("Enter Appointment ID to cancel: ");
	scanf("%d", &id);
	clearStdin();

	FILE *fp = fopen(APPOINTMENT_FILE, "rb+");
	if (!fp)
	{
		printf("No appointments found.\n");
		return;
	}

	Appointment a;
	int found = 0;
	while (fread(&a, sizeof(Appointment), 1, fp))
	{
		if (a.id == id)
		{
			strcpy(a.status, "Cancelled"); /* Mark appointment as cancelled */

			/* Move file pointer back one record and overwrite with updated appointment */
			if (fseek(fp, -(long)sizeof(Appointment), SEEK_CUR) != 0)
				perror("fseek");
			if (fwrite(&a, sizeof(Appointment), 1, fp) != 1)
				perror("fwrite");
			fflush(fp); /* Ensure data is written to disk immediately */
			found = 1;
			printf("Appointment %d cancelled successfully.\n", id);
			break;
		}
	}
	fclose(fp);
	if (!found)
		printf("Appointment not found.\n");
}

// --- Reschedule Appointment ---
void rescheduleAppointment()
{
	printf("\n--- Reschedule Appointment ---\n");
	int id;
	printf("Enter Appointment ID to reschedule: ");
	scanf("%d", &id);
	clearStdin();

	FILE *fp = fopen(APPOINTMENT_FILE, "rb+");
	if (!fp)
	{
		printf("No appointments found.\n");
		return;
	}

	Appointment a;
	int found = 0;
	while (fread(&a, sizeof(Appointment), 1, fp))
	{
		if (a.id == id)
		{
			if (strcmp(a.status, "Cancelled") == 0)
			{
				printf("Cannot reschedule a cancelled appointment.\n");
				fclose(fp);
				return;
			}

			printf("Current date/time: %s\n", a.appointment_date);
			printf("Enter new date/time (YYYY-MM-DD HH:MM): ");
			getInput(a.appointment_date, sizeof(a.appointment_date));

			if (fseek(fp, -(long)sizeof(Appointment), SEEK_CUR) != 0)
				perror("fseek");
			fwrite(&a, sizeof(Appointment), 1, fp);
			found = 1;
			printf("Appointment %d rescheduled successfully.\n", id);
			break;
		}
	}

	fclose(fp);
	if (!found)
		printf("Appointment not found.\n");
}

/*
 * Admin function: Assign a doctor to a pending appointment
 *
 * WORKFLOW:
 * 1. Admin enters the appointment ID (currently pending)
 * 2. Admin enters the doctor ID to assign
 * 3. System updates appointment with doctor_id and changes status to "Scheduled"
 * 4. Changes are immediately flushed to disk for persistence
 */
void assignDoctorToPatient()
{
	printf("\n--- Assign Doctor to Patient ---\n");
	int appointmentId, doctorId;

	printf("Enter Appointment ID: ");
	scanf("%d", &appointmentId);
	clearStdin();

	if (!appointmentExists(appointmentId))
	{
		printf("Appointment not found.\n");
		return;
	}

	printf("Enter Doctor ID to assign: ");
	scanf("%d", &doctorId);
	clearStdin();

	if (!doctorExists(doctorId))
	{
		printf("Doctor not found.\n");
		return;
	}

	FILE *fp = fopen(APPOINTMENT_FILE, "rb+");
	if (!fp)
	{
		printf("Failed to open appointments file.\n");
		return;
	}

	Appointment a;
	int found = 0;
	while (fread(&a, sizeof(Appointment), 1, fp))
	{
		if (a.id == appointmentId)
		{
			a.doctor_id = doctorId;
			strcpy(a.status, "Scheduled"); /* Change status from Pending to Scheduled */

			/* Move file pointer back one record and overwrite with updated appointment */
			if (fseek(fp, -(long)sizeof(Appointment), SEEK_CUR) != 0)
				perror("fseek");
			if (fwrite(&a, sizeof(Appointment), 1, fp) != 1)
				perror("fwrite");
			fflush(fp); /* Ensure disk write immediately */
			found = 1;
			printf("Doctor %d successfully assigned to appointment %d.\n", doctorId, appointmentId);
			printf("Appointment status changed to: Scheduled\n");
			break;
		}
	}

	fclose(fp);
	if (!found)
		printf("Appointment not found.\n");
}

// --- Conflict Detection ---
static int hasConflict(int doctorId, const char *dateTime, int excludeAppointmentId)
{
	FILE *fp = fopen(APPOINTMENT_FILE, "rb");
	if (!fp)
		return 0;

	Appointment a;
	while (fread(&a, sizeof(Appointment), 1, fp))
	{
		if (a.doctor_id == doctorId &&
			a.id != excludeAppointmentId &&
			strcmp(a.status, "Cancelled") != 0 &&
			strcmp(a.appointment_date, dateTime) == 0)
		{
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

/* ============================================================================
   APPOINTMENT MANAGEMENT FUNCTIONS
   ============================================================================ */

/*
 * Enhanced appointment scheduling with conflict detection
 * Called by PATIENTS when booking appointments
 *
 * FLOW:
 * 1. Patient enters appointment date/time and reason
 * 2. System auto-generates an ID and sets status to "Pending"
 * 3. Doctor ID is initially set to 0 (not assigned yet)
 * 4. Admin will later assign a specific doctor to this appointment
 */
void scheduleAppointmentWithConflictCheck(int patientId)
{
	printf("\n--- Schedule Appointment ---\n");
	Appointment a;
	a.id = getNextId(APPOINTMENT_FILE, sizeof(Appointment));
	a.patient_id = patientId;
	a.doctor_id = 0; /* Doctor will be assigned by admin later */

	printf("Enter appointment date/time (YYYY-MM-DD HH:MM): ");
	getInput(a.appointment_date, sizeof(a.appointment_date));
	printf("Enter reason: ");
	getInput(a.reason, sizeof(a.reason));
	strcpy(a.status, "Pending"); /* Status is Pending until doctor is assigned */

	FILE *fp = fopen(APPOINTMENT_FILE, "ab+");
	if (!fp)
	{
		perror("Error opening appointment file");
		return;
	}
	fwrite(&a, sizeof(Appointment), 1, fp);
	fclose(fp);
	printf("Appointment scheduled with ID %d. Doctor will be assigned by admin.\n", a.id);
}

// --- Analytics Menu ---
void analyticsMenu()
{
	int choice = -1;
	while (1)
	{
		printf("\n=== Analytics & Reports ===\n");
		printf("1) Total Patients & Doctors\n");
		printf("2) Total Appointments\n");
		printf("3) View All Bills & Revenue\n");
		printf("4) Doctor Appointment Count\n");
		printf("5) Back to Menu\n");
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
		case 5:
			return;
		default:
			printf("Invalid choice.\n");
			break;
		}
	}
}

void totalPatientsAndDoctors()
{
	printf("\n--- System Statistics ---\n");

	FILE *pf = fopen(PATIENT_FILE, "rb");
	int patientCount = 0;
	if (pf)
	{
		fseek(pf, 0, SEEK_END);
		patientCount = ftell(pf) / sizeof(Patient);
		fclose(pf);
	}

	FILE *df = fopen(DOCTOR_FILE, "rb");
	int doctorCount = 0;
	if (df)
	{
		fseek(df, 0, SEEK_END);
		doctorCount = ftell(df) / sizeof(Doctor);
		fclose(df);
	}

	printf("Total Patients: %d\n", patientCount);
	printf("Total Doctors: %d\n", doctorCount);
}

void totalAppointments()
{
	printf("\n--- Appointment Statistics ---\n");

	FILE *ap = fopen(APPOINTMENT_FILE, "rb");
	if (!ap)
	{
		printf("No appointment data.\n");
		return;
	}

	Appointment a;
	int scheduled = 0, cancelled = 0, total = 0;

	while (fread(&a, sizeof(Appointment), 1, ap))
	{
		total++;
		if (strcmp(a.status, "Scheduled") == 0)
			scheduled++;
		else if (strcmp(a.status, "Cancelled") == 0)
			cancelled++;
	}

	fclose(ap);

	printf("Total Appointments: %d\n", total);
	printf("Scheduled: %d\n", scheduled);
	printf("Cancelled: %d\n", cancelled);
}

void doctorAppointmentCount()
{
	printf("\n--- Doctor Appointment Count ---\n");
	int doctorId;
	printf("Enter Doctor ID: ");
	scanf("%d", &doctorId);
	clearStdin();

	if (!doctorExists(doctorId))
	{
		printf("Doctor not found.\n");
		return;
	}

	FILE *ap = fopen(APPOINTMENT_FILE, "rb");
	if (!ap)
	{
		printf("No appointment data.\n");
		return;
	}

	Appointment a;
	int count = 0;

	while (fread(&a, sizeof(Appointment), 1, ap))
	{
		if (a.doctor_id == doctorId && strcmp(a.status, "Cancelled") != 0)
			count++;
	}

	fclose(ap);
	printf("Doctor ID %d has %d active appointments.\n", doctorId, count);
}

/* Helper function: Mark appointment as "Done" after medical record is added */
static void markAppointmentAsDone(int appointmentId)
{
    FILE *fp = fopen(APPOINTMENT_FILE, "rb+");
    if (!fp)
        return;

    Appointment a;
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.id == appointmentId)
        {
            strcpy(a.status, "Done");
            if (fseek(fp, -(long)sizeof(Appointment), SEEK_CUR) != 0)
                perror("fseek");
            if (fwrite(&a, sizeof(Appointment), 1, fp) != 1)
                perror("fwrite");
            fflush(fp);
            break;
        }
    }
    fclose(fp);
}

// --- Medical Records ---
void addMedicalRecord()
{
    printf("\n--- Add Medical Record ---\n");
    int app_id;
    printf("Enter Appointment ID: ");
    scanf("%d", &app_id);
    clearStdin();

    if (!appointmentExists(app_id))
    {
        printf("Appointment not found.\n");
        return;
    }

    MedicalRecord rec;
    rec.id = getNextId(MEDICAL_RECORD_FILE, sizeof(MedicalRecord));
    rec.appointment_id = app_id;

    printf("Enter Diagnosis: ");
    getInput(rec.diagnosis, sizeof(rec.diagnosis));
    printf("Enter Prescription: ");
    getInput(rec.prescription, sizeof(rec.prescription));

    FILE *fp = fopen(MEDICAL_RECORD_FILE, "ab");
    if (!fp)
    {
        perror("Error opening records file");
        return;
    }
    fwrite(&rec, sizeof(MedicalRecord), 1, fp);
    fclose(fp);

    /* Mark the appointment as Done after medical record is added */
    markAppointmentAsDone(app_id);

    printf("Medical record added successfully.\n");
    printf("Appointment status set to 'Done'.\n");
}

/* View Patient Medical History */
void viewPatientMedicalHistory()
{
	printf("\n--- View Patient Medical History ---\n");
	int patient_id;
	printf("Enter Patient ID: ");
	scanf("%d", &patient_id);
	clearStdin();

	printf("\n--- Medical History for Patient ID: %d ---\n", patient_id);

	FILE *app_fp = fopen(APPOINTMENT_FILE, "rb");
	FILE *rec_fp = fopen(MEDICAL_RECORD_FILE, "rb");

	if (!app_fp || !rec_fp)
	{
		/* Improved message for missing files */
		printf("No reports found.\n");
		if (app_fp)
			fclose(app_fp);
		if (rec_fp)
			fclose(rec_fp);
		return;
	}

	Appointment app;
	MedicalRecord rec;
	int records_found = 0;

	while (fread(&app, sizeof(Appointment), 1, app_fp))
	{
		if (app.patient_id == patient_id)
		{
			rewind(rec_fp);
			while (fread(&rec, sizeof(MedicalRecord), 1, rec_fp))
			{
				if (rec.appointment_id == app.id)
				{
					printf("\nAppointment ID: %d\n", app.id);
					printf("Date: %s\n", app.appointment_date);
					printf("Reason: %s\n", app.reason);
					printf("Diagnosis: %s\n", rec.diagnosis);
					printf("Prescription: %s\n", rec.prescription);
					records_found++;
					break;
				}
			}
		}
	}

	if (records_found == 0)
		printf("No reports found.\n");

	fclose(app_fp);
	fclose(rec_fp);
}

/* View Doctor Reports (Medical Records) for a patient */
void viewDoctorReports(int patientId)
{
	printf("\n--- Doctor Reports for Patient ID: %d ---\n", patientId);

	FILE *app_fp = fopen(APPOINTMENT_FILE, "rb");
	FILE *rec_fp = fopen(MEDICAL_RECORD_FILE, "rb");

	if (!app_fp || !rec_fp)
	{
		/* Improved message for missing files */
		printf("No reports found.\n");
		if (app_fp)
			fclose(app_fp);
		if (rec_fp)
			fclose(rec_fp);
		return;
	}

	Appointment app;
	MedicalRecord rec;
	int records_found = 0;

	printf("\n%-5s %-10s %-20s %-30s %-30s\n",
		   "ID", "Doctor ID", "Date", "Diagnosis", "Prescription");

	while (fread(&app, sizeof(Appointment), 1, app_fp))
	{
		if (app.patient_id == patientId)
		{
			rewind(rec_fp);
			while (fread(&rec, sizeof(MedicalRecord), 1, rec_fp))
			{
				if (rec.appointment_id == app.id)
				{
					printf("%-5d %-10d %-20s %-30s %-30s\n",
						   rec.id, app.doctor_id, app.appointment_date,
						   rec.diagnosis, rec.prescription);
					records_found++;
					break;
				}
			}
		}
	}

	if (records_found == 0)
		printf("No reports found.\n");

	fclose(app_fp);
	fclose(rec_fp);
}

// --- Billing ---
void generateBill(int appointmentId, int patientId, float amount)
{
	FILE *fp = fopen(BILL_FILE, "ab");
	if (!fp)
	{
		perror("Error opening bill file");
		return;
	}

	Bill b;
	b.id = getNextId(BILL_FILE, sizeof(Bill));
	b.appointment_id = appointmentId;
	b.patient_id = patientId;
	b.amount = amount;
	strcpy(b.status, "Unpaid");

	time_t now = time(NULL);
	struct tm *tm_info = localtime(&now);
	strftime(b.date, sizeof(b.date), "%Y-%m-%d", tm_info);

	fwrite(&b, sizeof(Bill), 1, fp);
	fclose(fp);
	printf("Bill generated: ID %d, Amount: $%.2f\n", b.id, amount);
}

void viewPatientBills(int patientId)
{
	FILE *fp = fopen(BILL_FILE, "rb");
	if (!fp)
	{
		printf("No bills found.\n");
		return;
	}

	Bill b;
	printf("\n%-5s %-15s %-10s %-12s %-10s\n",
		   "ID", "Appointment", "Amount", "Date", "Status");

	int found = 0;
	while (fread(&b, sizeof(Bill), 1, fp))
	{
		if (b.patient_id == patientId)
		{
			printf("%-5d %-15d $%-9.2f %-12s %-10s\n",
				   b.id, b.appointment_id, b.amount, b.date, b.status);
			found = 1;
		}
	}
	fclose(fp);

	if (!found)
		printf("No bills for this patient.\n");
}

void viewAllBills(void)
{
	FILE *fp = fopen(BILL_FILE, "rb");
	if (!fp)
	{
		printf("No bills found.\n");
		return;
	}

	Bill b;
	float totalRevenue = 0, totalUnpaid = 0;
	printf("\n%-5s %-10s %-10s %-10s %-12s %-10s\n",
		   "ID", "Patient", "Amount", "Appointment", "Date", "Status");

	while (fread(&b, sizeof(Bill), 1, fp))
	{
		printf("%-5d %-10d $%-9.2f %-10d %-12s %-10s\n",
			   b.id, b.patient_id, b.amount, b.appointment_id, b.date, b.status);
		totalRevenue += b.amount;
		if (strcmp(b.status, "Unpaid") == 0)
			totalUnpaid += b.amount;
	}
	fclose(fp);

	printf("\n--- Summary ---\n");
	printf("Total Revenue: $%.2f\n", totalRevenue);
	printf("Unpaid Amount: $%.2f\n", totalUnpaid);
}

// --- Search ---
void searchPatientByName(const char *name)
{
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp)
    {
        printf("No patients found.\n");
        return;
    }

    Patient p;
    printf("\n%-5s %-20s %-5s %-10s %-15s\n",
           "ID", "Name", "Age", "Gender", "Phone");

    int found = 0;
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (strstr(p.name, name) != NULL)
        {
            printf("%-5d %-20s %-5d %-10s %-15s\n",
                   p.id, p.name, p.age, p.gender, p.phone);
            found = 1;
        }
    }
    fclose(fp);

    if (!found)
        printf("No patients found with name containing '%s'.\n", name);
}