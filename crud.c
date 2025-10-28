#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

// Helper function to safely read string input and clear the buffer
void getInput(char *buffer, int size)
{
fgets(buffer, size, stdin);
buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline
}

// Small helper functions used by appointment/record features
static int patientExists(int id)
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

static int doctorExists(int id)
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

static int getNextAppointmentId()
{
FILE *fp = fopen(APPOINTMENT_FILE, "rb");
if (!fp)
return 1;
Appointment a;
int maxId = 0;
while (fread(&a, sizeof(Appointment), 1, fp))
if (a.id > maxId)
maxId = a.id;
fclose(fp);
return maxId + 1;
}

static int getNextMedicalRecordId()
{
FILE *fp = fopen(MEDICAL_RECORD_FILE, "rb");
if (!fp)
return 1;
MedicalRecord r;
int maxId = 0;
while (fread(&r, sizeof(MedicalRecord), 1, fp))
if (r.id > maxId)
maxId = r.id;
fclose(fp);
return maxId + 1;
}

void addPatient()
{
FILE *fp = fopen(PATIENT_FILE, "ab+");
if (!fp)
{
perror("Error opening patient file");
return;
}

Patient p;
char buffer[100]; // Buffer for reading string inputs

printf("Enter ID: ");
scanf("%d", &p.id);
while (getchar() != '\n')
; // Clear input buffer

printf("Name: ");
	 /* Read the full name line safely. Using scanf("%s", ...) before
		 fgets/getInput causes the subsequent fgets to read the leftover
		 newline and overwrite the name with an empty string. Use only
		 getInput (fgets wrapper) here. */
	 getInput(p.name, sizeof(p.name));
printf("Age: ");
scanf("%d", &p.age);
while (getchar() != '\n')
; // Clear input buffer
printf("Gender: ");
getInput(p.gender, sizeof(p.gender));
printf("Phone: ");
getInput(p.phone, sizeof(p.phone));
printf("Disease: ");
getInput(p.disease, sizeof(p.disease));

fwrite(&p, sizeof(Patient), 1, fp);
fclose(fp);
}

void viewPatients()
{
FILE *fp = fopen(PATIENT_FILE, "rb");
if (!fp)
return;

Patient p;
printf("\n%-5s %-15s %-5s %-10s %-15s %-20s\n",
"ID", "Name", "Age", "Gender", "Phone", "Disease");
while (fread(&p, sizeof(Patient), 1, fp))
printf("%-5d %-15s %-5d %-10s %-15s %-20s\n",
p.id, p.name, p.age, p.gender, p.phone, p.disease);

fclose(fp);
}

void updatePatient()
{
FILE *fp = fopen(PATIENT_FILE, "rb+");
if (!fp)
return;

int id, found = 0;
printf("Enter Patient ID to update: ");
scanf("%d", &id);

	while (getchar() != '\n')
		; // clear input buffer

Patient p;
while (fread(&p, sizeof(Patient), 1, fp))
{
if (p.id == id)
{
			{
				char buf[128];
				/* Name */
				printf("Enter new Name (press Enter to keep '%s'): ", p.name);
				getInput(buf, sizeof(buf));
				if (buf[0] != '\0')
					strncpy(p.name, buf, sizeof(p.name));

				/* Age */
				printf("Enter new Age (press Enter to keep '%d'): ", p.age);
				getInput(buf, sizeof(buf));
				if (buf[0] != '\0')
				{
					int newAge;
					if (sscanf(buf, "%d", &newAge) == 1)
						p.age = newAge;
					else
						printf("Invalid age input. Keeping previous value.\n");
				}

				/* Gender */
				printf("Enter new Gender (press Enter to keep '%s'): ", p.gender);
				getInput(buf, sizeof(buf));
				if (buf[0] != '\0')
					strncpy(p.gender, buf, sizeof(p.gender));

				/* Phone */
				printf("Enter new Phone (press Enter to keep '%s'): ", p.phone);
				getInput(buf, sizeof(buf));
				if (buf[0] != '\0')
					strncpy(p.phone, buf, sizeof(p.phone));

				/* Disease */
				printf("Enter new Disease (press Enter to keep '%s'): ", p.disease);
				getInput(buf, sizeof(buf));
				if (buf[0] != '\0')
					strncpy(p.disease, buf, sizeof(p.disease));
			}

fseek(fp, -sizeof(Patient), SEEK_CUR);
fwrite(&p, sizeof(Patient), 1, fp);
found = 1;
			/* Show the updated patient to the user */
			printf("\nUpdated Patient:\n");
			printf("ID: %d\n", p.id);
			printf("Name: %s\n", p.name);
			printf("Age: %d\n", p.age);
			printf("Gender: %s\n", p.gender);
			printf("Phone: %s\n", p.phone);
			printf("Disease: %s\n", p.disease);
break;
}
}

fclose(fp);
if (!found)
printf("Record not found.\n");
}

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
while (fread(&p, sizeof(Patient), 1, fp))
{
if (p.id != id)
fwrite(&p, sizeof(Patient), 1, temp);
else
found = 1;
}

