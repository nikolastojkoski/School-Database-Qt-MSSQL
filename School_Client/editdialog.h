#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QStringList>
#include <QComboBox>
#include <vector>

namespace Ui {
  class EditDialog;
}

class EditDialog : public QDialog
{
  Q_OBJECT

public:
  explicit EditDialog(QWidget *parent = nullptr);
  ~EditDialog();

  void addLineEditItem(const QString &title, const QString &initialValue = "");
  void addComboBoxItem(const QString &title, QStringList *allowedValues);

  //void addItem(const ValueType &type, const QString &title, const QString &initialValue = "");

  //todo: refactor as setLineEditValue
  void setValue(int index, const QString &value);

  //void setCbCurrentIndex(int index, int valueIndex);


  void setAllowedValues(int index, QStringList *allowedValues);
  void setReadOnly(int index);

  QWidget* getItem(int index);
  QString getValue(int index);

  bool accepted();

private slots:
  void on_cancelButton_clicked();
  void on_saveButton_clicked();

private:
  Ui::EditDialog *ui;

  enum InputType{ LineEdit, ComboBox};
  std::vector<InputType> inputTypes;

  bool isEdited = false;

  QVBoxLayout *titleVLayout;
  QVBoxLayout *valueVLayout;

};

#endif // EDITDIALOG_H
