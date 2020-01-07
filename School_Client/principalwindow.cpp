#include "principalwindow.h"
#include "ui_principalwindow.h"
#include <QSqlQuery>
#include <QComboBox>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include "queries.h"
#include "editdialog.h"
#include <iostream>

PrincipalWindow::PrincipalWindow(QSqlDatabase &database, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::PrincipalWindow)
{
  ui->setupUi(this);
  db = database;

  QTabWidget* tabWidget = findChild<QTabWidget*>("tabWidget");
  tabWidget->setTabText(0, "Студенты");
  tabWidget->setTabText(1, "Группы");
  tabWidget->setTabText(2, "Учители");
  tabWidget->setTabText(3, "Учебный план");
  tabWidget->setTabText(4, "Занятия");
  tabWidget->setTabText(5, "Расписание");
  tabWidget->setTabText(6, "Дополнительно");
  tabWidget->setCurrentIndex(0);

  setupTable("studentTableView", queries::selectStudents, "student", 4);
  setupTable("groupTableView", queries::selectGroups, "group");
  setupTable("teacherTableView", queries::selectTeachers, "teacher", 3);
  setupTable("uchebniPlanTableView", queries::selectUchebniPlan, "uchebniPlan", 4);
  setupTable("lessonsTableView", queries::selectLessons, "lessons", 3);
  setupTable("raspisanieTableView", queries::selectRaspisanie, "raspisanie", 7);
  setupSortComboBox("sortStudentComboBox", "student", 4);
  setupSortComboBox("sortGroupComboBox", "group", 4);
  setupSortComboBox("sortTeacherComboBox", "teacher", 3);
  setupSortComboBox("sortPredmetComboBox", "uchebniPlan", 4);
  setupSortComboBox("sortLessonComboBox", "lessons", 3);
  setupSortComboBox("sortRaspisanieComboBox", "raspisanie", 7);
  setupAdditionalComboBox();
}

PrincipalWindow::~PrincipalWindow()
{
  delete ui;
}

void PrincipalWindow::setupTable(const QString &tableViewName, const QString &query,
                                 const QString &accessibleName, int columnToHide)
{
  QSqlQueryModel *model = new QSqlQueryModel;
  model->setQuery(query, db);
  models[accessibleName] = model;

  QTableView* tableView = findChild<QTableView*>(tableViewName);
  tableView->setModel(model);
  tableView->update();
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

  if(columnToHide != -1)
    tableView->setColumnHidden(columnToHide, true);

  tableView->resizeColumnsToContents();
  tableViews[accessibleName] = tableView;
}

void PrincipalWindow::setupSortComboBox(const QString &comboBoxName, const QString &tableAccessibleName, int numTableColumns)
{
  QComboBox* sortComboBox = findChild<QComboBox*>(comboBoxName);
  sortComboBoxes[tableAccessibleName] = sortComboBox;
  sortComboBox->addItem(" - ");
  for(int i = 0; i < numTableColumns; i++)
    sortComboBox->addItem(getHeaderValue(tableAccessibleName, i));
}

void PrincipalWindow::setupAdditionalComboBox()
{
  QComboBox* additionalComboBox = findChild<QComboBox*>("additionalComboBox");
  for(int i = 0; i < queries::numAdditionalQueries; i++)
    additionalComboBox->addItem(queries::selectAdditionalTitles[i]);
}

