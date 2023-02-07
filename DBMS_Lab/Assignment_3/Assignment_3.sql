/* creating tables */
-- physician
create table
    physician (
        employeeid integer primary key,
        name text not null,
        position text not null,
        ssn integer not null
    );

-- procedure
create table
    `procedure` (
        code integer primary key,
        name text not null,
        cost integer not null
    );

-- medication
create table
    medication (
        code integer primary key,
        name text not null,
        brand text not null,
        description text not null
    );

-- block
create table
    block (
        floor integer not null,
        code integer not null,
        primary key (floor, code)
    );

-- nurse
create table
    nurse (
        employeeid integer primary key,
        name text not null,
        position text not null,
        registered boolean not null,
        ssn integer not null
    );

-- trained_in
create table
    trained_in (
        physician integer not null,
        treatment integer not null,
        certificationdate timestamp not null,
        certificationexpires timestamp not null,
        primary key (physician, treatment),
        foreign key (physician) references physician (employeeid),
        foreign key (treatment) references `procedure` (code)
    );

-- department
create table
    department (
        departmentid integer primary key,
        name text not null,
        head integer not null,
        foreign key (head) references physician (employeeid)
    );

-- affiliated_with
create table
    affiliated_with (
        physician integer not null,
        department integer not null,
        primaryaffiliation boolean not null,
        foreign key (physician) references physician (employeeid),
        foreign key (department) references department (departmentid),
        primary key (physician, department)
    );

-- patient
create table
    patient (
        ssn integer primary key,
        name text not null,
        address text not null,
        phone text not null,
        insuranceid integer not null,
        pcp integer not null,
        foreign key (pcp) references physician (employeeid)
    );

-- room
create table
    room (
        number integer primary key,
        type text not null,
        blockfloor integer not null,
        blockcode integer not null,
        unavailable boolean not null,
        foreign key (blockfloor, blockcode) references block (floor, code)
    );

-- on_call, here "end" is used called end is a keyword in postgresql
create table
    on_call (
        nurse integer not null,
        blockfloor integer not null,
        blockcode integer not null,
        start timestamp not null,
        end timestamp not null,
        foreign key (nurse) references nurse (employeeid),
        foreign key (blockfloor, blockcode) references block (floor, code),
        primary key (nurse, blockfloor, blockcode, start, end)
    );

-- stay
create table
    stay (
        stayid integer primary key,
        patient integer not null,
        room integer not null,
        start timestamp not null,
        end timestamp not null,
        foreign key (room) references room (number),
        foreign key (patient) references patient (ssn)
    );

-- undergoes
create table
    undergoes (
        patient integer not null,
        `procedure` integer not null,
        stay integer not null,
        date timestamp not null,
        physician integer not null,
        assistingnurse integer,
        primary key (patient, `procedure`, stay, date),
        foreign key (patient) references patient (ssn),
        foreign key (`procedure`) references `procedure` (code),
        foreign key (stay) references stay (stayid),
        foreign key (physician) references physician (employeeid),
        foreign key (assistingnurse) references nurse (employeeid)
    );

-- appointment
create table
    appointment (
        appointmentid integer primary key,
        patient integer not null,
        prepnurse integer,
        physician integer not null,
        start timestamp not null,
        end timestamp not null,
        examinationroom text not null,
        foreign key (patient) references patient (ssn),
        foreign key (prepnurse) references nurse (employeeid),
        foreign key (physician) references physician (employeeid)
    );

-- prescribes
create table
    prescribes (
        physician integer not null,
        patient integer not null,
        medication integer not null,
        date timestamp not null,
        appointment integer,
        dose text not null,
        primary key (physician, patient, medication, date),
        foreign key (physician) references physician (employeeid),
        foreign key (patient) references patient (ssn),
        foreign key (medication) references medication (code),
        foreign key (appointment) references appointment (appointmentid)
    );

/* populating data */
insert into physician values(1,'alan donald','intern',111111111);
insert into physician values(2,'bruce reid','attending physician',222222222);
insert into physician values(3,'courtney walsh','surgeon physician',333333333);
insert into physician values(4,'malcom marshall','senior physician',444444444);
insert into physician values(5,'dennis lillee','head chief of medicine',555555555);
insert into physician values(6,'jeff thomson','surgeon physician',666666666);
insert into physician values(7,'richard hadlee','surgeon physician',777777777);
insert into physician values(8,'kapil  dev','resident',888888888);
insert into physician values(9,'ishant sharma','psychiatrist',999999999);

