/* USC CS585 HW2 Q5 version 2*/
/* Junzhe Liu 2270250947 */
/* MySql */

/* Please run Q5.sql first, it includes a create table */

SELECT A.Instructor
FROM ((
        SELECT Instructor
        FROM candidates
        WHERE Subject = 'JavaScript') AS A
    JOIN (
        SELECT Instructor
        FROM candidates
        WHERE Subject = 'Scratch') AS B
    ON A.Instructor = B.Instructor
    JOIN (SELECT Instructor
        FROM candidates
        WHERE Subject = 'Python') AS C
    ON B.Instructor = C.Instructor);
    
/*
    This second method takes the advantage of JOIN function,
    which will automatically takes the same part between the tables.
    Here we extract the instructors who can teach JavaScript, Scratch, 
    and Python relatively, and JOIN the 3 tables together.
    We will get a list of instructors who can teach all the 3 courses.
*/