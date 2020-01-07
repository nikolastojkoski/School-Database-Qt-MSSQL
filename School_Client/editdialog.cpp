#include "editdialog.h"
#include "ui_editdialog.h"

#include <QLineEdit>
#include <QLabel>
#include <QCompleter>
#include <QComboBox>

#include <iostream>

EditDialog::EditDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EditDialog)
{
  ui->setupUi(this);
  titleVLayout = findChild<QVBoxLayout*>("titleVLayout");
  titleVLayout->setAlignment(Qt::AlignHCenter);
  valueVLayout = findChild<QVBoxLayout*>("valueVLayout");
}

EditDialog::~EditDialog()
{
  delete ui;
}

void EditDialog::addLineEditItem(const QString &title, const QString &initialValue)
{
  //QLabel *label = new QLabel(title.trimmed());
  //label->setAlignment(Qt::AlignHCenter)
  titleVLayout->addWidget(new QLabel(title.trimmed()));
  titleVLayout->setAlignment(Qt::AlignHCenter);
  valueVLayout->addWidget(new QLineEdit(initialValue.trimmed()));
  inputTypes.push_back(InputType::LineEdit);
}

void EditDialog::addComboBoxItem(const QString &title, QStringList *allowedValues)
{
  titleVLayout->addWidget(new QLabel(title.trimmed()));
  titleVLayout->setAlignment(Qt::AlignHCenter);
  QComboBox* cb = new QComboBox();

  if(allowedValues != nullptr)
    cb->addItems(*allowedValues);

  valueVLayout->addWidget(cb);
  inputTypes.push_back(InputType::ComboBox);
}

void EditDialog::setAllowedValues(int index, QStringList *allowedValues)
{
  if(inputTypes[index] == InputType::ComboBox)
  {
    QComboBox* comboBox = static_cast<QComboBox*>(valueVLayout->itemAt(index)->widget());
    comboBox->clear();
    comboBox->addItems(*allowedValues);
    return;
  }
  else if(inputTypes[index] == InputType::LineEdit)
  {
    QLineEdit* lineEdit = static_cast<QLineEdit*>(valueVLayout->itemAt(index)->widget());
    QCompleter *completer = new QCompleter(*allowedValues, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchFlag::MatchContains);
    lineEdit->setCompleter(completer);
  }
}

void EditDialog::setReadOnly(int index)
{
  QLineEdit* lineEdit = static_cast<QLineEdit*>(valueVLayout->itemAt(index)->widget());
  lineEdit->setReadOnly(true);
}

QWidget* EditDialog::getItem(int index)
{
  return valueVLayout->itemAt(index)->widget();
}
void EditDialog::setValue(int index, const QString &value)
{
  if(inputTypes[index] == InputType::ComboBox)
  {
    QComboBox* comboBox = static_cast<QComboBox*>(valueVLayout->itemAt(index)->widget());
    comboBox->setCurrentText(value);
  }
  else if(inputTypes[index] == InputType::LineEdit)
  {
    QLineEdit* lineEdit = static_cast<QLineEdit*>(valueVLayout->itemAt(index)->widget());
    lineEdit->setText(value);
  }
}

QString EditDialog::getValue(int index)
{
  if(inputTypes[index] == InputType::LineEdit)
    return static_cast<QLineEdit*>(valueVLayout->itemAt(index)->widget())->text();
  else if(inputTypes[index] == InputType::ComboBox)
    return static_cast<QComboBox*>(valueVLayout->itemAt(index)->widget())->currentText();
}

bool EditDialog::accepted()
{
  return isEdited;
}

void EditDialog::on_cancelButton_clicked()
{
  reject();
}

void EditDialog::on_saveButton_clicked()
{
  isEdited = true;
  accept();
}
