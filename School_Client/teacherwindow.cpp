#include "teacherwindow.h"
#include "ui_teacherwindow.h"
#include <QSqlQuery>
#include "queries.h"
#include "editdialog.h"

#include <iostream>

TeacherWindow::TeacherWindow(int teacherId, QSqlDatabase &database, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::TeacherWindow)
{
  ui->setupUi(this);
  db = database;
  this->teacherId = teacherId;

  QSqlQuery query(db);
  query.exec(QString("select FIO from Teachers where id = %1").arg(teacherId));
  query.next();
  setWindowTitle(query.value(0).toString());

  QTabWidget* tabWidget = findChild<QTabWidget*>("tabWidget");
  tabWidget->setTabText(0, "Личные данные");
  tabWidget->setTabText(1, "Расписание");
  tabWidget->setTabText(2, "Журнал");
  tabWidget->setCurrentIndex(0);

  predmetComboBox = findChild<QComboBox*>("predmetComboBox");
  groupComboBox = findChild<QComboBox*>("groupComboBox");

  setupPersonalInfo();
  setupRaspisanie();
  setupZurnal();

}

TeacherWindow::~TeacherWindow()
{
  delete ui;
}

void TeacherWindow::setupRaspisanie()
{
  QSqlQueryModel *raspisanieModel = new QSqlQueryModel;
  raspisanieModel->setQuery(queries::selectTeacherRaspisanie.arg(teacherId), db);

  QTableView* raspisanieTableView = findChild<QTableView*>("raspisanieTableView");
  raspisanieTableView->setModel(raspisanieModel);
  raspisanieTableView->update();
  raspisanieTableView->resizeColumnsToContents();
}

void TeacherWindow::setupZurnal()
{
  zurnalTableView = findChild<QTableView*>("zurnalTableView");

  QSqlQuery query(db);
  query.exec(queries::selectTeacherPredmets.arg(teacherId));

  QComboBox* predmetComboBox = findChild<QComboBox*>("predmetComboBox");
  while(query.next())
  {
    predmetComboBox->addItem(query.value(0).toString());
  }
  predmetComboBox->setCurrentIndex(0);
}

QStringList* TeacherWindow::getListOfStudents()
{
  QString groupId = groupComboBox->currentText();
  QSqlQuery query(db);
  query.exec(queries::selectStudentsFromGroup.arg(groupId));

  QStringList* list = new QStringList();
  while(query.next())
  {
    list->append(query.value(0).toString().trimmed());
  }
  return list;
}

void TeacherWindow::updateModel(QSqlQueryModel *model)
{
  QString queryStr = model->query().executedQuery();
  model->clear();
  model->query().clear();
  model->setQuery(queryStr);
}

void TeacherWindow::updateZurnalTableView()
{
  zurnalTableView->update();
  zurnalTableView->resizeColumnsToContents();
  zurnalTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  zurnalTableView->setColumnHidden(3, true);
  zurnalRowIndex = -1;
}

QString TeacherWindow::getCurrentLessonId()
{
  QSqlQuery query(db);
  query.exec(queries::selectLessonId_o.arg(teacherId).arg(groupComboBox->currentText()));
  query.next();
  return query.value(0).toString();
}

QString TeacherWindow::getStudentId(const QString &fio)
{
  QSqlQuery query(db);
  query.exec(queries::selectStudentId.arg(fio));
  query.next();
  return query.value(0).toString();
}

QString TeacherWindow::getDate()
{
  QSqlQuery query(db);
  query.exec(queries::selectGetDate);
  query.next();
  return query.value(0).toString().left(10);
}

void TeacherWindow::on_predmetComboBox_currentIndexChanged(int /*index*/)
{
  QSqlQuery query(db);
  query.exec(queries::selectTeacherPredmetGroups.arg(predmetComboBox->currentText()).arg(teacherId));
  while(query.next())
  {
    groupComboBox->addItem(query.value(0).toString());
  }
}