void PrincipalWindow::setTableSortMode(const QString &tableAccessibleName, int column)
{
  if(column == 0)
    return;

  QSortFilterProxyModel *sortModel;

  if(tableAccessibleName == "raspisanie" && column == 1)
  {
    class MyProxyModel: public QSortFilterProxyModel
    {
    public:
      explicit MyProxyModel(QObject *parent = Q_NULLPTR): QSortFilterProxyModel(parent){ }
    protected:
      bool   lessThan ( const QModelIndex & left, const QModelIndex & right ) const
      {
        QVariant leftData = sourceModel()->data(left);
        QVariant rightData = sourceModel()->data(right);
        static const QStringList order({"Понедельник","Вторник","Среда","Четверг","Пятница"});
        return order.indexOf(leftData.toString().trimmed()) < order.indexOf(rightData.toString().trimmed());
      }
    };

    sortModel = new MyProxyModel();
  }
  else
  {
    sortModel = new QSortFilterProxyModel(this);
  }

  sortModel->setDynamicSortFilter(true);
  sortModel->setSourceModel(models[tableAccessibleName]);
  tableViews[tableAccessibleName]->setModel(sortModel);
  tableViews[tableAccessibleName]->setSortingEnabled(true);
  tableViews[tableAccessibleName]->sortByColumn(column-1, Qt::AscendingOrder);
}

void PrincipalWindow::on_insertRaspisanieButton_clicked()
{
  EditDialog *w = new EditDialog(this);
  w->addComboBoxItem(getHeaderValue("raspisanie", 0), getDaysOfWeekList());
  for(int i = 1; i < 4; i++)
  {
    w->addLineEditItem(getHeaderValue("raspisanie", i));
  }
  w->addComboBoxItem(getHeaderValue("raspisanie", 4), getPredmetList());
  w->addComboBoxItem(getHeaderValue("raspisanie", 5), nullptr); //teacher
  w->addComboBoxItem(getHeaderValue("raspisanie", 6), nullptr); //group

  connect(static_cast<QComboBox*>(w->getItem(4)), &QComboBox::currentTextChanged, [&](const QString &value)
  {
    QString predmetId = getPredmetId(value);
    w->setAllowedValues(5, getTeacherListForPredmet(predmetId));
  });
  connect(static_cast<QComboBox*>(w->getItem(5)), &QComboBox::currentTextChanged, [&](const QString &value)
  {
    QString predmetId = getPredmetId(w->getValue(4));
    QString teacherId = getTeacherId(value);
    w->setAllowedValues(6, getGroupListForTeacherPredmet(teacherId, predmetId));
  });

  w->exec();
  if(w->accepted())
  {
    QString day = getDayIdx(w->getValue(0));
    QString timeFrom = w->getValue(1);
    QString timeTo = w->getValue(2);
    QString classroom = w->getValue(3);
    QString lessonId = getLessonId(getPredmetId(w->getValue(4)), getTeacherId(w->getValue(5)), w->getValue(6));

    QSqlQuery query(db);
    query.exec(queries::insertRaspisanie.arg(day).arg(timeFrom).arg(timeTo).arg(classroom).arg(lessonId));

    updateTable("raspisanie", 7);
  }
}

void PrincipalWindow::on_deleteRaspisanieButton_clicked()
{
  if(!isAnyRowSelected("raspisanie"))
    return;

  QString id = getCellValue("raspisanie", getSelectedRow("raspisanie"), 7);
  QSqlQuery query(db);
  query.exec(queries::deleteRaspisanie.arg(id));
  updateTable("raspisanie");
}

