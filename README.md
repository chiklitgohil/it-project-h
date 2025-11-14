# Hospital Management System

## Project Overview
A comprehensive hospital management system built in C that handles patient records, doctor management, appointment scheduling, medical records, billing, and analytics with role-based access control.

---

## Prerequisites
- **GCC Compiler** (MinGW on Windows, GCC on Linux/Mac)
- **Windows PowerShell** or Command Prompt (for Windows)
- **Terminal/Bash** (for Linux/Mac)

---

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
├── hospital_management_system.exe  # Compiled executable (Windows)
├── data/                 # Data directory (created at runtime)
│   ├── patients.dat
│   ├── doctors.dat
│   ├── admins.dat
│   ├── appointments.dat
│   ├── medical_records.dat
│   └── bills.dat
├── README.md             # This file
└── Makefile              # Build automation (optional)
```

---

## Compilation Instructions

### Windows (PowerShell/Command Prompt)
```powershell
cd C:\path\to\hospital-management-system
gcc *.c -o hospital_management_system.exe
```

### Linux/Mac (Terminal/Bash)
```bash
cd /path/to/hospital-management-system
gcc *.c -o hospital_management_system
chmod +x hospital_management_system
```

### Using Makefile (Optional)
```bash
make all          # Compile all files
make clean        # Remove object files and executable
make run          # Compile and run
```

---

## Execution Instructions

### Windows
```powershell
.\hospital_management_system.exe
```

### Linux/Mac
```bash
./hospital_management_system
```

---

## Features & Functionalities

### 1. **Authentication System**
- **Patient Login/Signup** - Username and password-based authentication
- **Doctor Login/Signup** - Separate credential management
- **Admin Login/Signup** - Administrative access with full system control
- Password validation: Minimum 6 characters with at least 1 digit and 1 letter
- Duplicate username prevention across all roles

### 2. **Patient Portal**
- View personal profile
- Schedule appointments with doctors
- Cancel or reschedule appointments
- View medical records and prescriptions
- View and pay bills with UPI payment option
- Search for doctors

### 3. **Doctor Portal**
- View assigned patients
- View scheduled appointments
- Add medical records with diagnosis and prescription
- View medical reports and history
- View patient bills and payment status

### 4. **Admin Portal**
- Full system management capabilities
- View all patients and doctors
- View all appointments with assignment options
- Search patients by name
- Generate and manage bills
- Access billing management system
- View analytics and reports

### 5. **Appointment Management**
- Schedule appointments with date/time validation
- Automatic conflict detection (prevent double-booking)
- Reschedule appointments with doctor availability checks
- Cancel appointments
- Assign doctors to appointments
- Sort appointments by date

### 6. **Patient & Doctor Management**
- Add new patients (admin/patient signup)
- Update patient information (age, phone, gender)
- Delete patient records (admin only)
- Add new doctors (admin/doctor signup)
- Update doctor information
- Delete doctor records (admin only)
- Sort by name and view profiles

### 7. **Medical Records & Billing**
- Create medical records with diagnosis and prescription
- View patient medical history
- Generate bills after appointments
- Track bill payment status (Paid/Unpaid)
- Mark bills as paid
- UPI payment instructions: `hospital.care@upi`
- Currency: Indian Rupees (₹)

### 8. **Search & Analytics**
- Search patients by name
- Search doctors by specialty
- View total patients and doctors count
- View total appointments
- Get doctor-specific appointment counts

---

## File Format

All data is stored in binary format (.dat files) in the `data/` directory:
- `patients.dat` - Patient records
- `doctors.dat` - Doctor records
- `admins.dat` - Admin credentials
- `appointments.dat` - Appointment records
- `medical_records.dat` - Medical records and prescriptions
- `bills.dat` - Billing information

---

## System Requirements

- **CPU**: Any modern processor
- **RAM**: Minimum 512 MB
- **Disk Space**: 10 MB (including executables and data files)
- **OS**: Windows (XP and above), Linux (any distro), macOS (10.5 and above)

---

## Input Validation

- **Age**: 1-120 years
- **Phone**: Minimum 7 digits
- **Password**: Minimum 6 characters (1 digit + 1 letter required)
- **Date/Time**: Must be in future and in format `YYYY-MM-DD HH:MM`
- **Username**: No duplicates across patients, doctors, or admins

---

## Default Test Data

The system creates empty data files on first run. You can:
1. Create test accounts through the signup system
2. Add patients and doctors through admin portal
3. Schedule appointments and generate bills
4. View analytics and reports

---

## Troubleshooting

### Issue: "gcc: command not found"
**Solution**: Install GCC compiler
- **Windows**: Install MinGW (http://www.mingw.org/)
- **Linux**: `sudo apt-get install build-essential` (Ubuntu/Debian)
- **Mac**: Install Xcode Command Line Tools: `xcode-select --install`

### Issue: ".exe file not found"
**Solution**: Ensure compilation completed successfully:
```powershell
gcc *.c -o hospital_management_system.exe 2>&1
```

### Issue: "data/ directory not found"
**Solution**: The system creates it automatically on first run. If needed, create manually:
```powershell
mkdir data
```

### Issue: "Diagnosis/Prescription input not working"
**Solution**: This has been fixed in the latest version. Recompile with `gcc *.c -o hospital_management_system.exe`

---

## Compilation & Execution Summary

**Step 1: Navigate to project directory**
```powershell
cd C:\Users\YourUsername\path\to\hospital-management-system
```

**Step 2: Compile all C files**
```powershell
gcc *.c -o hospital_management_system.exe
```

**Step 3: Run the executable**
```powershell
.\hospital_management_system.exe
```

**That's it!** The system will guide you through login/signup and role-based operations.

---

## Notes

- All data persists in binary format between sessions
- Each role (Patient, Doctor, Admin) has separate login credentials
- Passwords are stored in plain text (for educational purposes only)
- The system uses file-based storage; no database required
- Ensure write permissions in the project directory for data storage

---

## Contact & Support

For issues or questions, refer to the project report for implementation details and contact information of team members.