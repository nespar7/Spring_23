public class Main {
    public static void main(String[] args) {
        // Setting url, username, password
        String url = "jdbc:mysql://10.5.18.69:3306/20CS10038";
        String username = "20CS10038";
        String password = "20CS10038";
        // Initialising a MySQLConnector object
        MySQLConnector connector = new MySQLConnector(url, username, password);
        // Adding queries
        connector.addQuery(1, """
                    select distinct
                        ph.name
                    from
                        physician as ph
                        join trained_in as tr on tr.physician = ph.employeeid
                        join `procedure` as pr on pr.code = tr.treatment
                    where
                        pr.name = 'bypass surgery';
                """);
        connector.addQuery(2, """
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
                """);
        connector.addQuery(3, """
                    select distinct
                        name
                    from
                        nurse as n
                        join room as r on r.number = 123
                        join on_call as oc on (oc.blockfloor, oc.blockcode) = (r.blockfloor, r.blockcode)
                    where
                        oc.nurse = n.employeeid;
                """);
        connector.addQuery(4, """
                    select
                        pt.name,
                        pt.address
                    from
                        patient as pt
                        join medication as mc on mc.name = 'remdesivir'
                        join prescribes as pc on pc.medication = mc.code
                    where
                        pt.ssn = pc.patient;
                """);
        connector.addQuery(5, """
                    select distinct
                        pt.name,
                        pt.insuranceid
                    from
                        patient as pt
                        join room as r on r.type = 'icu'
                        join stay as s on s.patient = pt.ssn
                    where
                        timestampdiff(DAY, s.start, s.end) >= 15;
                """);
        connector.addQuery(6, """
                    select distinct
                        n.name as nurse_name
                    from
                        nurse as n
                        join `procedure` as pr on pr.name = 'bypass surgery'
                        join undergoes as u on u.`procedure` = pr.code
                    where
                        u.assistingnurse = n.employeeid;
                """);
        connector.addQuery(7, """
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
                """);
        connector.addQuery(8, """
                    select distinct
                        (
                            select
                                physician.name
                            from
                                physician
                            where
                                physician.employeeid = undergoes.physician
                        ) as physician
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
                """);
        connector.addQuery(9, """
                    select distinct
                      ph.name
                    from
                        physician as ph
                        join trained_in as tr on tr.physician = ph.employeeid
                        join undergoes as u on u.physician = ph.employeeid
                    where
                        tr.treatment = u.`procedure`
                        and u.date > tr.certificationexpires;
                """);
        connector.addQuery(10, """
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
                """);
        connector.addQuery(11, """
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
                        ) as physician
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
                """);
        connector.addQuery(12, """
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
                """);
        connector.addQuery(13, """
                    select distinct
                        ph.name
                    from
                        physician as ph
                        join trained_in as tr on tr.physician = ph.employeeid
                        join `procedure` as pr on pr.code = tr.treatment
                    where
                        pr.name = '%s';
                """);

        // Connect to the database and execute the queries
        connector.connect();
    }
}