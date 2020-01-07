use School2
create database School2

drop table Raspisanie
drop table Zurnal
drop table Qualifications
drop table Lessons
drop table UchebniPlan
drop table Students
drop table Groups
drop table Teachers


create table Teachers(
  id int IDENTITY(1,1) primary key not null,
  FIO char(50) not null,
  [date of birth] date not null,
  [date of employment] date not null
)

create table Qualifications(
  teacherId int FOREIGN KEY REFERENCES Teachers(id) not null,
  [date of acquirement] date not null,
  [type of qualification] char(60),
  [description] text not null
)

create table Groups(
  id int IDENTITY(1,1) primary key not null,
  class smallint,
  [number of students] smallint not null DEFAULT 0,
  [Klassniy rukovoditel] int FOREIGN KEY REFERENCES Teachers(id)
)

create table Students(
  id int IDENTITY(1,1) primary key not null,
  FIO char(50) not null,
  [date of birth] date not null,
  class smallint,
  groupId int FOREIGN KEY REFERENCES Groups(id)
)

create table UchebniPlan(
  id int IDENTITY(1,1) primary key not null,
  class smallint not null,
  predmetnayaOblast char(60) not null,
  uchebniyPredmet char(60) not null,
  [hours per week] smallint not null
)

create table Lessons(
  id int IDENTITY(1,1) primary key not null,
  predmetId int REFERENCES UchebniPlan(id) not null,
  teacherId int REFERENCES Teachers(id) not null,
  groupId int REFERENCES Groups(id) not null
)

create table Raspisanie(
  id int IDENTITY(1,1) primary key not null,
  lessonId int REFERENCES Lessons(id) not null,
  [day] tinyint not null CHECK ([day] > 0 and [day] < 7),
  [time from] time(7) not null,
  [time to] time(7) not null,
  classroom char(30)
)

create table Zurnal(
  id int IDENTITY(1,1) primary key not null,
  StudentId int REFERENCES Students(id) not null,
  LessonId int REFERENCES Lessons(id) not null,
  Ocenka tinyint not null,
  [date] date not null DEFAULT GETDATE()
)

