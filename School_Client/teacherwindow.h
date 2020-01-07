#ifndef TEACHERWINDOW_H
#define TEACHERWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>
#include <QStringList>
#include <QComboBox>
#include <QTableView>

namespace Ui {
  class TeacherWindow;
}

class TeacherWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit TeacherWindow(int teacherId, QSqlDatabase &database, QWidget *parent = nullptr);
  ~TeacherWindow();

private slots:
  void zurnalSelectionChanged(const QModelIndex &current, const QModelIndex&);
  void on_predmetComboBox_currentIndexChanged(int);
  void on_groupComboBox_currentIndexChanged(int);
  void on_editButton_clicked();
  void on_insertButton_clicked();
  void on_deleteButton_clicked();

private:
  Ui::TeacherWindow *ui;
  QSqlDatabase db;
  int teacherId;
  QComboBox* predmetComboBox;
  QComboBox* groupComboBox;

  QSqlQueryModel* currentZurnalModel;
  QTableView* zurnalTableView;
  int zurnalRowIndex = -1;


  void setupRaspisanie();
  void setupZurnal();
  void setupPersonalInfo();
  void updateModel(QSqlQueryModel* model);
  void updateZurnalTableView();
  QStringList* getListOfStudents();
  QString getCurrentLessonId();
  QString getStudentId(const QString &fio);
  QString getDate();
};

#endif // TEACHERWINDOW_H