void PrincipalWindow::on_editRaspisanieButton_clicked()
{
  if(!isAnyRowSelected("raspisanie"))
    return;
  EditDialog *w = new EditDialog(this);
  w->addComboBoxItem(getHeaderValue("raspisanie", 0), getDaysOfWeekList());
  for(int i = 1; i < 4; i++)
  {
    w->addLineEditItem(getHeaderValue("raspisanie", i));
  }
  w->addComboBoxItem(getHeaderValue("raspisanie", 4), getPredmetList());
  w->addComboBoxItem(getHeaderValue("raspisanie", 5), nullptr); //teacher
  w->addComboBoxItem(getHeaderValue("raspisanie", 6), nullptr); //group

  connect(static_cast<QComboBox*>(w->getItem(4)), &QComboBox::currentTextChanged, [&](const QString &value)
  {
    QString predmetId = getPredmetId(value);
    w->setAllowedValues(5, getTeacherListForPredmet(predmetId));
  });
  connect(static_cast<QComboBox*>(w->getItem(5)), &QComboBox::currentTextChanged, [&](const QString &value)
  {
    QString predmetId = getPredmetId(w->getValue(4));
    QString teacherId = getTeacherId(value);
    w->setAllowedValues(6, getGroupListForTeacherPredmet(teacherId, predmetId));
  });

  for(int i = 0; i < 7; i++)
    w->setValue(i, getCellValue("raspisanie", getSelectedRow("raspisanie"), i));

  w->exec();
  if(w->accepted())
  {
    QString day = getDayIdx(w->getValue(0));
    QString timeFrom = w->getValue(1);
    QString timeTo = w->getValue(2);
    QString classroom = w->getValue(3);
    QString lessonId = getLessonId(getPredmetId(w->getValue(4)), getTeacherId(w->getValue(5)), w->getValue(6));
    QString id = getCellValue("raspisanie", getSelectedRow("raspisanie"), 7);
    QSqlQuery query(db);

    std::cout << queries::updateRaspisnie.arg(day).arg(timeFrom).arg(timeTo).arg(classroom).arg(lessonId).arg(id).toStdString() << std::endl;
    query.exec(queries::updateRaspisnie.arg(day).arg(timeFrom).arg(timeTo).arg(classroom).arg(lessonId).arg(id));
    updateTable("raspisanie", 7);
  }
}

void PrincipalWindow::on_insertLessonButton_clicked()
{
  EditDialog *w = new EditDialog(this);

  w->addComboBoxItem(getHeaderValue("lessons", 0), getPredmetList());
  w->addComboBoxItem(getHeaderValue("lessons", 1), getTeacherList());
  w->addComboBoxItem(getHeaderValue("lessons", 2), getGroupList());
  w->exec();
  if(w->accepted())
  {
    QSqlQuery query(db);
    QString predmetId = getPredmetId(w->getValue(0));
    QString teacherId = getTeacherId(w->getValue(1));
    query.exec(queries::insertLesson.arg(predmetId).arg(teacherId).arg(w->getValue(2)));
  }
  updateTable("lessons", 3);
}

void PrincipalWindow::on_deleteLessonButton_clicked()
{
  if(!isAnyRowSelected("lessons"))
    return;

  QString lessonId = getCellValue("lessons", getSelectedRow("lessons"), 3);
  QSqlQuery query(db);
  for(int i = 0; i < queries::deleteLessonsNumQueries; i++)
  {
    query.exec(queries::deleteLesson[i].arg(lessonId));
  }
  updateTable("lessons", 3);
}

void PrincipalWindow::on_editLessonButton_clicked()
{
  if(!isAnyRowSelected("lessons"))
    return;

  EditDialog *w = new EditDialog(this);
  w->addComboBoxItem(getHeaderValue("lessons", 0), getPredmetList());
  w->addComboBoxItem(getHeaderValue("lessons", 1), getTeacherList());
  w->addComboBoxItem(getHeaderValue("lessons", 2), getGroupList());
  w->setValue(0, getCellValue("lessons", getSelectedRow("lessons"), 0));
  w->setValue(1, getCellValue("lessons", getSelectedRow("lessons"), 1));
  w->setValue(2, getCellValue("lessons", getSelectedRow("lessons"), 2));
  w->exec();

  if(w->accepted())
  {
    QSqlQuery query(db);
    QString predmetId = getPredmetId(w->getValue(0));
    QString teacherId = getTeacherId(w->getValue(1));
    QString lessonId = getCellValue("lessons", getSelectedRow("lessons"), 3);

    std::cout << "edit lesson query: " << std::endl;
    std::cout << queries::updateLesson.arg(predmetId).arg(teacherId).arg(w->getValue(2)).arg(lessonId).toStdString() << std::endl;

    query.exec(queries::updateLesson.arg(predmetId).arg(teacherId).arg(w->getValue(2)).arg(lessonId));
  }
  updateTable("lessons", 3);
}

