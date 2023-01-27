-- Physician
CREATE TABLE Physician (
    EmployeeID INTEGER PRIMARY KEY,
    Name TEXT NOT NULLABLE,
    Position TEXT NOT NULLABLE
);

-- Procedure
CREATE TABLE Procedure (
    Code INTEGER PRIMARY KEY,
    Name TEXT NOT NULLABLE,
    Cost INTEGER
);

-- Medication
CREATE TABLE Medication (
    Code INTEGER PRIMARY KEY,
    Name TEXT NOT NULLABLE,
    Brand TEXT NOT NULLABLE,
    Description TEXT NOT NULLABLE
);

-- Block
CREATE TABLE Block (
    Floor INTEGER NOT NULL,
    Code INTEGER NOT NULL,
    PRIMARY KEY (Floor, Code)
);

-- Nurse
CREATE TABLE Nurse (
    EmployeeID INTEGER PRIMARY KEY,
    Name TEXT NOT NULLABLE,
    Position TEXT NOT NULLABLE,
    Registered BOOLEAN NOT NULL,
    SSN INTEGER
);

-- Trained_In
CREATE TABLE Trained_In (
    Physician INTEGER,
    Treatment INTEGER,
    CertificationDate TIMESTAMP NOT NULL,
    CertificationExpires TIMESTAMP NOT NULL,
    PRIMARY KEY (Physician, Treatment), 
    FOREIGN KEY (Physician) REFERENCES Physician(EmployeeID),
    FOREIGN KEY (Treatment) REFERENCES Procedure(Code)
);

-- Department
CREATE TABLE Department (
    Head INTEGER NOT NULL,
    DepartmentID INTEGER PRIMARY KEY,
    Name text,
    FOREIGN KEY (Head) REFERENCES Physician(EmployeeID)
);

-- Affiliated_with
CREATE TABLE Affiliated_with (
    Physician INTEGER,
    Department INTEGER,
    PrimaryAffiliation BOOLEAN NOT NULL,
    FOREIGN KEY (Physician) REFERENCES Physician(EmployeeID),
    FOREIGN KEY (Department) REFERENCES Department(DepartmentID)
);

-- Patient
CREATE TABLE Patient (
    SSN INTEGER PRIMARY KEY,
    Name text,
    Address text,
    Phone text,
    InsuranceID INTEGER,
    PCP INTEGER NOT NULL,
    FOREIGN KEY (PCP) REFERENCES Physician(EmployeeID)
);

-- Room
CREATE TABLE Room (
    Number INTEGER PRIMARY KEY,
    Type text,
    BlockFloor INTEGER,
    BlockCode INTEGER,
    Unavailable BOOLEAN NOT NULL,
    FOREIGN KEY (BlockFloor, BlockCode) REFERENCES Block(Floor, Code)
);

-- On_Call, here "End" is used called End is a keyword in postgresql
CREATE TABLE On_Call (
    Nurse INTEGER,
    BlockFloor INTEGER,
    BlockCode INTEGER,
    Start TIMESTAMP NOT NULL,
    "End" TIMESTAMP NOT NULL,
    FOREIGN KEY (Nurse)  REFERENCES Nurse(EmployeeID),
    FOREIGN KEY (BlockFloor, BlockCode) REFERENCES Block(Floor, Code),
    PRIMARY KEY (Nurse, BlockFloor, BlockCode, Start, "End") 
);

-- Stay
CREATE TABLE Stay (
    StayID INTEGER PRIMARY KEY,
    Patient INTEGER,
    Room INTEGER,
    Start TIMESTAMP NOT NULL,
    "End" TIMESTAMP NOT NULL,
    FOREIGN KEY (Patient) REFERENCES Room(Number),
    FOREIGN KEY (Room) REFERENCES Patient(SSN)
);

-- Undergoes
CREATE TABLE Undergoes (
    Patient INTEGER NOT NULL,
    Procedure INTEGER NOT NULL,
    Stay INTEGER NOT NULL,
    Date TIMESTAMP NOT NULL,
    Physician INTEGER NOT NULL,
    AssistingNurse INTEGER,
    PRIMARY KEY (Patient, Procedure, Stay, Date),
    FOREIGN KEY (Patient) REFERENCES Patient(SSN),
    FOREIGN KEY (Procedure) REFERENCES Procedure(Code),
    FOREIGN KEY (Stay) REFERENCES Stay(StayID),
    FOREIGN KEY (Physician) REFERENCES Physician(EmployeeID),
    FOREIGN KEY (AssistingNurse) REFERENCES Nurse(EmployeeID)
);

-- Appointment
CREATE TABLE Appointment (
    AppointmentID INTEGER NOT NULL PRIMARY KEY,
    Patient INTEGER NOT NULL,
    prepNurse INTEGER,
    Physician INTEGER,
    Start TIMESTAMP,
    "End" TIMESTAMP,
    ExaminationRoom text,
    FOREIGN KEY (Patient) REFERENCES Patient(SSN),
    FOREIGN KEY (prepNurse) REFERENCES Nurse(EmployeeID),
    FOREIGN KEY (Physician) REFERENCES Physician(EmployeeID)
);

-- Prescribes
CREATE TABLE Prescribes (
    Physician INTEGER NOT NULL,
    Patient INTEGER NOT NULL,
    Medication INTEGER NOT NULL,
    Date TIMESTAMP NOT NULL,
    Appointment INTEGER,
    Dose text NOT NULL,
    PRIMARY KEY (Physician, Patient, Medication, Date),
    FOREIGN KEY (Physician) REFERENCES Physician(EmployeeID),
    FOREIGN KEY (Patient) REFERENCES Patient(SSN),
    FOREIGN KEY (Medication) REFERENCES Medication(Code),
    FOREIGN KEY (Appointment) REFERENCES Appointment(AppointmentID)
);