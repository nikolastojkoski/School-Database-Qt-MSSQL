#ifndef QUERIES_H
#define QUERIES_H

#include <QString>

namespace queries
{
  //todo: add more namespaces, queries::student::selectRaspisanie, queries::teacher::selectRaspisanie

  static QString selectUserInfo = "SELECT user_type, id FROM Users WHERE username='%1'";

  static QString getStudentRaspisanie = "select CASE day"
                                 " when 1 then 'Понедельник' when 2 then 'Вторник' when 3 then 'Среда'"
                                 " when 4 then 'Четверг' when 5 then 'Пятница' END as День, "
                                 " [time from] as [Время од], [time to] as [Время до], "
                                 " classroom as аудитория, UchebniPlan.uchebniyPredmet as Предмет, "
                                 " Teachers.FIO as Учитель "
                       " from Raspisanie, Students, Teachers, Lessons, UchebniPlan where"
                         " Students.id = %1  and Students.groupId = Lessons.groupId and Teachers.id = Lessons.teacherId and"
                         " UchebniPlan.id = Lessons.predmetId and Raspisanie.lessonId = Lessons.id"
                           " order by day, [time from]";

  static QString getStudentZurnal = "select uchebniyPredmet as Предмет, Ocenka as Оценка, Teachers.FIO as Учитель, Zurnal.date as Дата"
                             " from Zurnal, Lessons, UchebniPlan, Teachers where"
                               " Zurnal.StudentId = %1 and"
                             " Zurnal.LessonId = Lessons.id and UchebniPlan.id = Lessons.predmetId and"
                             " Teachers.id = Lessons.teacherId order by Zurnal.date";

  static QString selectClassUchebniPlan = "select predmetnayaOblast as [Предметная облсть], uchebniyPredmet as [Учебный предмет], [hours per week] as [Количество занятии в неделю] from UchebniPlan where class = %1";
  static QString selectTeacherRaspisanie = "select CASE day"
                                           " when 1 then 'Понедельник' when 2 then 'Вторник' when 3 then 'Среда'"
                                           " when 4 then 'Четверг' when 5 then 'Пятница' END as День, "
      " [time from] as [Время од], [time to] as [Время до], classroom as аудитория, UchebniPlan.uchebniyPredmet as Предмет, "
      " Lessons.groupId as группа"
      " from Raspisanie, Lessons, UchebniPlan where Lessons.teacherId = %1"
      " and UchebniPlan.id = Lessons.predmetId"
      " and Raspisanie.lessonId = Lessons.id"
      " order by day, [time from]";

  static QString selectTeacherPredmets = "select distinct uchebniyPredmet from Lessons, UchebniPlan"
    " where Lessons.predmetId = UchebniPlan.id and Lessons.teacherId = %1";

  static QString selectTeacherPredmetGroups = "select groupId from Lessons, UchebniPlan"
    " where Lessons.predmetId = UchebniPlan.id and uchebniyPredmet = '%1' and Lessons.teacherId = %2";


  static QString selectTeacherGroupPredmetZurnal = "select Students.FIO as Ученик, Zurnal.Ocenka as Оценка, Zurnal.date as Дата,"
    " Zurnal.id from Zurnal, Lessons, Students, UchebniPlan"
    " where Students.id = Zurnal.StudentId and Zurnal.LessonId = Lessons.id"
    " and Lessons.teacherId = %1 and Lessons.groupId = %2 and Lessons.predmetId = UchebniPlan.id"
    " and UchebniPlan.uchebniyPredmet = '%3'";

  static QString selectStudentsFromGroup = "select FIO from Students where groupId = %1";

  static QString updateStudentZurnal = "UPDATE Zurnal SET Ocenka=%2, date='%3' WHERE Zurnal.id = %1";

  static QString selectLessonId_o = "select id from Lessons where Lessons.teacherId = %1 and Lessons.groupId = %2";

  static QString selectStudentId = "select id from Students where FIO = '%1'";

  static QString insertZurnalEntry = "INSERT INTO Zurnal(StudentId, LessonId, Ocenka, date) VALUES ('%1','%2','%3','%4')";

  static QString selectGetDate = "select getDate()";

  static QString deleteZurnalEntry = "delete from Zurnal where id = %1";

  static QString selectStudents = "select FIO as Студент, [date of birth] as [Дата рождения], class as Класс, groupId as Группа, id from Students";

  static QString selectTeachers = "select FIO as Учитель, [date of birth] as [Дата рождения], [date of employment] as [Дата трудоустройства], id from Teachers";

