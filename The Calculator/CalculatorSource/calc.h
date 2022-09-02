// by stevenso aka v3ssel
#ifndef CALC_H
#define CALC_H

#include <QMainWindow>
#include <QMessageBox>
#include <QValidator>
#include <QVector>
#include <QtWidgets>

#include "Credit/credit.h"
#include "Deposit/deposit.h"
#include "Graphics/qcustomplot.h"

extern "C" {
#include "CalculatorSource/stack.h"
}

QT_BEGIN_NAMESPACE
namespace Ui {
class calc;
}
QT_END_NAMESPACE

class calc : public QMainWindow {
  Q_OBJECT

 public:
  explicit calc(QWidget *parent = nullptr);
  ~calc();

 private slots:
  void on_pushButton_0_clicked();
  void on_pushButton_1_clicked();
  void on_pushButton_2_clicked();
  void on_pushButton_3_clicked();
  void on_pushButton_4_clicked();
  void on_pushButton_5_clicked();
  void on_pushButton_6_clicked();
  void on_pushButton_7_clicked();
  void on_pushButton_8_clicked();
  void on_pushButton_9_clicked();
  void on_pushButton_x_clicked();
  void on_pushButton_dot_clicked();
  void on_pushButton_lBracket_clicked();
  void on_pushButton_rBracket_clicked();
  void on_pushButton_allclear_clicked();
  void on_pushButton_clearAnswer_clicked();
  void on_pushButton_clearInput_clicked();
  void on_pushButton_sin_clicked();
  void on_pushButton_cos_clicked();
  void on_pushButton_tan_clicked();
  void on_pushButton_asin_clicked();
  void on_pushButton_acos_clicked();
  void on_pushButton_atan_clicked();
  void on_pushButton_ln_clicked();
  void on_pushButton_log_clicked();
  void on_pushButton_sqrt_clicked();
  void on_pushButton_power_clicked();
  void on_pushButton_plus_clicked();
  void on_pushButton_minus_clicked();
  void on_pushButton_mult_clicked();
  void on_pushButton_div_clicked();
  void on_pushButton_mod_clicked();
  void on_pushButton_help_clicked();
  void on_pushButton_more_clicked();
  void on_pushButton_less_clicked();
  void on_pushButton_credit_clicked();
  void on_pushButton_deposit_clicked();
  void on_pushButton_start_clicked();
  void on_pushButton_upGraph_clicked();
  void on_pushButton_def_clicked();

 private:
  Ui::calc *ui;
  credit *cred;
  deposit *dep;
  QVector<double> x, y;
};

class MyValidator : public QValidator {
 public:
  explicit MyValidator(QObject *parent = nullptr) : QValidator(parent) {}
  State validate(QString &input, int &) const override {
    input = input.toLower();
    return QValidator::Acceptable;
  }
};

#endif  // CALC_H