void PrincipalWindow::on_insertPredmetButton_clicked()
{
  EditDialog *w = new EditDialog(this);
  for(int i = 0; i < 4; i++)
  {
    w->addLineEditItem(getHeaderValue("uchebniPlan", i));
  }
  w->exec();
  if(w->accepted())
  {
    QSqlQuery query(db);
    query.exec(queries::insertPredmet.arg(w->getValue(0)).arg(w->getValue(1))
               .arg(w->getValue(2)).arg(w->getValue(3)));
  }
  updateTable("uchebniPlan", 4);
}

void PrincipalWindow::on_deletePredmetButton_clicked()
{
  if(!isAnyRowSelected("uchebniPlan"))
    return;
  QString predmetId = getCellValue("uchebniPlan", getSelectedRow("uchebniPlan"), 4);

  for(int i = 0; i < queries::deletePredmetNumQueries; i++)
  {
    QSqlQuery query(db);
    query.exec(queries::deletePredmet[i].arg(predmetId));
  }
  updateTable("uchebniPlan", 4);
}

void PrincipalWindow::on_editPredmetButton_clicked()
{
  if(!isAnyRowSelected("uchebniPlan"))
    return;

  EditDialog *w = new EditDialog(this);
  for(int i = 0; i < 4; i++)
  {
    w->addLineEditItem(getHeaderValue("uchebniPlan", i),
                       getCellValue("uchebniPlan", getSelectedRow("uchebniPlan"), i));
  }
  w->exec();
  if(w->accepted())
  {
    QString predmetId = getCellValue("uchebniPlan", getSelectedRow("uchebniPlan"), 4);
    QSqlQuery query(db);
    query.exec(queries::updatePredmet.arg(w->getValue(0)).arg(w->getValue(1))
               .arg(w->getValue(2)).arg(w->getValue(3)).arg(predmetId));
  }
  updateTable("uchebniPlan", 4);
}


void PrincipalWindow::on_insertTeacherButton_clicked()
{
  EditDialog *w = new EditDialog(this);
  w->addLineEditItem("ФИО");
  w->addLineEditItem(getHeaderValue("teacher", 1));
  w->addLineEditItem(getHeaderValue("teacher", 2), getDate());
  w->exec();
  if(w->accepted())
  {
    QSqlQuery query(db);
    query.exec(queries::insertTeacher.arg(w->getValue(0)).arg(w->getValue(1)).arg(w->getValue(2)));
    updateTable("teacher", 3);
  }
}

void PrincipalWindow::on_deleteTeacherButton_clicked()
{
  if(!isAnyRowSelected("teacher"))
    return;

  QString teacherId = getCellValue("teacher", getSelectedRow("teacher"), 3);
  QSqlQuery query(db);
  for(int i = 0; i < queries::deleteTeacherNumQueries; i++)
  {
    std::cout << queries::deleteTeacher[i].arg(teacherId).toStdString() << std::endl;
    db.exec(queries::deleteTeacher[i].arg(teacherId));
  }
  updateTable("teacher", 3);
}

void PrincipalWindow::on_editTeacherButton_clicked()
{
  if(!isAnyRowSelected("teacher"))
    return;

  EditDialog *w = new EditDialog(this);
  w->addLineEditItem("ФИО", getCellValue("teacher", getSelectedRow("teacher"), 0));
  w->addLineEditItem(getHeaderValue("teacher", 1), getCellValue("teacher", getSelectedRow("teacher"), 1));
  w->addLineEditItem(getHeaderValue("teacher", 2), getCellValue("teacher", getSelectedRow("teacher"), 2));
  w->exec();
  if(w->accepted())
  {
    QString teacherId = getCellValue("teacher", getSelectedRow("teacher"), 3);
    QSqlQuery query(db);
    query.exec(queries::updateTeacher.arg(w->getValue(0)).arg(w->getValue(1))
               .arg(w->getValue(2)).arg(teacherId));
  }
  updateTable("teacher", 3);

}

