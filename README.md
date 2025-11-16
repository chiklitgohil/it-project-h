# Hospital Management System

> Course: IT113

> Group ID: 30

## Project Overview
An AI generated comprehensive hospital management system built in C that handles patient records, doctor management, appointment scheduling, medical records, billing, and analytics with role-based access control.

## Project Structure
```
hospital-management-system/
├── main.c                 # Main entry point with menu system
├── common.c              # Utility functions (clearStdin, getInput, getNextId)
├── common.h              # Common data structures and function declarations
├── auth.c                # Authentication system (login/signup for all roles)
├── auth.h                # Authentication function declarations
├── patients.c            # Patient CRUD operations
├── patients.h            # Patient function declarations
├── doctors.c             # Doctor CRUD operations
├── doctors.h             # Doctor function declarations
├── appointments.c        # Appointment scheduling with conflict detection
├── appointments.h        # Appointment function declarations
├── medical_records.c     # Medical records and billing management
├── medical_records.h     # Medical records function declarations
├── analytics.c           # Search and analytics functions
├── analytics.h           # Analytics function declarations
├── hospital_manager.exe  # Compiled executable (Windows)
├── data/                 # Data directory (created at runtime)
│   ├── patients.dat
│   ├── doctors.dat
│   ├── admins.dat
│   ├── appointments.dat
│   ├── medical_records.dat
│   └── bills.dat
└── README.md             # This file
```


## Compilation Instructions

### Windows (PowerShell/Command Prompt)
```powershell
cd C:\path\to\hospital-management-system
gcc *.c -o hospital_manager.exe
```

### Linux/Mac (Terminal/Bash)
```bash
cd /path/to/hospital-management-system
gcc *.c -o hospital_manager
chmod +x hospital_manager
```

---

## Execution Instructions

### Windows
```powershell
.\hospital_manager.exe
```

### Linux/Mac
```bash
./hospital_manager
```

---

## Default Test Data

The system creates empty data files on first run. You can:
1. Create test accounts through the signup system
2. Add patients and doctors through admin portal
3. Schedule appointments and generate bills
4. View analytics and reports

**Sample Data Already Present for testing**
Patients

Username: bhagya
Password: bhagya01
Username: ompatel
Password: ompatel05

Doctors

Username: chiklit
Password: chiklit01
Username: rachitsir
Password: rachit07

Admin

Username: dau
Password: dau2025
Username: sanjayshrivastav
Password: sanjay10

---


## Notes

- WARNING: The folder "data" stores patient and doctor records. Do not delete this folder.
- Each role (Patient, Doctor, Admin) has separate login credentials
- The system uses file-based storage; no database required

---