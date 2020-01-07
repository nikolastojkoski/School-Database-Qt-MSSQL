
--Groups
Create View [List of Groups] as
Select Groups.id, class, [number of students], Teachers.FIO as [Классный руководитель] 
  from Groups, Teachers
    where Teachers.id = Groups.[Klassniy rukovoditel]

--Lessons
Create View [List of Lessons] as
Select UchebniPlan.uchebniyPredmet, Teachers.FIO, Lessons.groupId
  from Lessons, UchebniPlan, Teachers
    where UchebniPlan.id = Lessons.predmetId and Teachers.id = Lessons.teacherId
	  

--Lessons for Student
Select UchebniPlan.uchebniyPredmet, Teachers.FIO
 from Lessons, Students, Teachers, UchebniPlan
  where Students.id = 5 and Lessons.groupId = Students.groupId and
    Lessons.teacherId = Teachers.id and Lessons.predmetId = UchebniPlan.id

--Raspisanie for Student
select day, [time from], [time to], classroom, UchebniPlan.uchebniyPredmet, Teachers.FIO, Lessons.groupId
 from Raspisanie, Students, Teachers, Lessons, UchebniPlan where
   Students.id = 6  and Students.groupId = Lessons.groupId and Teachers.id = Lessons.teacherId and
   UchebniPlan.id = Lessons.predmetId and Raspisanie.lessonId = Lessons.id
     order by day, [time from]

--Zurnal for Student
select uchebniyPredmet, Ocenka, Teachers.FIO, Zurnal.date
  from Zurnal, Students, Lessons, UchebniPlan, Teachers where
    Students.FIO = 'Петров Денис Петрович' and Zurnal.StudentId = Students.id and
	Zurnal.LessonId = Lessons.id and UchebniPlan.id = Lessons.predmetId and 
	Teachers.id = Lessons.teacherId 

--Lessons for Group
select UchebniPlan.uchebniyPredmet, Teachers.FIO 
 from Lessons, UchebniPlan, Teachers where Lessons.groupId = 4 and
  Lessons.predmetId = UchebniPlan.id and Lessons.teacherId = Teachers.id

--Raspisanie for Group
select day, [time from], [time to], classroom, UchebniPlan.uchebniyPredmet, Teachers.FIO, Lessons.groupId
 from Raspisanie, Teachers, Lessons, UchebniPlan where
   Lessons.groupId = 1 and Teachers.id = Lessons.teacherId and
   UchebniPlan.id = Lessons.predmetId and Raspisanie.lessonId = Lessons.id
     order by day, [time from]

--Zurnal for Group
select Students.FIO, uchebniyPredmet, Ocenka, Teachers.FIO, Zurnal.date
  from Zurnal, Students, Lessons, UchebniPlan, Teachers where
    Students.groupId = 2 and Zurnal.StudentId = Students.id and
	Zurnal.LessonId = Lessons.id and UchebniPlan.id = Lessons.predmetId and 
	Teachers.id = Lessons.teacherId 

--Lessons for Teacher
select UchebniPlan.uchebniyPredmet, Lessons.groupId, Groups.class
 from Lessons, UchebniPlan, Groups, Teachers where Teachers.FIO = 'Попов Борис Петрович' and
  Teachers.id = Lessons.teacherId and Lessons.predmetId = UchebniPlan.id and Groups.id = Lessons.groupId

--Raspisanie for Teacher
select day, [time from], [time to], classroom, UchebniPlan.uchebniyPredmet, Lessons.groupId
 from Raspisanie, Teachers, Lessons, UchebniPlan where Teachers.FIO = 'Устинов Сергей Михайлович' 
   and Teachers.id = Lessons.teacherId and UchebniPlan.id = Lessons.predmetId
   and Raspisanie.lessonId = Lessons.id
     order by day, [time from]

--Total num of students in groups
Create View [Total number of students] as
select sum(Groups.[number of students]) as [Total Students] from Groups