void PrincipalWindow::on_insertGroupButton_clicked()
{
  EditDialog *w = new EditDialog(this);
  w->addLineEditItem(getHeaderValue("group", 1));
  w->addComboBoxItem(getHeaderValue("group", 3), getTeacherList());
  w->exec();
  if(w->accepted())
  {
    QSqlQuery query(db);
    query.exec(queries::insertGroup.arg(w->getValue(0)).arg(getTeacherId(w->getValue(1))));
    updateTable("group");
  }
}

void PrincipalWindow::on_editGroupButton_clicked()
{
  if(!isAnyRowSelected("group"))
    return;

  EditDialog *w = new EditDialog(this);

  for(int i = 0; i < 3; i++)
    w->addLineEditItem(getHeaderValue("group", i),
                       getCellValue("group", getSelectedRow("group"), i));

  w->addComboBoxItem(getHeaderValue("group", 3), getTeacherList());
  w->setValue(3, getCellValue("group", getSelectedRow("group"), 3));
  w->exec();
  if(w->accepted())
  {
    QSqlQuery query(db);

    std::cout << "update group query: " << std::endl;
    std::cout << queries::updateGroup.arg(w->getValue(1))
                 .arg(getTeacherId(w->getValue(3).trimmed())).arg(w->getValue(0)).toStdString() << std::endl;

    query.exec(queries::updateGroup.arg(w->getValue(1))
               .arg(getTeacherId(w->getValue(3).trimmed())).arg(w->getValue(0)));
    updateTable("group");
  }
}

void PrincipalWindow::on_deleteGroupButton_clicked()
{
  if(!isAnyRowSelected("group"))
    return;

  int groupId = getCellValue("group", getSelectedRow("group"), 0).toInt();
  int n = queries::deleteGroupNumQueries;
  QSqlQuery query(db);
  for(int i = 0; i < n; i++)
  {
    query.exec(queries::deleteGroup[i].arg(groupId));
  }
  updateTable("group");
}

void PrincipalWindow::on_insertStudentButton_clicked()
{
  EditDialog* w = new EditDialog(this);

  for(int i = 0; i < 3; i++)
    w->addLineEditItem(getHeaderValue("student", i));
  w->addComboBoxItem(getHeaderValue("student", 3), nullptr);

  connect(static_cast<QLineEdit*>(w->getItem(2)), &QLineEdit::textChanged, [&](const QString &value)
  {
    std::cout << "setting allowed values for groupId" << std::endl;
    w->setAllowedValues(3, getGroupList(value));
  });

  w->exec();

  if(w->accepted())
  {
    QSqlQuery query(db);
    query.exec(queries::insertStudent.arg(w->getValue(0)).arg(w->getValue(1))
               .arg(w->getValue(2)).arg(w->getValue(3)));

    updateTable("student", 4);
  }

  delete w;
}

void PrincipalWindow::on_deleteStudentButton_clicked()
{
  if(!isAnyRowSelected("student"))
    return;

  QString studentId = getCellValue("student", getSelectedRow("student"), 4);

  QSqlQuery query(db);
  query.exec(QString("delete from Zurnal where StudentId = %1").arg(studentId));
  query.exec(QString("delete from Students where id = %1").arg(studentId));

  updateTable("student", 4);
}