fclose(fp);
fclose(temp);
remove(PATIENT_FILE);
rename("data/temp.dat", PATIENT_FILE);

if (!found)
printf("Record not found.\n");
}

void addDoctor()
{
	Doctor d;
	printf("Enter ID: ");
	if (scanf("%d", &d.id) != 1)
	{
		while (getchar() != '\n')
			;
		printf("Invalid ID input.\n");
		return;
	}
	/* Clear newline left by scanf("%d") before using getInput */
	while (getchar() != '\n')
		;

	/* Check duplicate doctor ID */
	if (doctorExists(d.id))
	{
		printf("Doctor with ID %d already exists.\n", d.id);
		return;
	}

	printf("Name: ");
	getInput(d.name, sizeof(d.name));
	printf("Specialization: ");
	getInput(d.specialization, sizeof(d.specialization));
	printf("Phone: ");
	getInput(d.phone, sizeof(d.phone));

	FILE *fp = fopen(DOCTOR_FILE, "ab+");
	if (!fp)
		return;
	fwrite(&d, sizeof(Doctor), 1, fp);
	fclose(fp);
}

void viewDoctors()
{
FILE *fp = fopen(DOCTOR_FILE, "rb");
if (!fp)
return;

Doctor d;
printf("\n%-5s %-15s %-20s %-15s\n",
"ID", "Name", "Specialization", "Phone");
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

	while (getchar() != '\n')
		; // clear input buffer

Doctor d;
while (fread(&d, sizeof(Doctor), 1, fp))
{
if (d.id == id)
{
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
			}

fseek(fp, -sizeof(Doctor), SEEK_CUR);
fwrite(&d, sizeof(Doctor), 1, fp);
found = 1;
			/* Show the updated doctor to the user */
			printf("\nUpdated Doctor:\n");
			printf("ID: %d\n", d.id);
			printf("Name: %s\n", d.name);
			printf("Specialization: %s\n", d.specialization);
			printf("Phone: %s\n", d.phone);
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

// --- Appointment Management Functions (Skeletons) ---
void scheduleAppointment()
{
printf("\n--- Schedule Appointment ---\n");
Appointment a;
// Generate ID
a.id = getNextAppointmentId();

printf("Enter Patient ID: ");
if (scanf("%d", &a.patient_id) != 1)
{
while (getchar() != '\n')
;
printf("Invalid input.\n");
return;
}
while (getchar() != '\n')
;
if (!patientExists(a.patient_id))
{
printf("Patient with ID %d does not exist.\n", a.patient_id);
return;
}

printf("Enter Doctor ID: ");
if (scanf("%d", &a.doctor_id) != 1)
{
while (getchar() != '\n')
;
printf("Invalid input.\n");
return;
}
while (getchar() != '\n')
;
if (!doctorExists(a.doctor_id))
{
printf("Doctor with ID %d does not exist.\n", a.doctor_id);
return;
}

printf("Enter appointment date/time (YYYY-MM-DD HH:MM): ");
getInput(a.appointment_date, sizeof(a.appointment_date));
printf("Enter reason: ");
getInput(a.reason, sizeof(a.reason));
strncpy(a.status, "Scheduled", sizeof(a.status));

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
{
printf("%-5d %-10d %-10d %-20s %-25s %-10s\n",
a.id, a.patient_id, a.doctor_id, a.appointment_date, a.reason, a.status);
}
fclose(fp);
}

void cancelAppointment()
{
printf("\n--- Cancel Appointment ---\n");

int id;
printf("Enter Appointment ID to cancel: ");
if (scanf("%d", &id) != 1)
{
while (getchar() != '\n')
;
printf("Invalid input.\n");
return;
}
while (getchar() != '\n')
;

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
strncpy(a.status, "Cancelled", sizeof(a.status));
fseek(fp, -sizeof(Appointment), SEEK_CUR);
fwrite(&a, sizeof(Appointment), 1, fp);
found = 1;
printf("Appointment %d cancelled.\n", id);
break;
}
}
fclose(fp);
if (!found)
printf("Appointment with ID %d not found.\n", id);
}