--Avg mark for student
Select avg(CAST(Zurnal.Ocenka as float)) as [Avg. Ocenka] 
  from Zurnal, Students where Students.FIO = 'Круглов Андрей Андрейевич' 
  and Zurnal.StudentId = Students.id

--Avg mark for all students
Create View [Avg Student Marks] as
select Students.FIO, avg(cast(Zurnal.Ocenka as float)) as [Avg. Ocenka]
 from Zurnal, Students where Students.id = Zurnal.StudentId group by Students.FIO
 order by [Avg. Ocenka]

--Avg mark for students in group
select Students.FIO, avg(cast(Zurnal.Ocenka as float)) as [Avg. Ocenka] 
  from Zurnal, Students where Students.groupId = 2 and Zurnal.StudentId = Students.id
  group by Students.FIO 
  order by [Avg. Ocenka]

--Avg mark for all groups
Create View [Avg Group Marks] as
select a.groupId, Groups.[number of students], a.[Avg. Ocenka], Teachers.FIO as [Классный руководитель]
 from 
	(select Students.groupId, cast(avg(cast(Zurnal.Ocenka as float)) as decimal(10,2)) as [Avg. Ocenka]
	 from Zurnal, Students where Zurnal.StudentId = Students.id group by Students.groupId) a,
	 Groups, Teachers
 where a.groupId = Groups.id and Groups.[Klassniy rukovoditel] = Teachers.id

--Avg mark given by Teacher
Create View [Avg Teacher Marks] as
select Teachers.FIO, cast(avg(cast(Zurnal.Ocenka as float)) as decimal(10,2)) as [Avg. Ocenka]
 from Zurnal, Lessons, Teachers where Zurnal.LessonId = Lessons.id and Lessons.teacherId = Teachers.id
 group by Teachers.FIO order by [Avg. Ocenka]

 --Avg mark for Group for each Predmet
 select UchebniPlan.uchebniyPredmet, cast(avg(cast(Zurnal.Ocenka as float)) as decimal(10,2)) as [Avg. Ocenka]
 from UchebniPlan, Lessons, Zurnal where Lessons.groupId = 2
 and Zurnal.LessonId = Lessons.id and Lessons.predmetId = UchebniPlan.id 
 group by UchebniPlan.uchebniyPredmet order by [Avg. Ocenka]

 --Avg mark for Each group for each Predmet
 Create View [Avg mark for Group - Predmet] as
 select Groups.class, a.uchebniyPredmet, a.groupId, a.[Avg. Ocenka] from
	(select Lessons.groupId, UchebniPlan.uchebniyPredmet, 
       cast(avg(cast(Zurnal.Ocenka as float)) as decimal(10,2)) as [Avg. Ocenka]
	   from UchebniPlan, Lessons, Zurnal where Zurnal.LessonId = Lessons.id 
	   and Lessons.predmetId = UchebniPlan.id  group by Lessons.groupId, UchebniPlan.uchebniyPredmet) a,
   Groups where Groups.id = a.groupId order by Groups.class, a.uchebniyPredmet,  a.[Avg. Ocenka]

--Number of lessons each day for each group
Create View [Group - Number of lessons per day] as
select Raspisanie.Day, Lessons.groupId, count(Raspisanie.lessonId) as [Number of Lessons] 
  from Raspisanie, Lessons
  where Raspisanie.lessonId = Lessons.id
  group by Raspisanie.Day, Lessons.groupId 
  order by Raspisanie.day

--Number of lessons each day for each Teacher
Create View [Teacher - Number of lessons per day] as
select Raspisanie.Day, Teachers.FIO, count(Raspisanie.lessonId) as [Number of Lessons]
  from Raspisanie, Lessons, Teachers where Raspisanie.lessonId = Lessons.id and
  Lessons.teacherId = Teachers.id group by Raspisanie.day, Teachers.FIO
  order by Raspisanie.day

