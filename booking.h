#ifndef BOOKING_H
#define BOOKING_H

#include <stdbool.h>
#include <stddef.h>

/*
 * Simple Appointment Scheduling API (basic C)
 * - Allows booking, cancelling and rescheduling appointments
 * - Prevents double-booking the same doctor at the same date/time-slot
 * - Uses simple fixed-size arrays (no dynamic allocation) for clarity
 *
 * This header is intentionally heavily commented for learning and
 * easy integration into the rest of the project.
 */

/* Constants - adjust sizes to fit small projects */
#define MAX_DOCTORS 20
#define MAX_PATIENTS 100
#define MAX_APPOINTMENTS 200
#define DATE_STR_LEN 16 /* e.g. "2025-11-04" */

/* Time slot representation: integer hour in 24h (e.g. 9 for 9:00-9:59)
 * We keep it simple: each appointment occupies one hour slot */
typedef int TimeSlot;

/* Appointment structure */
typedef struct {
	int id;                     /* unique appointment id (>=1) */
	int doctor_id;              /* doctor index (0..MAX_DOCTORS-1) */
	int patient_id;             /* patient index (0..MAX_PATIENTS-1) */
	char date[DATE_STR_LEN];    /* simple date string YYYY-MM-DD */
	TimeSlot slot;             /* hour slot (0..23) */
	bool active;               /* whether appointment is active */
} Appointment;

/* Simple return codes for operations */
typedef enum {
	BOOK_OK = 0,
	BOOK_ERR_CONFLICT = 1,
	BOOK_ERR_FULL = 2,
	BOOK_ERR_INVALID = 3,
	BOOK_ERR_NOT_FOUND = 4
} BookResult;

/* Initialize the booking system. Must be called before other ops. */
void initBookingSystem(void);

/* Book an appointment. On success returns BOOK_OK and sets *out_id to new id.
 * Parameters:
 *  - doctor_id: index of doctor (0..MAX_DOCTORS-1)
 *  - patient_id: index of patient (0..MAX_PATIENTS-1)
 *  - date: string YYYY-MM-DD
 *  - slot: hour 0..23
 */
BookResult bookAppointment(int doctor_id, int patient_id, const char *date, TimeSlot slot, int *out_id);

/* Cancel appointment by appointment id (returned from bookAppointment) */
BookResult cancelAppointment(int appointment_id);

/* Reschedule an existing appointment to a new date/slot (checks conflicts). */
BookResult rescheduleAppointment(int appointment_id, const char *new_date, TimeSlot new_slot);

/* Check availability: returns true if doctor is free on given date+slot */
bool isDoctorAvailable(int doctor_id, const char *date, TimeSlot slot);

/* Utility: print all appointments to stdout (for debug/demo) */
void printAppointments(void);

/* Accessor for appointments array - useful for unit tests or other code.
 * It returns pointer to internal array (do not free). Use only for read.
 */
const Appointment *getAppointments(size_t *out_count);

#endif /* BOOKING_H */
