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
	(5, 'George', 'Head of Department', 5),
	(9, 'Sera', 'General Physician', 12);

-- Procedure Data
-- Code         Name                    Cost
-- 1            bypass surgery          450000
-- 2            RTPCR test              1400
-- 3            Appendectomy            30000
INSERT INTO
	Procedure (Code, Name, Cost)
VALUES
	(1, 'bypass surgery', 450000),
	(2, 'RTPCR test', 1400),
	(3, 'Appendectomy', 30000),
	(4, 'colonoscopy', 250000),
	(5, 'Immunotherapy', 300000);

-- Department
-- DepartmentID     Name                Head
-- 1                Cardiology          4
-- 2                Gastroenterology    5
-- 3                Oncology            3
INSERT INTO
	Department (DepartmentID, Name, Head)
VALUES
	(1, 'Cardiology', 4),
	(2, 'Gastroenterology', 5),
	(3, 'Oncology', 3);

-- Affiliated_with
-- Physician        Department      PrimaryAffiliation
-- 1                1               true
-- 2                2               true
-- 3                3               true
-- 4                1               true
-- 5                2               true
INSERT INTO
	Affiliated_with (Physician, Department, PrimaryAffiliation)
VALUES
	(1, 1, true),
	(2, 2, true),
	(3, 3, true),
	(4, 1, true),
	(5, 2, true),
	(9, 2, true);

-- Trained In
-- Physician        Treatment           CertificationDate       CertificationExpires
-- 1                1                   2020-01-03              2024-01-03
-- 2                4                   2020-11-13              2024-11-13
-- 3                5                   2020-10-07              2024-04-07
-- 4                1                   2021-04-20              2025-04-20
-- 5                3                   2021-01-17              2026-01-17
INSERT INTO
	Trained_In (
		Physician,
		Treatment,
		CertificationDate,
		CertificationExpires
	)
VALUES
	(1, 1, '2020-01-03', '2024-01-03'),
	(2, 4, '2020-11-13', '2024-11-13'),
	(3, 5, '2020-10-07', '2024-04-07'),
	(4, 1, '2021-04-20', '2025-04-20'),
	(5, 1, '2021-01-17', '2026-01-17');

-- Block
-- Floor        Code
-- 1            123
-- 1            124
-- 2            123
-- 69           420
-- 3            123
INSERT INTO
	Block (Floor, Code)
VALUES
	(1, 1),
	(1, 2),
	(2, 1),
	(3, 1);

-- Nurse
-- EmployeeID       Name            Position                Registered      SSN
-- 6                Jaques          Registered Nurse        true            6        
-- 7                Xavier          Nurse Practitioner      false           7
-- 8                Stella          ICU Registered Nurse    true            8
INSERT INTO
	Nurse (EmployeeID, Name, Position, Registered, SSN)
VALUES
	(6, 'Jaques', 'Registered Nurse', true, 6),
	(7, 'Xavier', 'Nurse Practitioner', false, 7),
	(8, 'Stella', 'ICU Registered Nurse', true, 8);

-- Room
-- Number           Type            	BlockFloor              BlockCode       Unavailable
-- 121              ICU             	1                       1               false
-- 122				Operation Theatre	1						2				false
-- 123				Multibed Ward		2						1				false
-- 124				Single Room			3						1				false
-- 125				Single Room			3						1				true
INSERT INTO
	Room (Number, Type, BlockFloor, BlockCode, Unavailable)
VALUES
	(121, 'ICU', 1, 1, false),
	(122, 'Operation Theatre', 1, 2, false),
	(123, 'Multibed Ward', 2, 1, false),
	(124, 'Single Room', 2, 1, false),
	(125, 'Single Room', 3, 1, true);

-- On_Call
-- Nurse	Floor	Code	Start		End
-- 6		2		1		2023-01-27	2023-01-31
-- 7		3		1		2023-01-30	2023-02-01
-- 8		2		1		2023-02-01	2023-02-03
-- 7		1		1		2023-02-05	2023-02-08
INSERT INTO
	On_Call (Nurse, BlockFloor, BlockCode, Start, "End")
VALUES
	(6, 2, 1, '2023-01-27', '2023-01-31'),
	(7, 3, 1, '2023-01-30', '2023-02-01'),
	(8, 2, 1, '2023-02-01', '2023-02-03'),
	(7, 1, 1, '2023-02-05', '2023-02-08');

-- Patient
-- SSN 		Name		Address									Phone		InsuranceID		PCP
-- 9		Kobe		'48 Finch Ave. Woodside, NY 11377'		'63045'		100				1	
-- 10		Nancy		'4 San Juan Ave. Grove City, OH 43123'	'45879'		101				2	
-- 11		Iris		'371 Locust Drive Glenview, IL 60025'	'87564'		102				3
INSERT INTO
	Patient (SSN, Name, Address, Phone, InsuranceID, PCP)
VALUES
	(
		9,
		'Kobe',
		'48 Finch Ave. Woodside, NY 11377',
		'63045',
		100,
		1
	),
	(
		10,
		'Nancy',
		'4 San Juan Ave. Grove City, OH 43123',
		'45879',
		101,
		2
	),
	(
		11,
		'Iris',
		'371 Locust Drive Glenview, IL 60025',
		'87564',
		102,
		3
	);

