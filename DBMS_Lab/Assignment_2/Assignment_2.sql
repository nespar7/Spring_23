/* Creating tables */
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

/* Populating data */
INSERT INTO Physician VALUES(1,'Alan Donald','Intern',111111111);
INSERT INTO Physician VALUES(2,'Bruce Reid','Attending Physician',222222222);
INSERT INTO Physician VALUES(3,'Courtney Walsh','Surgeon Physician',333333333);
INSERT INTO Physician VALUES(4,'Malcom Marshall','Senior Physician',444444444);
INSERT INTO Physician VALUES(5,'Dennis Lillee','Head Chief of Medicine',555555555);
INSERT INTO Physician VALUES(6,'Jeff Thomson','Surgeon Physician',666666666);
INSERT INTO Physician VALUES(7,'Richard Hadlee','Surgeon Physician',777777777);
INSERT INTO Physician VALUES(8,'Kapil  Dev','Resident',888888888);
INSERT INTO Physician VALUES(9,'Ishant Sharma','Psychiatrist',999999999);

INSERT INTO Department VALUES(1,'medicine',4);
INSERT INTO Department VALUES(2,'surgery',7);
INSERT INTO Department VALUES(3,'psychiatry',9);
INSERT INTO Department VALUES(4,'cardiology',8);

INSERT INTO Affiliated_With VALUES(1,1,true);
INSERT INTO Affiliated_With VALUES(2,1,true);
INSERT INTO Affiliated_With VALUES(3,1,false);
INSERT INTO Affiliated_With VALUES(3,2,true);
INSERT INTO Affiliated_With VALUES(4,1,true);
INSERT INTO Affiliated_With VALUES(5,1,true);
INSERT INTO Affiliated_With VALUES(6,2,true);
INSERT INTO Affiliated_With VALUES(7,1,false);
INSERT INTO Affiliated_With VALUES(7,2,true);
INSERT INTO Affiliated_With VALUES(8,1,true);
INSERT INTO Affiliated_With VALUES(9,3,true);

INSERT INTO Procedure VALUES(1,'bypass surgery',1500.0);
INSERT INTO Procedure VALUES(2,'angioplasty',3750.0);
INSERT INTO Procedure VALUES(3,'arthoscopy',4500.0);
INSERT INTO Procedure VALUES(4,'carotid endarterectomy',10000.0);
INSERT INTO Procedure VALUES(5,'cholecystectomy',4899.0);
INSERT INTO Procedure VALUES(6,'tonsillectomy',5600.0);
INSERT INTO Procedure VALUES(7,'cataract surgery',25.0);

INSERT INTO Patient VALUES(100000001,'Dilip Vengsarkar','42 Foobar Lane','555-0256',68476213,1);
INSERT INTO Patient VALUES(100000002,'Richie Richardson','37 Infinite Loop','555-0512',36546321,2);
INSERT INTO Patient VALUES(100000003,'Mark Waugh','101 Parkway Street','555-1204',65465421,2);
INSERT INTO Patient VALUES(100000004,'Ramiz Raza','1100 Sparks Avenue','555-2048',68421879,3);

INSERT INTO Nurse VALUES(101,'Eknath Solkar','Head Nurse',true,111111110);
INSERT INTO Nurse VALUES(102,'David Boon','Nurse',true,222222220);
INSERT INTO Nurse VALUES(103,'Andy Flowers','Nurse',false,333333330);

INSERT INTO Appointment VALUES(13216584,100000001,101,1,'2018-04-24 10:00','2018-04-24 11:00','A');
INSERT INTO Appointment VALUES(26548913,100000002,101,2,'2018-04-24 10:00','2018-04-24 11:00','B');
INSERT INTO Appointment VALUES(36549879,100000001,102,1,'2018-04-25 10:00','2018-04-25 11:00','A');
INSERT INTO Appointment VALUES(46846589,100000004,103,4,'2018-04-25 10:00','2018-04-25 11:00','B');
INSERT INTO Appointment VALUES(59871321,100000004,NULL,4,'2018-04-26 10:00','2018-04-26 11:00','C');
INSERT INTO Appointment VALUES(69879231,100000003,103,2,'2018-04-26 11:00','2018-04-26 12:00','C');
INSERT INTO Appointment VALUES(76983231,100000001,NULL,3,'2018-04-26 12:00','2018-04-26 13:00','C');
INSERT INTO Appointment VALUES(86213939,100000004,102,9,'2018-04-27 10:00','2018-04-21 11:00','A');
INSERT INTO Appointment VALUES(93216548,100000002,101,2,'2018-04-27 10:00','2018-04-27 11:00','B');

INSERT INTO Medication VALUES(1,'Paracetamol','Z','N/A');
INSERT INTO Medication VALUES(2,'Actemra','Foolki Labs','N/A');
INSERT INTO Medication VALUES(3,'Molnupiravir','Bale Laboratories','N/A');
INSERT INTO Medication VALUES(4,'Paxlovid','Bar Industries','N/A');
INSERT INTO Medication VALUES(5,'Remdesivir','Donald Pharmaceuticals','N/A');