  //static QString selectGroups = "  select Groups.id as ID, class as Класс, [number of students] as [Количество студентов], Teachers.FIO as [Классный руководитель] from Groups, Teachers where Teachers.id = Groups.[Klassniy rukovoditel]";

  static QString selectGroups = "select Groups.id as ID, class as Класс, [number of students] as [Количество студентов], Teachers.FIO as [Классный руководитель] from Groups LEFT JOIN Teachers ON Groups.[Klassniy rukovoditel] = Teachers.id";

  static QString selectGroupIds = "select id from Groups where class = %1";

  static QString insertStudent = "INSERT INTO Students(FIO, [date of birth], class, groupId) VALUES ('%1', '%2', '%3', '%4')";

  static QString deleteStudent = "delete from Zurnal where StudentId = %1 delete from Students where id = %1";

  static QString updateStudent = "UPDATE Students SET FIO = '%1', [date of birth] = '%2', class = '%3', groupId = '%4' WHERE id = %5";

  static QString selectTeacherFIO = "select FIO from Teachers where id = %1";

  static QString insertGroup = "INSERT INTO Groups(class, [Klassniy rukovoditel]) VALUES ('%1', '%2')";

  static QString updateGroup = "UPDATE Groups SET class = '%1', [Klassniy rukovoditel] = '%2' where id = '%3'";

  static int deleteGroupNumQueries = 5;
  static QString deleteGroup[5] = {"DELETE Raspisanie FROM Raspisanie INNER JOIN Lessons ON Raspisanie.lessonId = Lessons.id and Lessons.groupId = %1"
      ,"DELETE Zurnal FROM Zurnal INNER JOIN Lessons ON Zurnal.LessonId = Lessons.id and Lessons.groupId = %1"
      ,"DELETE from Lessons where Lessons.groupId = %1"
      ,"DELETE from Students where Students.groupId = %1"
      ,"DELETE from Groups where Groups.id = %1"};

  static QString insertTeacher = "INSERT INTO Teachers(FIO, [date of birth], [date of employment]) VALUES ('%1', '%2', '%2')";

  static int deleteTeacherNumQueries = 4;
  static QString deleteTeacher[4] = {"UPDATE Groups SET [Klassniy rukovoditel] = null where [Klassniy rukovoditel] = %1",
                                     "UPDATE Lessons SET teacherId = null where teacherId = %1",
                                     "DELETE from Qualifications where teacherId = %1",
                                     "DELETE from Teachers where Teachers.id = %1"};

  static QString updateTeacher = "UPDATE Teachers SET FIO = '%1', [date of birth] = '%2', [date of employment] = '%3' where Teachers.id = %4";

  static QString selectUchebniPlan = "select class as Класс, predmetnayaOblast as [Предметная область],"
     " uchebniyPredmet as [Учебный предмет], [hours per week] as [Кол. часов в неделю], id from UchebniPlan";

  static QString insertPredmet = "INSERT INTO UchebniPlan(class, predmetnayaOblast, uchebniyPredmet, [hours per week]) VALUES ('%1', '%2', '%3', '%4')";

  static QString updatePredmet = "UPDATE UchebniPlan SET class = %1, predmetnayaOblast = '%2', uchebniyPredmet = '%3', [hours per week] = '%4' where id = %5";

  static int deletePredmetNumQueries = 2;
  static QString deletePredmet[2] = {"UPDATE Lessons SET Lessons.predmetId = null where Lessons.predmetId = %1",
                                     "DELETE from UchebniPlan where UchebniPlan.id = %1"};

  static QString selectLessons = "select  UchebniPlan.uchebniyPredmet as 'Предмет', Teachers.FIO as 'Учитель', groupId as Группа, Lessons.id"
      " from Lessons, UchebniPlan, Teachers  where UchebniPlan.id = Lessons.predmetId and Teachers.id = Lessons.teacherId";

  static QString insertLesson = "INSERT INTO Lessons(predmetId, teacherId, groupId) VALUES ('%1','%2','%3')";

  static QString selectPredmets = "select distinct uchebniyPredmet from UchebniPlan";

  static QString selectPredmetId = "select UchebniPlan.id from UchebniPlan where uchebniyPredmet = '%1'";

  static int deleteLessonsNumQueries = 3;
  static QString deleteLesson[3] = {"DELETE from Zurnal where Zurnal.LessonId = %1",
                                    "DELETE from Raspisanie where Raspisanie.lessonId = %1",
                                    "DELETE from Lessons where Lessons.id = %1"};
  static QString updateLesson = "UPDATE Lessons SET predmetId = '%1', teacherId = '%2', groupId = '%3' WHERE Lessons.id = %4";