-- Medication
-- Code				Name			Brand				Description
-- 1				'remdesivir'	'Zydus Cadila'		'antiviral medicine that works against severe acute respiratory syndrome coronavirus 2 (SARS-CoV-2)'
-- 2				'prilosec'		'AstraZeneca'		'a prescription and over-the-counter medicine used to treat the symptoms of gastroesophageal reflux disease (GERD), gastric ulcers, and other conditions caused by excess stomach acid'
-- 3				'ibrutinib'		'Pharmacyclics LLC'	'a prescription medication used as an inhibitor of Bruton's tyrosine kinase (BTK) used to treat patients with mantle cell lymphoma (MCL)'
INSERT INTO
	Medication (Code, Name, Brand, Description)
VALUES
	(1, 'remdesivir', 'Zydus Cadila', 'antiviral medicine that works against severe acute respiratory syndrome coronavirus 2 (SARS-CoV-2)'),
	(2, 'prilosec', 'AstraZeneca', 'a prescription and over-the-counter medicine used to treat the symptoms of gastroesophageal reflux disease (GERD), gastric ulcers, and other conditions caused by excess stomach acid'),
	(3, 'ibrutinib', 'Pharmacyclics LLC', 'a prescription medication used as an inhibitor of Bruton tyrosine kinase (BTK) used to treat patients with mantle cell lymphoma (MCL)');

-- Appointment
-- AppointmentID		Patient		PrepNurse		Physician		Start 					"End" 					ExaminationRoom
-- 1					9			6				1				2023-01-29 16:30:00		2023-01-29 17:30:00		'Cardiovascular Exam Room'
-- 2					10			NULL			2				2023-01-31 19:00:00		2023-01-31 19:30:00		'X-Ray Examination Room'
-- 3					11			8				3				2023-02-06 09:00:00		2023-02-06 10:15:00		'Radiation Exam Room'
INSERT INTO
	Appointment (AppointmentID, Patient, PrepNurse, Physician, Start, "End", ExaminationRoom)
VALUES
	(1, 9, 6, 1, '2023-01-29 16:30:00', '2023-01-29 17:30:00', 'Cardiovascular Exam Room'),
	(2, 10, NULL, 2, '2023-01-31 19:00:00', '2023-01-31 19:30:00', 'X-Ray Examination Room'),
	(3, 11, 8, 3, '2023-02-06 09:00:00', '2023-02-06 10:15:00', 'Radiation Exam Room');

-- Prescribes
-- Physician	Patient		Medication		Date			Appointment		Dose
-- 9			9			1				'2023-02-10'	NULL			'100 mg'
-- 2			10			2				'2023-01-31'	2				'20 mg'
-- 3			11			3				'2023-02-06'	3				'420 mg'

INSERT INTO
	Prescribes (Physician, Patient, Medication, Date, Appointment, Dose)
VALUES
	(9, 9, 1, '2023-02-10', NULL, '100 mg'),
	(2, 10, 2, '2023-01-31', 2, '20 mg'),
	(3, 11, 3, '2023-02-06', 3, '420 mg');

-- Stay
-- StayID		Patient		Room		Start					"End"
-- 1			10			124			'2023-01-31'			'2023-02-04'
-- 2			11			121			'2023-02-06 14:30:00'	'2023-02-06 17:30:00'

/* Queries */
-- Q1. Names of all physicians who are trained in procedure name "bypass surgery"
SELECT
	Ph.Name
FROM
	Physician as Ph
	JOIN Trained_In as Tr ON Tr.Physician = Ph.EmployeeID
	JOIN Procedure as Pr ON Pr.code = Tr.Treatment
WHERE
	Pr.Name = 'bypass surgery';

-- Q2
SELECT
	Ph.name
FROM
	Physician as Ph
	JOIN Affiliated_with as Aw ON (Aw.Physician, Aw.Department) = (
		ph.EmployeeID,
		(
			SELECT
				DepartmentID
			FROM
				Department
			WHERE
				Name = 'Cardiology'
		)
	)
	JOIN Trained_In as Tr ON Tr.Physician = Ph.EmployeeID
	JOIN Procedure as Pr ON Pr.code = Tr.Treatment
WHERE
	Pr.name = 'bypass surgery';

-- Q3
SELECT
	Name
FROM
	Nurse as N
	JOIN Room as R ON R.Number = 123
	JOIN On_Call as OC ON (OC.BlockFloor, OC.BlockCode) = (R.BlockFloor, R.BlockCode)
WHERE
	OC.Nurse = N.EmployeeID;

-- Q4
SELECT
	PT.Name, PT.Address
FROM
	Patient as PT
	JOIN Medication as MC ON MC.Name = 'remdesivir'
	JOIN prescribes as PC ON PC.medication = MC.code
WHERE
	PT.SSN = PC.Patient;

-- Q5
SELECT
	PT.Name, PT.InsuranceID
FROM
	Patient as PT
	JOIN 