void PrincipalWindow::on_editStudentButton_clicked()
{
  if(!isAnyRowSelected("student"))
    return;

  EditDialog* w = new EditDialog(this);

  for(int i = 0; i < 3; i++)
    w->addLineEditItem(getHeaderValue("student", i),
                       getCellValue("student", getSelectedRow("student"), i));
  w->addComboBoxItem(getHeaderValue("student", 3), nullptr);

  w->setAllowedValues(3, getGroupList(w->getValue(2)));
  w->setValue(3, getCellValue("student", getSelectedRow("student"), 3));

  connect(static_cast<QLineEdit*>(w->getItem(2)), &QLineEdit::textChanged, [&](const QString &value)
  {
    w->setAllowedValues(3, getGroupList(value));
  });

  w->exec();

  if(w->accepted())
  {
    QString studentId = getCellValue("student", getSelectedRow("student"), 4);

    QSqlQuery query(db);
    query.exec(queries::updateStudent.arg(w->getValue(0)).arg(w->getValue(1))
               .arg(w->getValue(2)).arg(w->getValue(3)).arg(studentId));
  }
  updateTable("student", 4);
  delete w;
}

void PrincipalWindow::updateModel(QSqlQueryModel *model)
{
  QString queryStr = model->query().executedQuery();
  model->clear();
  model->query().clear();
  model->setQuery(queryStr, db);
}

void PrincipalWindow::updateTableView(QTableView* tableView, int columnToHide)
{
  tableView->update();
  tableView->resizeColumnsToContents();
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  if(columnToHide != -1)
    tableView->setColumnHidden(columnToHide, true);
}
void PrincipalWindow::updateTable(const QString &tableAccessibleName, int columnToHide)
{
  updateModel(models[tableAccessibleName]);
  updateTableView(tableViews[tableAccessibleName], columnToHide);
}
bool PrincipalWindow::isAnyRowSelected(const QString &tableAccesibleName)
{
  QModelIndexList selectedIndex = tableViews[tableAccesibleName]->selectionModel()->selectedRows();
  if(selectedIndex.empty())
    return false;
  return true;
}
int PrincipalWindow::getSelectedRow(const QString &tableAccessibleName)
{
  return tableViews[tableAccessibleName]->selectionModel()->selectedRows().first().row();
}
QString PrincipalWindow::getCellValue(const QString &tableAccessibleName, int row, int col)
{
  QModelIndex index = tableViews[tableAccessibleName]->model()->index(row, col, QModelIndex());
  return tableViews[tableAccessibleName]->model()->data(index).toString();
}
QString PrincipalWindow::getHeaderValue(const QString &tableAccessibleName, int column)
{
  return tableViews[tableAccessibleName]->model()->headerData(column, Qt::Horizontal).toString();
}

QString PrincipalWindow::getDate()
{
  QSqlQuery query(db);
  query.exec(queries::selectGetDate);
  query.next();
  return query.value(0).toString().left(10);
}

QString PrincipalWindow::getTeacherFIO(const QString &teacherId)
{
  QSqlQuery query(db);
  query.exec(queries::selectTeacherFIO.arg(teacherId));
  query.next();
  return query.value(0).toString();
}

QString PrincipalWindow::getTeacherId(const QString &teacherFIO)
{
  QSqlQuery query(db);
  query.exec(QString("select id from Teachers where FIO = '%1'").arg(teacherFIO));
  query.next();
  return query.value(0).toString();
}
QString PrincipalWindow::getPredmetId(const QString &predmet)
{
  QSqlQuery query(db);
  query.exec(queries::selectPredmetId.arg(predmet));
  query.next();
  return query.value(0).toString();
}

QString PrincipalWindow::getLessonId(const QString &predmetId, const QString &teacherId, const QString &groupId)
{
  QSqlQuery query(db);
  query.exec(queries::selectLessonId.arg(predmetId).arg(teacherId).arg(groupId));
  query.next();
  return query.value(0).toString();
}
QStringList *PrincipalWindow::getPredmetList()
{
  QSqlQuery query(db);
  query.exec(queries::selectPredmets);

  QStringList* list = new QStringList;
  while(query.next())
  {
    list->append(query.value(0).toString());
  }
  return list;
}