  static QString selectNumberOfStudentsInGroup = "select count(id) as sum from Students where groupId = %1";
  static QString selectGroupsIds = "select id from Groups";

  static QString selectRaspisanie = "select CASE day"
            " when 1 then 'Понедельник' when 2 then 'Вторник' when 3 then 'Среда'"
            " when 4 then 'Четверг' when 5 then 'Пятница' END as День, "
            " [time from] as [Время од], [time to] as [Время до], classroom as аудитория, UchebniPlan.uchebniyPredmet as Предмет, "
            " Teachers.FIO as Учитель, Lessons.groupId as группа, Raspisanie.id"
            " from Raspisanie, Lessons, UchebniPlan, Teachers where UchebniPlan.id = Lessons.predmetId and Raspisanie.lessonId = Lessons.id"
            " and Teachers.id = Lessons.teacherId order by day, [time from]";
  static QString selectGroupsWhichHavePredmet = "select distinct groupId from Lessons where predmetId = %1";
  static QString selectTeachersWhichHavePredmet = "select distinct Teachers.FIO from Lessons, Teachers where Lessons.teacherId = Teachers.id and Lessons.predmetId = %1";
  static QString selectGroupsWhichHaveTeacherPredmet = "select distinct Lessons.groupId from Lessons where Lessons.teacherId = %1 and Lessons.predmetId = %2";
  static QString selectLessonId = "select id from Lessons where predmetId = %1 and teacherId = %2 and groupId = %3";

  static QString insertRaspisanie = "INSERT INTO Raspisanie(day, [time from], [time to], classroom, lessonId) VALUES ('%1','%2', '%3', '%4', '%5')";
  static QString deleteRaspisanie = "DELETE from Raspisanie where id = %1";
  static QString updateRaspisnie = "UPDATE Raspisanie SET day = %1, [time from] = '%2', [time to] = '%3', classroom = '%4', lessonId = %5 where Raspisanie.id = %6";

  static int numAdditionalQueries = 12;
  static QString selectAdditionalTitles[12] = {"Количество учеников","Ученик - количество предметов - количество оценок",
                                               "Ученик - средняя оценка","Ученики у которых средняя оценка < 4.0",
                                               "Группа - средняя оценка", "Группа - средняя оценка для предмет",
                                               "Группа - количество занятии в день","Группа - количество студентов с средняя оценка < 4.0",
                                               "Преподаватель - средняя оценка","Преподаватель - количество занятии в неделю",
                                               "Преподаватель - количество занятии в день", "Соответствие занятии с учебного плана",};
  static QString selectAdditional[12] = {"select [Total Students] as [Количество учеников] from [Total number of students]",
                                         "select FIO as Студент, [Kol. predmetov] as [Количество предметов], [Kol. ocenok] as [Количество оценок] from [Student - Kol.Predmetov - Kol.Ocenok]",
                                         "select FIO as [Ученик], [Avg. Ocenka] as [Средняя оценка] from [Avg Student Marks]",
                                         "select class as класс, groupId as [Группа ID], FIO as Ученик, avg as [Средняя оценка] from [List of students with avg < 4.0]",
                                         "select [groupId] as [Группа ID], [number of students] as [Количество студентов], [Avg. Ocenka] as [Средняя оценка], [Классный руководитель] from [Avg Group Marks]",
                                         "select class as класс, uchebniyPredmet as [Учебный предмет], groupId as [Группа ID], [Avg. Ocenka] as [Средняя оценка] from [Avg mark for Group - Predmet]",
                                         "select Day as День, groupId as [Группа ID], [Number of Lessons] as [Количество занятии в день] from [Group - Number of lessons per day]",
                                         "select groupId as [Группа ID], [Students with AVG < 4.0] as [Количество студентов с AVG < 4.0] from [Group - Number of Students with avg < 4.0]",
                                         "select FIO as [Учитель], [Avg. Ocenka] as [Средняя оценка] from [Avg Teacher Marks]",
                                         "select fio as ФИО, [Number of Lessons] as [Кол. занятии в неделю] from [Teacher - Number of lessons per week]",
                                         "select Day as День, FIO as Учитель, [Number of Lessons] as [Количество занятий в день] from [Teacher - Number of lessons per day]",
                                         "select GroupId as [Группа ID], [Lessons in uchebni plan per week] as [Количество занятии в уч. план в неделю], [Lessons in raspisanie per week] as [Количество занятии в расписание в неделю] from [Raspisanie <-> Uchebniy Plan]"
                                        };





}


#endif // QUERIES_H