INSERT INTO Prescribes VALUES(1,100000001,1,'2018-04-24 10:47',13216584,'5');
INSERT INTO Prescribes VALUES(9,100000004,2,'2018-04-27 10:53',86213939,'10');
INSERT INTO Prescribes VALUES(9,100000004,2,'2018-04-30 16:53',NULL,'5');

INSERT INTO Block VALUES(1,1);
INSERT INTO Block VALUES(1,2);
INSERT INTO Block VALUES(1,3);
INSERT INTO Block VALUES(2,1);
INSERT INTO Block VALUES(2,2);
INSERT INTO Block VALUES(2,3);
INSERT INTO Block VALUES(3,1);
INSERT INTO Block VALUES(3,2);
INSERT INTO Block VALUES(3,3);
INSERT INTO Block VALUES(4,1);
INSERT INTO Block VALUES(4,2);
INSERT INTO Block VALUES(4,3);

INSERT INTO Room VALUES(101,'Single',1,1,false);
INSERT INTO Room VALUES(102,'Single',1,1,false);
INSERT INTO Room VALUES(103,'Single',1,1,false);
INSERT INTO Room VALUES(111,'Single',1,2,false);
INSERT INTO Room VALUES(112,'Single',1,2,true);
INSERT INTO Room VALUES(113,'Single',1,2,false);
INSERT INTO Room VALUES(121,'Single',1,3,false);
INSERT INTO Room VALUES(122,'Single',1,3,false);
INSERT INTO Room VALUES(123,'Single',1,3,false);
INSERT INTO Room VALUES(201,'Single',2,1,true);
INSERT INTO Room VALUES(202,'Single',2,1,false);
INSERT INTO Room VALUES(203,'Single',2,1,false);
INSERT INTO Room VALUES(211,'Single',2,2,false);
INSERT INTO Room VALUES(212,'Single',2,2,false);
INSERT INTO Room VALUES(213,'Single',2,2,true);
INSERT INTO Room VALUES(221,'Single',2,3,false);
INSERT INTO Room VALUES(222,'Single',2,3,false);
INSERT INTO Room VALUES(223,'Single',2,3,false);
INSERT INTO Room VALUES(301,'Single',3,1,false);
INSERT INTO Room VALUES(302,'Single',3,1,true);
INSERT INTO Room VALUES(303,'Single',3,1,false);
INSERT INTO Room VALUES(311,'Single',3,2,false);
INSERT INTO Room VALUES(312,'Single',3,2,false);
INSERT INTO Room VALUES(313,'Single',3,2,false);
INSERT INTO Room VALUES(321,'Single',3,3,true);
INSERT INTO Room VALUES(322,'Single',3,3,false);
INSERT INTO Room VALUES(323,'Single',3,3,false);
INSERT INTO Room VALUES(401,'Single',4,1,false);
INSERT INTO Room VALUES(402,'Single',4,1,true);
INSERT INTO Room VALUES(403,'Single',4,1,false);
INSERT INTO Room VALUES(411,'Single',4,2,false);
INSERT INTO Room VALUES(412,'Single',4,2,false);
INSERT INTO Room VALUES(413,'Single',4,2,false);
INSERT INTO Room VALUES(421,'Single',4,3,true);
INSERT INTO Room VALUES(422,'Single',4,3,false);
INSERT INTO Room VALUES(423,'Single',4,3,false);

INSERT INTO On_Call VALUES(101,1,1,'2018-11-04 11:00','2018-11-04 19:00');
INSERT INTO On_Call VALUES(101,1,2,'2018-11-04 11:00','2018-11-04 19:00');
INSERT INTO On_Call VALUES(102,1,3,'2018-11-04 11:00','2018-11-04 19:00');
INSERT INTO On_Call VALUES(103,1,1,'2018-11-04 19:00','2018-11-05 03:00');
INSERT INTO On_Call VALUES(103,1,2,'2018-11-04 19:00','2018-11-05 03:00');
INSERT INTO On_Call VALUES(103,1,3,'2018-11-04 19:00','2018-11-05 03:00');

INSERT INTO Stay VALUES(3215,100000001,111,'2018-05-01','2018-05-04');
INSERT INTO Stay VALUES(3216,100000003,123,'2018-05-03','2018-05-14');
INSERT INTO Stay VALUES(3217,100000004,112,'2018-05-02','2018-05-03');

INSERT INTO Undergoes VALUES(100000001,6,3215,'2018-05-02',3,101);
INSERT INTO Undergoes VALUES(100000001,2,3215,'2018-05-03',7,101);
INSERT INTO Undergoes VALUES(100000004,1,3217,'2018-05-07',3,102);
INSERT INTO Undergoes VALUES(100000004,5,3217,'2018-05-09',6,NULL);
INSERT INTO Undergoes VALUES(100000001,7,3217,'2018-05-10',7,101);
INSERT INTO Undergoes VALUES(100000004,4,3217,'2018-05-13',3,103);