insert into department values(1,'medicine',4);
insert into department values(2,'surgery',7);
insert into department values(3,'psychiatry',9);
insert into department values(4,'cardiology',8);

insert into affiliated_with values(1,1,1);
insert into affiliated_with values(2,1,1);
insert into affiliated_with values(3,1,0);
insert into affiliated_with values(3,2,1);
insert into affiliated_with values(4,1,1);
insert into affiliated_with values(5,1,1);
insert into affiliated_with values(6,2,1);
insert into affiliated_with values(7,1,0);
insert into affiliated_with values(7,2,1);
insert into affiliated_with values(8,1,1);
insert into affiliated_with values(9,3,1);

insert into `procedure` values(1,'bypass surgery',1500.0);
insert into `procedure` values(2,'angioplasty',3750.0);
insert into `procedure` values(3,'arthoscopy',4500.0);
insert into `procedure` values(4,'carotid endarterectomy',10000.0);
insert into `procedure` values(5,'cholecystectomy',4899.0);
insert into `procedure` values(6,'tonsillectomy',5600.0);
insert into `procedure` values(7,'cataract surgery',25.0);

insert into patient values(100000001,'dilip vengsarkar','42 foobar lane','555-0256',68476213,1);
insert into patient values(100000002,'richie richardson','37 infinite loop','555-0512',36546321,2);
insert into patient values(100000003,'mark waugh','101 parkway street','555-1204',65465421,2);
insert into patient values(100000004,'ramiz raza','1100 sparks avenue','555-2048',68421879,3);

insert into nurse values(101,'eknath solkar','head nurse',1,111111110);
insert into nurse values(102,'david boon','nurse',1,222222220);
insert into nurse values(103,'andy flowers','nurse',0,333333330);

insert into appointment values(13216584,100000001,101,1,'2018-04-24 10:00','2018-04-24 11:00','a');
insert into appointment values(26548913,100000002,101,2,'2018-04-24 10:00','2018-04-24 11:00','b');
insert into appointment values(36549879,100000001,102,1,'2018-04-25 10:00','2018-04-25 11:00','a');
insert into appointment values(46846589,100000004,103,4,'2018-04-25 10:00','2018-04-25 11:00','b');
insert into appointment values(59871321,100000004,null,4,'2018-04-26 10:00','2018-04-26 11:00','c');
insert into appointment values(69879231,100000003,103,2,'2018-04-26 11:00','2018-04-26 12:00','c');
insert into appointment values(76983231,100000001,null,3,'2018-04-26 12:00','2018-04-26 13:00','c');
insert into appointment values(86213939,100000004,102,9,'2018-04-27 10:00','2018-04-21 11:00','a');
insert into appointment values(93216548,100000002,101,2,'2018-04-27 10:00','2018-04-27 11:00','b');

insert into medication values(1,'paracetamol','z','n/a');
insert into medication values(2,'actemra','foolki labs','n/a');
insert into medication values(3,'molnupiravir','bale laboratories','n/a');
insert into medication values(4,'paxlovid','bar industries','n/a');
insert into medication values(5,'remdesivir','donald pharmaceuticals','n/a');

insert into prescribes values(1,100000001,1,'2018-04-24 10:47',13216584,'5');
insert into prescribes values(9,100000004,2,'2018-04-27 10:53',86213939,'10');
insert into prescribes values(9,100000004,2,'2018-04-30 16:53',null,'5');

insert into block values(1,1);
insert into block values(1,2);
insert into block values(1,3);
insert into block values(2,1);
insert into block values(2,2);
insert into block values(2,3);
insert into block values(3,1);
insert into block values(3,2);
insert into block values(3,3);
insert into block values(4,1);
insert into block values(4,2);
insert into block values(4,3);

