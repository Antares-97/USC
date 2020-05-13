/* USC CS585 HW2 Q3 */
/* Junzhe Liu 2270250947 */
/* MySql */

CREATE TABLE projects
(PID VARCHAR(10) NOT NULL,
Step INTEGER NOT NULL,
Status VARCHAR(2) NOT NULL,
PRIMARY KEY (PID, Step));

INSERT INTO projects VALUES('P100', 0 ,'C');
INSERT INTO projects VALUES('P100', 1 ,'W');
INSERT INTO projects VALUES('P100', 2 ,'W');
INSERT INTO projects VALUES('P201', 0 ,'C');
INSERT INTO projects VALUES('P201', 1 ,'C');
INSERT INTO projects VALUES('P333', 0 ,'W');
INSERT INTO projects VALUES('P333', 1 ,'W');

/* Please Comment out the above code if you already have a table */

SELECT PID
FROM (
    SELECT PID, COUNT(Status) AS TOTAL
    FROM projects
    WHERE Status = 'C'
    GROUP BY PID
) AS PROJ
WHERE PROJ.TOTAL = 1;