--Total number of lessons per week per Teacher

Create View [Teacher - Number of lessons per week] as
select Teachers.FIO, count(Raspisanie.lessonId) as [Number of Lessons] 
  from Raspisanie, Teachers, Lessons where Raspisanie.lessonId = Lessons.id and 
  Lessons.teacherId = Teachers.id group by Teachers.FIO order by [Number of Lessons]

--Number of students per group with (avg mark < 4.0)
Create View [Group - Number of Students with avg < 4.0] as
select Students.groupId, count(a.StudentId) as [Students with AVG < 4.0] from 
  (select StudentId, cast(avg(cast(Zurnal.Ocenka as float)) as decimal(10,2)) as [avg]
  from Zurnal group by StudentId) a, 
  Students where Students.id = a.StudentId and a.avg < 4.0 group by Students.groupId

--List of students with (avg mark < 4.0)
Create View [List of students with avg < 4.0] as
  select Students.class, Students.groupId, Students.FIO, a.avg from 
    (select Zurnal.StudentId, cast(avg(cast(Zurnal.Ocenka as float)) as decimal(10,2)) as [avg]
    from Zurnal group by Zurnal.StudentId) a,
  Students where Students.id = a.StudentId and avg < 4.0
  order by Students.class, a.avg

--Number of Lessons in raspisanie and Number of Lessons according to Uchebni Plan for each Group
Create View [Raspisanie <-> Uchebniy Plan] as
  select b.GroupId, b.[Lessons in uchebni plan per week], ISNULL(c.numLessons, 0) as [Lessons in raspisanie per week] 
  from
    (select Groups.id as GroupId, a.[Lessons in uchebni plan per week] from
      (select UchebniPlan.class, sum(UchebniPlan.[hours per week]) as [Lessons in uchebni plan per week]
       from UchebniPlan group by UchebniPlan.class) a,
     Groups Where Groups.class = a.class) b
  FULL OUTER JOIN
    (select Groups.id, count(Raspisanie.lessonId) as [numLessons] from Raspisanie, Lessons, Groups 
     where Groups.id = Lessons.groupId and Lessons.id = Raspisanie.lessonId group by Groups.id) c
  ON b.GroupId = c.id

--Missing marks for Students
Create View [Student - Kol.Predmetov - Kol.Ocenok] as 
select a.FIO, a.[Kol. predmetov], b.[Kol. ocenok] from
  (select Students.FIO, count(predmetId) as [Kol. predmetov] from Lessons, Students where 
    Students.groupId = Lessons.groupId group by Students.FIO) a,
  (select Students.FIO, count(Zurnal.StudentId) as [Kol. ocenok] from Zurnal, Students
    where Students.id = Zurnal.StudentId group by Students.FIO) b 
where a.FIO = b.FIO order by b.[Kol. ocenok]


select distinct uchebniyPredmet from Lessons, UchebniPlan
  where Lessons.predmetId = UchebniPlan.id and Lessons.teacherId = 1 

select groupId from Lessons, UchebniPlan
  where Lessons.predmetId = UchebniPlan.id and uchebniyPredmet = 'Информатика' and Lessons.teacherId = 1

select Students.FIO, Zurnal.Ocenka, Zurnal.date from Zurnal, Lessons, Students, UchebniPlan
  where Students.id = Zurnal.StudentId and Zurnal.LessonId = Lessons.id 
  and Lessons.teacherId = 1 and Lessons.groupId = 1 and Lessons.predmetId = UchebniPlan.id
  and UchebniPlan.uchebniyPredmet = 'Информатика'


select Zurnal.Ocenka from Zurnal, Students where Zurnal.StudentId = Students.id 
and Students.FIO = 'Чернов Денис Павлович'

select day, classroom from Raspisanie, Lessons
where Raspisanie.lessonId = Lessons.id and Lessons.groupId = 2