insert into room values(101,'single',1,1,0);
insert into room values(102,'single',1,1,0);
insert into room values(103,'single',1,1,0);
insert into room values(111,'single',1,2,0);
insert into room values(112,'single',1,2,1);
insert into room values(113,'single',1,2,0);
insert into room values(121,'single',1,3,0);
insert into room values(122,'single',1,3,0);
insert into room values(123,'single',1,3,0);
insert into room values(201,'single',2,1,1);
insert into room values(202,'single',2,1,0);
insert into room values(203,'single',2,1,0);
insert into room values(211,'single',2,2,0);
insert into room values(212,'single',2,2,0);
insert into room values(213,'single',2,2,1);
insert into room values(221,'single',2,3,0);
insert into room values(222,'single',2,3,0);
insert into room values(223,'single',2,3,0);
insert into room values(301,'single',3,1,0);
insert into room values(302,'single',3,1,1);
insert into room values(303,'single',3,1,0);
insert into room values(311,'single',3,2,0);
insert into room values(312,'single',3,2,0);
insert into room values(313,'single',3,2,0);
insert into room values(321,'single',3,3,1);
insert into room values(322,'single',3,3,0);
insert into room values(323,'single',3,3,0);
insert into room values(401,'single',4,1,0);
insert into room values(402,'single',4,1,1);
insert into room values(403,'single',4,1,0);
insert into room values(411,'single',4,2,0);
insert into room values(412,'single',4,2,0);
insert into room values(413,'single',4,2,0);
insert into room values(421,'single',4,3,1);
insert into room values(422,'single',4,3,0);
insert into room values(423,'single',4,3,0);

insert into on_call values(101,1,1,'2018-11-04 11:00','2018-11-04 19:00');
insert into on_call values(101,1,2,'2018-11-04 11:00','2018-11-04 19:00');
insert into on_call values(102,1,3,'2018-11-04 11:00','2018-11-04 19:00');
insert into on_call values(103,1,1,'2018-11-04 19:00','2018-11-05 03:00');
insert into on_call values(103,1,2,'2018-11-04 19:00','2018-11-05 03:00');
insert into on_call values(103,1,3,'2018-11-04 19:00','2018-11-05 03:00');

insert into stay values(3215,100000001,111,'2018-05-01','2018-05-04');
insert into stay values(3216,100000003,123,'2018-05-03','2018-05-14');
insert into stay values(3217,100000004,112,'2018-05-02','2018-05-03');

insert into undergoes values(100000001,6,3215,'2018-05-02',3,101);
insert into undergoes values(100000001,2,3215,'2018-05-03',7,101);
insert into undergoes values(100000004,1,3217,'2018-05-07',3,102);
insert into undergoes values(100000004,5,3217,'2018-05-09',6,null);
insert into undergoes values(100000001,7,3217,'2018-05-10',7,101);
insert into undergoes values(100000004,4,3217,'2018-05-13',3,103);

insert into trained_in values(3,1,'2018-01-01','2018-12-31');
insert into trained_in values(3,2,'2018-01-01','2018-12-31');
insert into trained_in values(3,5,'2018-01-01','2018-12-31');
insert into trained_in values(3,6,'2018-01-01','2018-12-31');
insert into trained_in values(3,7,'2018-01-01','2018-12-31');
insert into trained_in values(6,2,'2018-01-01','2018-12-31');
insert into trained_in values(6,5,'2017-01-01','2017-12-31');
insert into trained_in values(6,6,'2018-01-01','2018-12-31');
insert into trained_in values(7,1,'2018-01-01','2018-12-31');
insert into trained_in values(7,2,'2018-01-01','2018-12-31');
insert into trained_in values(7,3,'2018-01-01','2018-12-31');
insert into trained_in values(7,4,'2018-01-01','2018-12-31');
insert into trained_in values(7,5,'2018-01-01','2018-12-31');
insert into trained_in values(7,6,'2018-01-01','2018-12-31');
insert into trained_in values(7,7,'2018-01-01','2018-12-31');

/* queries */
-- q1. names of all physicians who are trained in `procedure` name "bypass surgery"
select distinct
    ph.name
from
    physician as ph
    join trained_in as tr on tr.physician = ph.employeeid
    join `procedure` as pr on pr.code = tr.treatment
where
    pr.name = 'bypass surgery';

-- q2. names of all physicians trained in 'bypass surgery' and affiliated with 'cardiology'
select distinct
    ph.name
from
    physician as ph
    join affiliated_with as aw on (aw.physician, aw.department) = (
        ph.employeeid,
        (
            select
                departmentid
            from
                department
            where
                name = 'cardiology'
        )
    )
    join trained_in as tr on tr.physician = ph.employeeid
    join `procedure` as pr on pr.code = tr.treatment
where
    pr.name = 'bypass surgery';

-- q3
select distinct
    name
from
    nurse as n
    join room as r on r.number = 123
    join on_call as oc on (oc.blockfloor, oc.blockcode) = (r.blockfloor, r.blockcode)
where
    oc.nurse = n.employeeid;

-- q4
select
    pt.name,
    pt.address
from
    patient as pt
    join medication as mc on mc.name = 'remdesivir'
    join prescribes as pc on pc.medication = mc.code
