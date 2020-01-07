#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <iostream>


LoginWindow::LoginWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::LoginWindow)
{
  ui->setupUi(this);
  db = QSqlDatabase::addDatabase("QODBC3");
}

LoginWindow::~LoginWindow()
{
  delete ui;
}

void LoginWindow::on_loginButton_clicked()
{

  QString username = findChild<QLineEdit*>("usernameEdit")->text();
  QString password = findChild<QLineEdit*>("passwordEdit")->text();


  db.setDatabaseName(QString("DRIVER={SQL Server Native Client 11.0};"
                     "SERVER=DESKTOP-98OKAVH\\MSSQLSERVER01;DATABASE=School2;"
                     "UID=%1;PWD=%2;WSID=.;Trusted_connection=yes")
                      .arg(username).arg(password));
  db.open();

  QSqlQuery query(db);
  query.exec(queries::selectUserInfo.arg(username));

  if(!query.first())
  {
    QMessageBox messageBox;
    messageBox.setWindowTitle("Error");
    messageBox.setText("<p align='center'>Invalid username or password!</p>");
    messageBox.setStyleSheet("QLabel{min-width: 200px;}");
    messageBox.exec();
    return;
  }

  int userType = query.value(0).toInt();
  int userId = query.value(1).toInt();

  switch(userType)
  {
    case 1:
      studentWindow = new StudentWindow(userId, db);
      studentWindow->show();
      break;
    case 2:
      teacherWindow = new TeacherWindow(userId, db);
      teacherWindow->show();
      break;
    case 3:
      principalWindow = new PrincipalWindow(db);
      principalWindow->show();
      break;
  }

  hide();

}





