#ifndef PRINCIPALWINDOW_H
#define PRINCIPALWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>
#include <QStringList>
#include <QComboBox>
#include <QTableView>
#include <map>

namespace Ui {
  class PrincipalWindow;
}

class PrincipalWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit PrincipalWindow(QSqlDatabase &database, QWidget *parent = nullptr);
  ~PrincipalWindow();

private slots:
  void on_insertStudentButton_clicked();
  void on_deleteStudentButton_clicked();
  void on_editStudentButton_clicked();
  void on_editGroupButton_clicked();
  void on_insertGroupButton_clicked();
  void on_deleteGroupButton_clicked();
  void on_insertTeacherButton_clicked();
  void on_deleteTeacherButton_clicked();
  void on_editTeacherButton_clicked();
  void on_insertPredmetButton_clicked();
  void on_deletePredmetButton_clicked();
  void on_editPredmetButton_clicked();
  void on_insertLessonButton_clicked();
  void on_deleteLessonButton_clicked();
  void on_editLessonButton_clicked();
  void on_insertRaspisanieButton_clicked();
  void on_deleteRaspisanieButton_clicked();
  void on_editRaspisanieButton_clicked();
  void on_tabWidget_currentChanged(int index);
  void on_sortStudentComboBox_currentIndexChanged(int index);
  void on_sortGroupComboBox_currentIndexChanged(int index);
  void on_sortTeacherComboBox_currentIndexChanged(int index);
  void on_sortPredmetComboBox_currentIndexChanged(int index);
  void on_sortLessonComboBox_currentIndexChanged(int index);
  void on_sortRaspisanieComboBox_currentIndexChanged(int index);
  void on_additionalComboBox_currentIndexChanged(int index);

private:
  Ui::PrincipalWindow *ui;
  QSqlDatabase db;

  std::map<QString, QSqlQueryModel*> models;
  std::map<QString, QTableView*> tableViews;
  std::map<QString, QComboBox*> sortComboBoxes;

  void setupTable(const QString &tableViewName, const QString &query,
                  const QString &accessibleName, int columnToHide = -1);
  void setupSortComboBox(const QString &comboBoxName, const QString &tableAccessibleName, int numTableColumns);
  void setupAdditionalComboBox();
  void setTableSortMode(const QString &tableAccessibleName, int column);
  void updateModel(QSqlQueryModel *model);
  void updateTableView(QTableView* tableView, int columnToHide = -1);

  void updateTable(const QString &tableAccessibleName, int columnToHide = -1);


  bool isAnyRowSelected(const QString &tableAccesibleName);
  int getSelectedRow(const QString &tableAccessibleName);
  QString getCellValue(const QString &tableAccessibleName, int row, int col);
  QString getHeaderValue(const QString &tableAccessibleName, int column);

  QString getDate();
  QString getTeacherFIO(const QString &teacherId);
  QString getTeacherId(const QString &teacherFIO);
  QString getPredmetId(const QString &predmet);
  QString getLessonId(const QString &predmetId, const QString &teacherId, const QString &groupId);
  QStringList* getTeacherList();
  QStringList* getTeacherListForPredmet(const QString &predmetId);
  QStringList* getGroupList(const QString &class_num);
  QStringList* getGroupListForPredmet(const QString &predmetId);
  QStringList* getGroupListForTeacherPredmet(const QString &teacherId, const QString &predmetId);
  QStringList* getGroupList();
  QStringList* getPredmetList();
  QStringList* getDaysOfWeekList();
  QString getDayIdx(QString dayName);


};

#endif // PRINCIPALWINDOW_H
