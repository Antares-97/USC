/* USC CS585 HW2 Q4 */
/* Junzhe Liu 2270250947 */
/* MySql */

CREATE TABLE junkmail
(Name VARCHAR(20) NOT NULL,
Address VARCHAR(100) NOT NULL,
ID INTEGER, NOT NULL,
SameFam INTEGER,
PRIMARY KEY(ID));

INSERT INTO junkmail VALUES('Bob',     'B', 15, NULL);
INSERT INTO junkmail VALUES('Carmen',  'C', 22, NULL);
INSERT INTO junkmail VALUES('Diego',   'A', 9, 10);
INSERT INTO junkmail VALUES('Ella',    'B', 3, 15);
INSERT INTO junkmail VALUES('Farkhad', 'D', 11, NULL);

/* Please Comment out the above code if you already have a table */

DELETE FROM junkmail AS A 
WHERE A.SameFam IS NULL 
AND EXISTS (
	 SELECT ID 
	 FROM (SELECT * FROM junkmail) AS B 
	 WHERE B.SameFam = A.ID
	 );