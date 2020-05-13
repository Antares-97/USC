/* USC CS585 HW2 Q5 */
/* Junzhe Liu 2270250947 */
/* MySql */

CREATE TABLE candidates
(Instructor VARCHAR(20) NOT NULL,
Subject VARCHAR(100) NOT NULL,
PRIMARY KEY (Instructor, Subject));

INSERT INTO candidates VALUES ('Aleph', 'Scratch');
INSERT INTO candidates VALUES ('Aleph', 'Java');
INSERT INTO candidates VALUES ('Aleph', 'Processing');
INSERT INTO candidates VALUES ('Bit',   'Python');
INSERT INTO candidates VALUES ('Bit',   'JavaScript');
INSERT INTO candidates VALUES ('Bit',   'Java');
INSERT INTO candidates VALUES ('CRC',   'Python');
INSERT INTO candidates VALUES ('CRC',   'JavaScript');
INSERT INTO candidates VALUES ('Dat',   'JavaScript');
INSERT INTO candidates VALUES ('Dat',   'Python');
INSERT INTO candidates VALUES ('Dat',   'Scratch');
INSERT INTO candidates VALUES ('Emscr', 'Scratch');
INSERT INTO candidates VALUES ('Emscr', 'Processing');
INSERT INTO candidates VALUES ('Emscr', 'JavaScript');
INSERT INTO candidates VALUES ('Emscr', 'Python');

/* Please Comment out the above code if you already have a table */
    
SELECT DISTINCT Instructor
FROM candidates AS A
WHERE Subject = 'JavaScript'
AND EXISTS (
	SELECT Instructor
	FROM candidates AS B
	WHERE A.Instructor = B.Instructor
	AND B.Subject = 'Scratch')
AND EXISTS (
	SELECT Instructor
	FROM candidates AS C
	WHERE A.Instructor = C.Instructor
	AND C.Subject = 'Python');

/*  
    The above query looks all the instructors who can teach 
    JavaScript, Scratch and Python.
    If you want to try other subjects, just modify the Subject value above,
    You can also try more or less subjects by adding/deleting several EXISTS.
    
    In this version, I use the EXISTS subquery to add constraint
    on the instructors, the first WHERE filters out the instructors
    who can teach JavaScript. The Following EXISTS subquery then finds out
    those who can also teach Scratch among the previous instructors.
    Likewise, the second EXISTS goes further to figure out who can teach Python.
    3 Criteria are connected by AND, meaning the result will be
    the instructors who can teach all the 3 subjects.
*/