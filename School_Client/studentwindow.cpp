#include "studentwindow.h"
#include "ui_studentwindow.h"

#include <QTabWidget>
#include <QTableView>
#include <QSqlQuery>
#include "queries.h"

StudentWindow::StudentWindow(int studentId, QSqlDatabase &database, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::StudentWindow)
{
  ui->setupUi(this);

  this->studentId = studentId;
  db = database;

  QSqlQuery query(db);
  query.exec(QString("SELECT FIO from Students where id = %1").arg(studentId));
  query.next();
  setWindowTitle(query.value(0).toString());

  QTabWidget* tabWidget = findChild<QTabWidget*>("tabWidget");
  tabWidget->setTabText(0, "Личные данные");
  tabWidget->setTabText(1, "Расписание");
  tabWidget->setTabText(2, "Журнал");
  tabWidget->setTabText(3, "Учебный план");
  tabWidget->setCurrentIndex(0);

  setupPersonalInfo();

  QSqlQueryModel *raspisanieModel = new QSqlQueryModel;
  raspisanieModel->setQuery(queries::getStudentRaspisanie.arg(studentId), db);

  QTableView* raspisanieTableView = findChild<QTableView*>("raspisanieTableView");
  raspisanieTableView->setModel(raspisanieModel);
  raspisanieTableView->update();
  raspisanieTableView->resizeColumnsToContents();

  QSqlQueryModel *zurnalModel = new QSqlQueryModel;
  zurnalModel->setQuery(queries::getStudentZurnal.arg(studentId), db);

  QTableView* zurnalTableView = findChild<QTableView*>("zurnalTableView");
  zurnalTableView->setModel(zurnalModel);
  zurnalTableView->update();
  zurnalTableView->resizeColumnsToContents();

  QString klass = findChild<QLabel*>("classLabel")->text();
  QSqlQueryModel *uchebniPlanModel = new QSqlQueryModel;
  uchebniPlanModel->setQuery(queries::selectClassUchebniPlan.arg(klass));

  QTableView* uchebniPlanTableView = findChild<QTableView*>("uchebniPlanTableView");
  uchebniPlanTableView->setModel(uchebniPlanModel);
  uchebniPlanTableView->update();
  uchebniPlanTableView->resizeColumnsToContents();
}

StudentWindow::~StudentWindow()
{
  delete ui;
}

void StudentWindow::setupPersonalInfo()
{
  QSqlQuery query(db);
  query.exec(QString("SELECT * from Students where id = %1").arg(studentId));
  query.next();

  QString fio = query.value(1).toString();
  QString dateOfBirth = query.value(2).toString();
  int klass = query.value(3).toInt();
  int groupId = query.value(4).toInt();

  QStringList fioList = fio.split(" ");

  findChild<QLabel*>("studentIdLabel")->setText(QString::number(studentId));
  findChild<QLabel*>("surnameLabel")->setText(fioList.at(0));
  findChild<QLabel*>("nameLabel")->setText(fioList.at(1));
  findChild<QLabel*>("fatherNameLabel")->setText(fioList.at(2));
  findChild<QLabel*>("birthdateLabel")->setText(dateOfBirth);
  findChild<QLabel*>("classLabel")->setText(QString::number(klass));
  findChild<QLabel*>("groupLabel")->setText(QString::number(groupId));

}
