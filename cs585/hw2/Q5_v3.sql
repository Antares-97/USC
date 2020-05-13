/* USC CS585 HW2 Q5 version 3*/
/* Junzhe Liu 2270250947 */
/* MySql */

/* Please run Q5.sql first, it includes a create table */

SELECT DISTINCT Instructor
FROM candidates AS A
WHERE 'JavaScript' IN (
    SELECT Subject
    FROM candidates
    WHERE Instructor = A.Instructor)
AND 'Scratch' IN (
    SELECT Subject
    FROM candidates
    WHERE Instructor = A.Instructor)
AND 'Python' IN (
    SELECT Subject
    FROM candidates
    WHERE Instructor = A.Instructor);
    
/*
    In the third method, I use subquery in the WHERE,
    each time we get a Instructor name from table A(candidates),
    we can get all the subjects this instructor can teach,
    and check if every desired subject is in this list.
/*