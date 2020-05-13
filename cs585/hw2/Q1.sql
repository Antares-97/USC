/* USC CS585 HW2 Q1 */
/* Junzhe Liu 2270250947 */
/* MySql */

CREATE TABLE ProjectRoomBookings 
(roomNum INTEGER NOT NULL, 
startTime INTEGER NOT NULL, 
endTime INTEGER NOT NULL, 
groupName CHAR(10) NOT NULL, 
PRIMARY KEY (roomNum, startTime));

/*  
    Fix of Problem 1: By adding a constraint,
    which forces the startTime be larger than the endTime:
*/

ALTER TABLE ProjectRoomBookings ADD CHECK (startTime < endTime);

/* Insert booking, the last one fails*/
INSERT INTO ProjectRoomBookings VALUES (1, 7, 10, 'group1');
INSERT INTO ProjectRoomBookings VALUES (1, 11, 13, 'group2');
INSERT INTO ProjectRoomBookings VALUES (1, 18, 17, 'group3');

/*  
    Fix of Problem 2: In every insertion, we search through the table,
	if there's a reservation conflicts with the new booking,
    the insertion will fail,
	otherwise put the new reservation in the table. For example:
*/

INSERT INTO ProjectRoomBookings(RoomNum, startTime, endTime, groupName) 
SELECT 1, 8, 9, 'group4' 
FROM dual 
WHERE NOT EXISTS (
 	SELECT * FROM ProjectRoomBookings 
 	WHERE RoomNum = @room 
 	AND ((startTime < 8 AND 8 < endTime)
 	OR   (startTime < 9 AND 9 < endTime)
 	OR   (8 < startTime AND startTime < 9))
 	);

/*  
    The above insert will fail, 
    Try change the value above, to make a successful insert
*/