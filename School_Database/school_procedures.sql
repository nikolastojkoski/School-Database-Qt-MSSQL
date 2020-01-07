
	  
--Lessons for Student
Create Procedure Student_Lessons @StudentId int as
Begin
  Select UchebniPlan.uchebniyPredmet, Teachers.FIO
 from Lessons, Students, Teachers, UchebniPlan
  where Students.id = @StudentId and Lessons.groupId = Students.groupId and
    Lessons.teacherId = Teachers.id and Lessons.predmetId = UchebniPlan.id
end

--Raspisanie for Student
Create Procedure Student_Raspisanie @StudentId int as
begin
  select day, [time from], [time to], classroom, UchebniPlan.uchebniyPredmet, Teachers.FIO, Lessons.groupId
 from Raspisanie, Students, Teachers, Lessons, UchebniPlan where
   Students.id = @StudentId  and Students.groupId = Lessons.groupId and Teachers.id = Lessons.teacherId and
   UchebniPlan.id = Lessons.predmetId and Raspisanie.lessonId = Lessons.id
     order by day, [time from]
end

--Zurnal for Student
Create Procedure Student_Zurnal @StudentFIO char(50) as
begin
  select uchebniyPredmet, Ocenka, Teachers.FIO, Zurnal.date
  from Zurnal, Students, Lessons, UchebniPlan, Teachers where
    Students.FIO = @StudentFIO and Zurnal.StudentId = Students.id and
	Zurnal.LessonId = Lessons.id and UchebniPlan.id = Lessons.predmetId and 
	Teachers.id = Lessons.teacherId
end

--Lessons for Group
Create Procedure Group_Lessons @GroupId int as
begin
  select UchebniPlan.uchebniyPredmet, Teachers.FIO 
 from Lessons, UchebniPlan, Teachers where Lessons.groupId = @GroupId and
  Lessons.predmetId = UchebniPlan.id and Lessons.teacherId = Teachers.id
end

--Raspisanie for Group
Create Procedure Group_Raspisanie @GroupId int as
begin
 select day, [time from], [time to], classroom, UchebniPlan.uchebniyPredmet, Teachers.FIO, Lessons.groupId
 from Raspisanie, Teachers, Lessons, UchebniPlan where
   Lessons.groupId = @GroupId and Teachers.id = Lessons.teacherId and
   UchebniPlan.id = Lessons.predmetId and Raspisanie.lessonId = Lessons.id
     order by day, [time from]
end

--Zurnal for Group
Create Procedure Group_Zurnal @GroupId int as
begin
 select Students.FIO, uchebniyPredmet, Ocenka, Teachers.FIO, Zurnal.date
  from Zurnal, Students, Lessons, UchebniPlan, Teachers where
    Students.groupId = @GroupId and Zurnal.StudentId = Students.id and
	Zurnal.LessonId = Lessons.id and UchebniPlan.id = Lessons.predmetId and 
	Teachers.id = Lessons.teacherId
end

--Lessons for Teacher
Create Procedure Teacher_Lessons @FIO char(50) as
begin
  select UchebniPlan.uchebniyPredmet, Lessons.groupId, Groups.class
 from Lessons, UchebniPlan, Groups, Teachers where Teachers.FIO = @FIO and
  Teachers.id = Lessons.teacherId and Lessons.predmetId = UchebniPlan.id 
  and Groups.id = Lessons.groupId
end

--Raspisanie for Teacher
Create Procedure Teacher_Raspisanie @FIO char(50) as
begin
  select day, [time from], [time to], classroom, UchebniPlan.uchebniyPredmet, Lessons.groupId
 from Raspisanie, Teachers, Lessons, UchebniPlan where Teachers.FIO = @FIO 
   and Teachers.id = Lessons.teacherId and UchebniPlan.id = Lessons.predmetId
   and Raspisanie.lessonId = Lessons.id
     order by day, [time from]
end

--Avg mark for student
Create Procedure Student_Avg_Mark @FIO char(50) as
begin
 Select avg(CAST(Zurnal.Ocenka as float)) as [Avg. Ocenka] 
  from Zurnal, Students where Students.FIO = @FIO
  and Zurnal.StudentId = Students.id
end

--Avg mark for students in group
Create Procedure Group_Student_Avg @GroupId int as
begin
 select Students.FIO, avg(cast(Zurnal.Ocenka as float)) as [Avg. Ocenka] 
  from Zurnal, Students where Students.groupId = @GroupId and Zurnal.StudentId = Students.id
  group by Students.FIO 
  order by [Avg. Ocenka]
end

 --Avg mark for Group for each Predmet
 Create Procedure Group_Predmet_Avg @GroupId int as
begin
  select UchebniPlan.uchebniyPredmet, cast(avg(cast(Zurnal.Ocenka as float)) as decimal(10,2)) as [Avg. Ocenka]
 from UchebniPlan, Lessons, Zurnal where Lessons.groupId = @GroupId
 and Zurnal.LessonId = Lessons.id and Lessons.predmetId = UchebniPlan.id 
 group by UchebniPlan.uchebniyPredmet order by [Avg. Ocenka]
end


 execute Student_Lessons @StudentId =  1
 execute Student_Raspisanie @StudentId = 6
 execute Student_Zurnal @StudentFIO = 'Нгуен Хай Йен'
 execute Student_Avg_Mark @FIO = 'Чернов Денис Павлович'

 execute Group_Lessons @GroupId = 1
 execute Group_Raspisanie @GroupId = 2;
 execute Group_Zurnal @GroupId = 1;
 execute Group_Student_Avg @GroupId = 2
 execute Group_Predmet_Avg @GroupId = 3

 execute Teacher_Lessons @FIO = 'Попов Борис Петрович'
 execute Teacher_Raspisanie @FIO = 'Устинов Сергей Михайлович'