INSERT INTO Trained_In VALUES(3,1,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(3,2,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(3,5,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(3,6,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(3,7,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(6,2,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(6,5,'2017-01-01','2017-12-31');
INSERT INTO Trained_In VALUES(6,6,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,1,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,2,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,3,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,4,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,5,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,6,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,7,'2018-01-01','2018-12-31');

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
	PT.Name,
	PT.Address
FROM
	Patient as PT
	JOIN Medication as MC ON MC.Name = 'remdesivir'
	JOIN prescribes as PC ON PC.medication = MC.code
WHERE
	PT.SSN = PC.Patient;

-- Q5
SELECT
	PT.Name,
	PT.InsuranceID
FROM
	Patient as PT
	JOIN Room as R ON R.Type = 'ICU'
	JOIN Stay as S ON S.Patient = PT.SSN
WHERE
	S."End" - S.Start >= interval '15 days';

-- Q6
SELECT
	N.Name as Nurse_name
FROM
	Nurse as N
	JOIN Procedure as Pr ON Pr.Name = 'bypass surgery'
	JOIN Undergoes as U ON U.Procedure = Pr.Code
WHERE
	U.AssistingNurse = N.EmployeeID;

-- Q7
SELECT
	N.Name as Assisting_Nurse,
	N.Position as Nurse_position,
	(
		SELECT
			Name
		FROM
			Patient as PT
		WHERE
			U.Patient = PT.SSN
	) as Patient_name,
	(
		SELECT
			Name
		FROM
			Physician as Ph
		WHERE
			U.Physician = Ph.EmployeeID
	) as Physician_name
FROM
	Nurse as N
	JOIN Procedure as Pr ON Pr.Name = 'bypass surgery'
	JOIN Undergoes as U ON U.Procedure = Pr.Code
WHERE
	N.EmployeeID = U.AssistingNurse;

-- Q8
SELECT
	Ph.Name
FROM
	Physician as Ph
	JOIN Trained_In as TI ON TI.Physician = Ph.EmployeeID
	JOIN Undergoes as U ON U.Physician = Ph.EmployeeID
WHERE
	TI.Treatment != U.Procedure;

-- Q9
SELECT
	Ph.Name
FROM
	Physician as Ph
	JOIN Trained_In as Tr ON Tr.Physician = Ph.EmployeeID
	JOIN Undergoes as U on U.Physician = Ph.EmployeeID
WHERE
	Tr.Treatment = U.Procedure
	and U.Date > Tr.CertificationExpires;

-- Q10
SELECT
	Ph.Name as Physician,
	(
		SELECT
			Name
		FROM
			Procedure as Pr
		WHERE
			Pr.Code = U.Procedure
	) as Procedure,
	U.Date,
	(
		SELECT
			Name
		FROM
			Patient as Pt
		WHERE
			Pt.SSN = U.Patient
	) as Patient
FROM
	Physician as Ph
	JOIN Trained_In as Tr ON Tr.Physician = Ph.EmployeeID
	JOIN Undergoes as U on U.Physician = Ph.EmployeeID
WHERE
	Tr.Treatment = U.Procedure
	and U.Date > Tr.CertificationExpires;

-- Q11
WITH
	temp as (
		SELECT
			Pt.Name as Patient,
			Pt.PCP as Physician,
			(
				SELECT
					count(*)
				FROM
					Appointment as Ap
				WHERE
					Ap.Patient = Pt.SSN
					and Ap.Physician IN (
						SELECT
							Ph.EmployeeID
						FROM
							Physician as Ph
							JOIN Affiliated_with as Aw ON Aw.Physician = Ph.EmployeeID
							JOIN Department as Dp ON Dp.DepartmentID = Aw.Department
						WHERE
							Dp.Name = 'Cardiology'
					)
			) AS AppointmentsWithCardiologist,
			(
				SELECT
					count(*)
				FROM
					Prescribes as Pr
				WHERE
					Pr.Patient = Pt.SSN
					and Pr.Physician = Pt.PCP
			) AS PhysicianPrescribed,
			(
				SELECT
					count(*)
				FROM
					Undergoes as U
					JOIN Procedure as Pr ON Pr.Code = U.Procedure
				WHERE
					U.Patient = Pt.SSN
					and Pr.Cost > 5000
			) AS CostAtleastFiveK
		FROM
			Patient as Pt
	)
SELECT
	Patient,
	(
		SELECT
			Physician.Name
		FROM
			Physician
		WHERE
			Physician.EmployeeID = temp.Physician
	)
FROM
	temp
WHERE
	AppointmentsWithCardiologist >= 2
	and CostAtleastFiveK >= 1
	and PhysicianPrescribed >= 1
	and Physician NOT IN (
		SELECT
			EmployeeID
		FROM
			Physician
		WHERE
			Physician.Position = 'Head of Department'
	);

-- Q12
SELECT
	Medicine,
	Brand
FROM
	(
		SELECT
			MC.Name as Medicine,
			MC.Brand,
			COUNT(*) as number_of_times
		FROM
			Medication as MC
			JOIN Prescribes as Pc ON Pc.medication = MC.code
		GROUP BY
			MC.Name,
			MC.Brand
		ORDER BY
			number_of_times DESC
		LIMIT
			1
	) as temp;