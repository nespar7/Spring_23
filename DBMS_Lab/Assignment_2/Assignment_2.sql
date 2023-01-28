-- Physician
CREATE TABLE
    Physician (
        EmployeeID INTEGER PRIMARY KEY,
        Name TEXT NOT NULL,
        Position TEXT NOT NULL,
        SSN INTEGER NOT NULL
    );

-- Procedure
CREATE TABLE
    Procedure (
        Code INTEGER PRIMARY KEY,
        Name TEXT NOT NULL,
        Cost INTEGER NOT NULL
    );

-- Medication
CREATE TABLE
    Medication (
        Code INTEGER PRIMARY KEY,
        Name TEXT NOT NULL,
        Brand TEXT NOT NULL,
        Description TEXT NOT NULL
    );

-- Block
CREATE TABLE
    Block (
        Floor INTEGER NOT NULL,
        Code INTEGER NOT NULL,
        PRIMARY KEY (Floor, Code)
    );

-- Nurse
CREATE TABLE
    Nurse (
        EmployeeID INTEGER PRIMARY KEY,
        Name TEXT NOT NULL,
        Position TEXT NOT NULL,
        Registered BOOLEAN NOT NULL,
        SSN INTEGER NOT NULL
    );

-- Trained_In
CREATE TABLE
    Trained_In (
        Physician INTEGER NOT NULL,
        Treatment INTEGER NOT NULL,
        CertificationDate TIMESTAMP NOT NULL,
        CertificationExpires TIMESTAMP NOT NULL,
        PRIMARY KEY (Physician, Treatment),
        FOREIGN KEY (Physician) REFERENCES Physician (EmployeeID),
        FOREIGN KEY (Treatment) REFERENCES Procedure (Code)
    );

-- Department
CREATE TABLE
    Department (
        DepartmentID INTEGER PRIMARY KEY,
        Name TEXT NOT NULL,
        Head INTEGER NOT NULL,
        FOREIGN KEY (Head) REFERENCES Physician (EmployeeID)
    );

-- Affiliated_with
CREATE TABLE
    Affiliated_with (
        Physician INTEGER NOT NULL,
        Department INTEGER NOT NULL,
        PrimaryAffiliation BOOLEAN NOT NULL,
        FOREIGN KEY (Physician) REFERENCES Physician (EmployeeID),
        FOREIGN KEY (Department) REFERENCES Department (DepartmentID),
        PRIMARY KEY (Physician, Department)
    );

-- Patient
CREATE TABLE
    Patient (
        SSN INTEGER PRIMARY KEY,
        Name TEXT NOT NULL,
        Address TEXT NOT NULL,
        Phone TEXT NOT NULL,
        InsuranceID INTEGER NOT NULL,
        PCP INTEGER NOT NULL,
        FOREIGN KEY (PCP) REFERENCES Physician (EmployeeID)
    );

-- Room
CREATE TABLE
    Room (
        Number INTEGER PRIMARY KEY,
        Type TEXT NOT NULL,
        BlockFloor INTEGER NOT NULL,
        BlockCode INTEGER NOT NULL,
        Unavailable BOOLEAN NOT NULL,
        FOREIGN KEY (BlockFloor, BlockCode) REFERENCES Block (Floor, Code)
    );

-- On_Call, here "End" is used called End is a keyword in postgresql
CREATE TABLE
    On_Call (
        Nurse INTEGER NOT NULL,
        BlockFloor INTEGER NOT NULL,
        BlockCode INTEGER NOT NULL,
        Start TIMESTAMP NOT NULL,
        "End" TIMESTAMP NOT NULL,
        FOREIGN KEY (Nurse) REFERENCES Nurse (EmployeeID),
        FOREIGN KEY (BlockFloor, BlockCode) REFERENCES Block (Floor, Code),
        PRIMARY KEY (Nurse, BlockFloor, BlockCode, Start, "End")
    );

-- Stay
CREATE TABLE
    Stay (
        StayID INTEGER PRIMARY KEY,
        Patient INTEGER NOT NULL,
        Room INTEGER NOT NULL,
        Start TIMESTAMP NOT NULL,
        "End" TIMESTAMP NOT NULL,
        FOREIGN KEY (Room) REFERENCES Room (Number),
        FOREIGN KEY (Patient) REFERENCES Patient (SSN)
    );

