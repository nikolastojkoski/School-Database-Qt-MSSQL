#ifndef STUDENTWINDOW_H
#define STUDENTWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>

namespace Ui {
  class StudentWindow;
}

class StudentWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit StudentWindow(int studentId, QSqlDatabase &database, QWidget *parent = nullptr);
  ~StudentWindow();

private:
  Ui::StudentWindow *ui;

  int studentId;

  QSqlDatabase db;
  QSqlTableModel *tableModel;

  void setupPersonalInfo();

};

#endif // STUDENTWINDOW_H
