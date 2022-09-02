// by stevenso aka v3ssel
#ifndef CREDIT_H
#define CREDIT_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
class credit;
}

class credit : public QDialog {
  Q_OBJECT

 public:
  explicit credit(QWidget *parent = nullptr);
  ~credit();

 private slots:
  void on_pushButton_back_clicked();
  void on_pushButton_choose_type_clicked();
  void on_pushButton_months_clicked();
  void on_pushButton_calculate_monthly_clicked();
  void on_pushButton_calculate_credit_clicked();
  void on_pushButton_calculate_loan_clicked();
  void itemClicked(QListWidgetItem *);
  void itemClickedTwo(QListWidgetItem *);
  void hideall();
  void show_monthly();
  void show_credit();
  void show_loan();

 private:
  Ui::credit *ui;
};

#endif  // CREDIT_H