QStringList *PrincipalWindow::getDaysOfWeekList()
{
  return new QStringList({"Понедельник","Вторник","Среда","Четверг","Пятница"});
}

QString PrincipalWindow::getDayIdx(QString dayName)
{
  return QString::number(getDaysOfWeekList()->indexOf(dayName) + 1);
}
QStringList *PrincipalWindow::getTeacherList()
{
  QSqlQuery query(db);
  query.exec("select FIO from Teachers");

  QStringList* list = new QStringList;
  while(query.next())
  {
    list->append(query.value(0).toString());
  }
  return list;
}
QStringList *PrincipalWindow::getGroupList(const QString &class_num)
{
  QSqlQuery query(db);
  query.exec(queries::selectGroupIds.arg(class_num));

  QStringList* list = new QStringList();
  while(query.next())
  {
    list->append(query.value(0).toString());
  }
  return list;
}
QStringList *PrincipalWindow::getGroupListForPredmet(const QString &predmetId)
{
  QSqlQuery query(db);
  query.exec(queries::selectGroupsWhichHavePredmet.arg(predmetId));
  QStringList* list = new QStringList();
  while(query.next())
  {
    list->append(query.value(0).toString());
  }
  return list;
}

QStringList *PrincipalWindow::getGroupListForTeacherPredmet(const QString &teacherId, const QString &predmetId)
{
  QSqlQuery query(db);
  query.exec(queries::selectGroupsWhichHaveTeacherPredmet.arg(teacherId).arg(predmetId));
  QStringList* list = new QStringList();
  while(query.next())
  {
    list->append(query.value(0).toString());
  }
  return list;
}
QStringList *PrincipalWindow::getTeacherListForPredmet(const QString &predmetId)
{
  QSqlQuery query(db);
  query.exec(queries::selectTeachersWhichHavePredmet.arg(predmetId));
  QStringList* list = new QStringList();
  while(query.next())
  {
    list->append(query.value(0).toString());
  }
  return list;
}
QStringList *PrincipalWindow::getGroupList()
{
  QSqlQuery query(db);
  query.exec(queries::selectGroupsIds);
  QStringList* list = new QStringList();
  while(query.next())
  {
    list->append(query.value(0).toString());
  }
  return list;
}

void PrincipalWindow::on_tabWidget_currentChanged(int index)
{
  static bool firstTime = true;
  if(firstTime)
  {
    firstTime = false;
    return;
  }
  switch(index)
  {
    case 0:
      updateTable("student", 4);
      break;
    case 1:
      updateTable("group");
      break;
    case 2:
      updateTable("teacher", 3);
      break;
    case 3:
      updateTable("uchebniPlan", 4);
      break;
    case 4:
      updateTable("lessons", 3);
      break;
    case 5:
      updateTable("raspisanie", 7);
      break;
    default:
      updateTable("additional");
      break;
  }
}

void PrincipalWindow::on_sortStudentComboBox_currentIndexChanged(int index)
{
  setTableSortMode("student", index);
}

void PrincipalWindow::on_sortGroupComboBox_currentIndexChanged(int index)
{
  setTableSortMode("group", index);
}

void PrincipalWindow::on_sortTeacherComboBox_currentIndexChanged(int index)
{
  setTableSortMode("teacher", index);
}

void PrincipalWindow::on_sortPredmetComboBox_currentIndexChanged(int index)
{
  setTableSortMode("uchebniPlan", index);
}

void PrincipalWindow::on_sortLessonComboBox_currentIndexChanged(int index)
{
  setTableSortMode("lessons", index);
}

void PrincipalWindow::on_sortRaspisanieComboBox_currentIndexChanged(int index)
{
  setTableSortMode("raspisanie", index);
}

void PrincipalWindow::on_additionalComboBox_currentIndexChanged(int index)
{
  setupTable("additionalTableView", queries::selectAdditional[index], "additional");
}