// --- Medical Record Management Functions (Skeletons) ---
void addMedicalRecord()
{
printf("\n--- Add Medical Record ---\n");
MedicalRecord r;
r.id = getNextMedicalRecordId();

printf("Enter Appointment ID: ");
if (scanf("%d", &r.appointment_id) != 1)
{
while (getchar() != '\n')
;
printf("Invalid input.\n");
return;
}
while (getchar() != '\n')
;

if (!appointmentExists(r.appointment_id))
{
printf("Appointment with ID %d does not exist.\n", r.appointment_id);
return;
}

printf("Enter diagnosis: ");
getInput(r.diagnosis, sizeof(r.diagnosis));
printf("Enter prescription: ");
getInput(r.prescription, sizeof(r.prescription));

FILE *fp = fopen(MEDICAL_RECORD_FILE, "ab+");
if (!fp)
{
perror("Error opening medical record file");
return;
}
fwrite(&r, sizeof(MedicalRecord), 1, fp);
fclose(fp);
printf("Medical record added with ID %d\n", r.id);
}

void viewPatientMedicalHistory()
{
printf("\n--- View Patient Medical History ---\n");
int patient_id;
printf("Enter Patient ID: ");
if (scanf("%d", &patient_id) != 1)
{
while (getchar() != '\n')
;
printf("Invalid input.\n");
return;
}
while (getchar() != '\n')
;

FILE *afp = fopen(APPOINTMENT_FILE, "rb");
if (!afp)
{
printf("No appointments found.\n");
return;
}

FILE *rfp = fopen(MEDICAL_RECORD_FILE, "rb");
// rfp may be NULL if there are no records yet

Appointment a;
int foundAny = 0;
printf("\nAppointments and Medical Records for Patient %d:\n", patient_id);
while (fread(&a, sizeof(Appointment), 1, afp))
{
if (a.patient_id == patient_id)
{
foundAny = 1;
printf("\nAppointment ID: %d\n", a.id);
printf("  Doctor ID: %d\n", a.doctor_id);
printf("  Date/Time: %s\n", a.appointment_date);
printf("  Reason: %s\n", a.reason);
printf("  Status: %s\n", a.status);

if (rfp)
{
// Search for a medical record for this appointment
rewind(rfp);
MedicalRecord r;
int recFound = 0;
while (fread(&r, sizeof(MedicalRecord), 1, rfp))
{
if (r.appointment_id == a.id)
{
recFound = 1;
printf("  Medical Record ID: %d\n", r.id);
printf("    Diagnosis: %s\n", r.diagnosis);
printf("    Prescription: %s\n", r.prescription);
break;
}
}
if (!recFound)
printf("  No medical record for this appointment.\n");
}
else
{
printf("  No medical records file found.\n");
}
}
}

if (!foundAny)
printf("No appointments found for patient %d.\n", patient_id);

if (afp)
fclose(afp);
if (rfp)
fclose(rfp);
}