void TeacherWindow::on_groupComboBox_currentIndexChanged(int /*index*/)
{
  QSqlQueryModel *zurnalModel = new QSqlQueryModel;
  zurnalModel->setQuery(queries::selectTeacherGroupPredmetZurnal
                        .arg(teacherId)
                        .arg(groupComboBox->currentText())
                        .arg(predmetComboBox->currentText())
                        , db);
  currentZurnalModel = zurnalModel;

  zurnalTableView->setModel(zurnalModel);
  updateZurnalTableView();

  connect(zurnalTableView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(zurnalSelectionChanged(const QModelIndex &, const QModelIndex &)));

}

void TeacherWindow::zurnalSelectionChanged(const QModelIndex &current, const QModelIndex& /*previous*/)
{
  zurnalRowIndex = current.row();
}

void TeacherWindow::on_editButton_clicked()
{
  if(zurnalRowIndex == -1)
    return;

  EditDialog *w = new EditDialog(this);

  for(int i = 0; i < zurnalTableView->model()->columnCount()-1; i++)
  {
    QModelIndex index = zurnalTableView->model()->index(zurnalRowIndex, i, QModelIndex());

    w->addLineEditItem(zurnalTableView->model()->headerData(i, Qt::Horizontal).toString(),
                       zurnalTableView->model()->data(index).toString());
  }

  w->setReadOnly(0);
  w->exec();

  if(w->accepted())
  {

    QModelIndex index = zurnalTableView->model()->index(zurnalRowIndex, 3, QModelIndex());

    QString id = zurnalTableView->model()->data(index).toString();

    QSqlQuery query(db);
    query.exec(queries::updateStudentZurnal.arg(id).arg(w->getValue(1)).arg(w->getValue(2)));

    updateModel(currentZurnalModel);
    updateZurnalTableView();

  }
  delete w;

}

void TeacherWindow::on_insertButton_clicked()
{
  EditDialog *w = new EditDialog(this);

  for(int i = 0; i < zurnalTableView->model()->columnCount()-1; i++)
  {
    w->addLineEditItem(zurnalTableView->model()->headerData(i, Qt::Horizontal).toString());
  }

  w->setValue(2, getDate());
  w->setAllowedValues(0, getListOfStudents());
  w->exec();

  if(w->accepted())
  {
    QString studentId = getStudentId(w->getValue(0));
    QString lessonId = getCurrentLessonId();
    QString ocenka = w->getValue(1);
    QString date = w->getValue(2);

    QSqlQuery query(db);
    query.exec(queries::insertZurnalEntry.arg(studentId).arg(lessonId).arg(ocenka).arg(date));

    updateModel(currentZurnalModel);
    updateZurnalTableView();

  }
  delete w;
}

void TeacherWindow::on_deleteButton_clicked()
{
  if(zurnalRowIndex == -1)
    return;

  QModelIndex index = zurnalTableView->model()->index(zurnalRowIndex, 3, QModelIndex());
  QString id = zurnalTableView->model()->data(index).toString();

  QSqlQuery query(db);
  query.exec(queries::deleteZurnalEntry.arg(id));

  updateModel(currentZurnalModel);
  updateZurnalTableView();
}
void TeacherWindow::setupPersonalInfo()
{
  QSqlQuery query(db);
  query.exec(QString("SELECT * from Teachers where id = %1").arg(teacherId));
  query.next();

  QString fio = query.value(1).toString();
  QString dateOfBirth = query.value(2).toString();

  QStringList fioList = fio.split(" ");

  findChild<QLabel*>("teacherIdLabel")->setText(QString::number(teacherId));
  findChild<QLabel*>("surnameLabel")->setText(fioList.at(0));
  findChild<QLabel*>("nameLabel")->setText(fioList.at(1));
  findChild<QLabel*>("fatherNameLabel")->setText(fioList.at(2));
  findChild<QLabel*>("birthdateLabel")->setText(dateOfBirth);
}
