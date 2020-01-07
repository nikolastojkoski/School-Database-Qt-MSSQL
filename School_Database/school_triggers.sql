use School2

CREATE TRIGGER add_student_group ON Students
FOR INSERT
AS 
BEGIN
  DECLARE @id_group INT
  SELECT @id_group = INSERTED.groupId FROM INSERTED
  UPDATE Groups SET [number of students] += 1 WHERE id = @id_group
END

CREATE TRIGGER update_student_group ON Students
FOR UPDATE
AS
BEGIN
  DECLARE @id_group_old INT
  DECLARE @id_group_new INT
  SELECT @id_group_old = DELETED.groupId FROM DELETED
  SELECT @id_group_new = INSERTED.groupId FROM INSERTED
  UPDATE Groups SET [number of students] -= 1 WHERE id = @id_group_old
  UPDATE Groups SET [number of students] += 1 WHERE id = @id_group_new
END

CREATE TRIGGER delete_student_group ON Students
FOR DELETE
AS 
BEGIN
  DECLARE @id_group INT
  SELECT @id_group = DELETED.groupId FROM DELETED
  UPDATE Groups SET [number of students] -= 1 WHERE id = @id_group
END

