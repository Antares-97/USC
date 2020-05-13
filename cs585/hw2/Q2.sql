/* USC CS585 HW2 Q2 */
/* Junzhe Liu 2270250947 */
/* MySql */

CREATE TABLE enrollment
(SID INTEGER NOT NULL,
ClassName VARCHAR(20) NOT NULL,
Grade VARCHAR(2) NOT NULL,
PRIMARY KEY (SID, ClassName));

INSERT INTO enrollment VALUES(123, 'Processing', 'A');
INSERT INTO enrollment VALUES(123, 'Python',     'B');
INSERT INTO enrollment VALUES(123, 'Scratch',    'B');
INSERT INTO enrollment VALUES(662, 'Java',       'B');
INSERT INTO enrollment VALUES(662, 'Python',     'A');
INSERT INTO enrollment VALUES(662, 'JavaScript', 'A');
INSERT INTO enrollment VALUES(662, 'Scratch',    'B');
INSERT INTO enrollment VALUES(345, 'Scratch',    'A');
INSERT INTO enrollment VALUES(345, 'JavaScript', 'B');
INSERT INTO enrollment VALUES(345, 'Python',     'A');
INSERT INTO enrollment VALUES(555, 'Python',     'B');
INSERT INTO enrollment VALUES(555, 'JavaScript', 'B');

/* Please Comment out the above code if you already have a table */

SELECT ClassName, COUNT(SID) AS TOTAL
FROM enrollment
GROUP BY CLASSNAME
ORDER BY TOTAL DESC;