-- Undergoes
CREATE TABLE
    Undergoes (
        Patient INTEGER NOT NULL,
        Procedure INTEGER NOT NULL,
        Stay INTEGER NOT NULL,
        Date TIMESTAMP NOT NULL,
        Physician INTEGER NOT NULL,
        AssistingNurse INTEGER,
        PRIMARY KEY (Patient, Procedure, Stay, Date),
        FOREIGN KEY (Patient) REFERENCES Patient (SSN),
        FOREIGN KEY (Procedure) REFERENCES Procedure (Code),
        FOREIGN KEY (Stay) REFERENCES Stay (StayID),
        FOREIGN KEY (Physician) REFERENCES Physician (EmployeeID),
        FOREIGN KEY (AssistingNurse) REFERENCES Nurse (EmployeeID)
    );

-- Appointment
CREATE TABLE
    Appointment (
        AppointmentID INTEGER PRIMARY KEY,
        Patient INTEGER NOT NULL,
        prepNurse INTEGER,
        Physician INTEGER NOT NULL,
        Start TIMESTAMP NOT NULL,
        "End" TIMESTAMP NOT NULL,
        ExaminationRoom TEXT NOT NULL,
        FOREIGN KEY (Patient) REFERENCES Patient (SSN),
        FOREIGN KEY (prepNurse) REFERENCES Nurse (EmployeeID),
        FOREIGN KEY (Physician) REFERENCES Physician (EmployeeID)
    );

-- Prescribes
CREATE TABLE
    Prescribes (
        Physician INTEGER NOT NULL,
        Patient INTEGER NOT NULL,
        Medication INTEGER NOT NULL,
        Date TIMESTAMP NOT NULL,
        Appointment INTEGER,
        Dose TEXT NOT NULL,
        PRIMARY KEY (Physician, Patient, Medication, Date),
        FOREIGN KEY (Physician) REFERENCES Physician (EmployeeID),
        FOREIGN KEY (Patient) REFERENCES Patient (SSN),
        FOREIGN KEY (Medication) REFERENCES Medication (Code),
        FOREIGN KEY (Appointment) REFERENCES Appointment (AppointmentID)
    );

/* Data population */
-- Physician Data
-- ID       Name        Position                SSN
-- 1        Valerie     Medical Director        1
-- 2        Janet       Senior Resident         2
-- 3        Thomas      Head of Department      3
-- 4        Joseph      Head of Department      4
-- 5        George      Head of Department      5 
INSERT INTO
    Physician (EmployeeID, Name, Position, SSN)
VALUES
    (1, 'Valerie', 'Medical Director', 1),
    (2, 'Janet', 'Senior Resident', 2),
    (3, 'Thomas', 'Head of Department', 3),
    (4, 'Joseph', 'Head of Department', 4),
    (5, 'George', 'Head of Department', 5);

-- Procedure Data
-- Code         Name                    Cost
-- 1            bypass surgery          450000
-- 2            RTPCR test              1400
-- 3            Appendectomy            30000
INSERT INTO
    Procedure (Code, Name, Cost)
VALUES
    (1, 'coronary artery bypass surgery', 450000),
    (2, 'RTPCR test', 1400),
    (3, 'Appendectomy', 30000),
    (4, 'gastric bypass surgery', 250000);

-- Department
-- DepartmentID     Name            Head

-- Affiliated_with
-- Physician        Department      PrimaryAffiliation

-- Trained In
-- Physician        Treatment           CertificationDate       CertificationExpiry


/* Queries */
-- Q1. Names of all physicians who are trained in procedure name "bypass surgery"
SELECT
    Name
FROM
    Physician
WHERE
    EmployeeID = (
        SELECT
            Physician
        FROM
            Trained_In
        WHERE
            Treatment = (
                SELECT
                    Code
                FROM
                    Procedure
                WHERE
                    Name = 'bypass surgery'
            )
    )

-- Q2