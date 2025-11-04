#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

/* Credential files */
#define USER_CRED_FILE "data/users.dat"
#define DOCTOR_CRED_FILE "data/doctors.dat"

typedef struct Credential {
    int id;                /* patient or doctor id */
    char username[64];
    char password[64];     /* stored in plain text for this simple project */
} Credential;

/* Authentication menu & flows */
void authMenu(void);
int patientSignup(void);
int doctorSignup(void);
int patientLogin(void);
int doctorLogin(void);
void patientPortal(int patientId);
void doctorPortal(int doctorId);

/* Utility */
void clearStdin(void);
void getInputAuth(char *buf, int size);
int validatePassword(const char *pwd);
void maskInput(char *buf, int size);

#endif /* AUTH_H */