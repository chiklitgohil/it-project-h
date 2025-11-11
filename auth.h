#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

/* Credential files */
#define PATIENTS_CRED_FILE "data/patients_credentials.dat"
#define DOCTORS_CRED_FILE "data/doctors_credentials.dat"
#define ADMINS_CRED_FILE "data/admins_credentials.dat"

typedef struct Credential
{
    int id; /* unified user id (same across all roles) */
    char username[64];
    char password[64]; /* stored in plain text for this simple project */
} Credential;

/* Authentication flows */
int patientSignup(void);
int doctorSignup(void);
int adminSignup(void);
int patientLogin(void);
int doctorLogin(void);
int adminLogin(void);
void patientPortal(int patientId);
void doctorPortal(int doctorId);
void adminPortal(int adminId);

/* Migration helper: move old credential files to new names if present */
void migrateCredentialFiles(void);

/* Utility - clearStdin is declared in crud.h, just use it from there */
void getInputAuth(char *buf, int size);
int validatePassword(const char *pwd);
void maskInput(char *buf, int size);

#endif /* AUTH_H */