where
    pt.ssn = pc.patient;

-- q5
select distinct
    pt.name,
    pt.insuranceid
from
    patient as pt
    join room as r on r.type = 'icu'
    join stay as s on s.patient = pt.ssn
where
    s.end - s.start >= interval '15 days';

-- q6
select distinct
    n.name as nurse_name
from
    nurse as n
    join `procedure` as pr on pr.name = 'bypass surgery'
    join undergoes as u on u.`procedure` = pr.code
where
    u.assistingnurse = n.employeeid;

-- q7
select distinct
    n.name as assisting_nurse,
    n.position as nurse_position,
    (
        select
            name
        from
            patient as pt
        where
            u.patient = pt.ssn
    ) as patient_name,
    (
        select
            name
        from
            physician as ph
        where
            u.physician = ph.employeeid
    ) as physician_name
from
    nurse as n
    join `procedure` as pr on pr.name = 'bypass surgery'
    join undergoes as u on u.`procedure` = pr.code
where
    n.employeeid = u.assistingnurse;

-- q8
-- here i have also considered the case when the physician is certified at some point of time
-- but when the operation was happening, their certification is not yet issued or has expired
select distinct
    (
        select 
            physician.name
        from 
            physician
        where 
            physician.employeeid = undergoes.physician 
    )
from
    undergoes
where
    undergoes.`procedure` not in
    (
        select 
            treatment
        from
            trained_in
        where
            trained_in.physician = undergoes.physician and undergoes.date >= trained_in.certificationdate
    );

-- q9
select distinct
    ph.name
from
    physician as ph
    join trained_in as tr on tr.physician = ph.employeeid
    join undergoes as u on u.physician = ph.employeeid
where
    tr.treatment = u.`procedure`
    and u.date > tr.certificationexpires;

-- q10
select distinct
    ph.name as physician,
    (
        select
            name
        from
            `procedure` as pr
        where
            pr.code = u.`procedure`
    ) as `procedure`,
    u.date,
    (
        select
            name
        from
            patient as pt
        where
            pt.ssn = u.patient
    ) as patient
from
    physician as ph
    join trained_in as tr on tr.physician = ph.employeeid
    join undergoes as u on u.physician = ph.employeeid
where
    tr.treatment = u.`procedure`
    and u.date > tr.certificationexpires;

-- q11
with
    temp as (
        select
            pt.name as patient,
            pt.pcp as physician,
            (
                select
                    count(*)
                from
                    appointment as ap
                where
                    ap.patient = pt.ssn
                    and ap.physician in (
                        select
                            ph.employeeid
                        from
                            physician as ph
                            join affiliated_with as aw on aw.physician = ph.employeeid
                            join department as dp on dp.departmentid = aw.department
                        where
                            dp.name = 'cardiology'
                    )
            ) as appointmentswithcardiologist,
            (
                select
                    count(*)
                from
                    prescribes as pr
                where
                    pr.patient = pt.ssn
                    and pr.physician = pt.pcp
            ) as physicianprescribed,
            (
                select
                    count(*)
                from
                    undergoes as u
                    join `procedure` as pr on pr.code = u.`procedure`
                where
                    u.patient = pt.ssn
                    and pr.cost > 5000
            ) as costatleastfivek
        from
            patient as pt
    )
select distinct
    patient,
    (
        select
            physician.name
        from
            physician
        where
            physician.employeeid = temp.physician
    )
from
    temp
where
    appointmentswithcardiologist >= 2
    and costatleastfivek >= 1
    and physicianprescribed >= 1
    and physician not in (
        select
            head
        from
            department
    );

-- q12
with medicinepatient as (
    select distinct
        mc.name as medicine,
        mc.brand as brand,
        pc.patient as patient
    from
        medication as mc
        join prescribes as pc on pc.medication = mc.code
),
countprescribed as (
    select
        medicine,
        brand,
        count(*) as times_prescribed
    from
        medicinepatient
    group by
        medicine,
        brand
),
maxprescribed as (
    select
        max(times_prescribed) as max_pres
    from
        countprescribed
)
select
    medicine,
    brand
from
    countprescribed, maxprescribed
where
    countprescribed.times_prescribed = maxprescribed.max_pres;

-- q13
select distinct
    ph.name
from
    physician as ph
    join trained_in as tr on tr.physician = ph.employeeid
    join `procedure` as pr on pr.code = tr.treatment
where
    pr.name = '{}';