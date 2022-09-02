// by stevenso aka v3ssel
#include "../CalculatorSource/calc.h"
#include "ui_credit.h"

credit::credit(QWidget* parent) : QDialog(parent), ui(new Ui::credit) {
  ui->setupUi(this);
  ui->listWidget_type->addItem("Monthly payment");
  ui->listWidget_type->addItem("Credit term");
  ui->listWidget_type->addItem("Maximum loan amount");
  ui->listWidget_months_years->addItem("Months");
  ui->listWidget_months_years->addItem("Years");
  ui->lineEdit_amount->setValidator(new QDoubleValidator(0, 1e10, 3, this));
  ui->lineEdit_credit->setValidator(new QDoubleValidator(0, 1e10, 3, this));
  ui->lineEdit_rate->setValidator(new QDoubleValidator(1, 1e10, 3, this));
  ui->lineEdit_monthly_pay->setValidator(
      new QDoubleValidator(0, 1e10, 3, this));
  ui->lineEdit_monthly_pay_loan->setValidator(
      new QDoubleValidator(0, 1e10, 3, this));
  hideall();
}

credit::~credit() { delete ui; }

void credit::hideall() {
  ui->listWidget_payment_output->hide();
  ui->pushButton_months->setText("Months ↓");
  ui->radioButton_annuity->setChecked(true);
  ui->listWidget_months_years->hide();
  ui->listWidget_type->hide();
  ui->pushButton_months->hide();
  ui->typeOfMonthly->hide();
  ui->amount->hide();
  ui->interest_rate->hide();
  ui->credit_term->hide();
  ui->radioButton_annuity->hide();
  ui->radioButton_differ->hide();
  ui->lineEdit_amount->hide();
  ui->lineEdit_credit->hide();
  ui->lineEdit_rate->hide();
  ui->rub->hide();
  ui->percent->hide();
  ui->pushButton_calculate_monthly->hide();
  ui->pushButton_calculate_credit->hide();
  ui->pushButton_calculate_loan->hide();
  ui->rub_2->hide();
  ui->credit_monthly_pay->hide();
  ui->lineEdit_monthly_pay->hide();
  ui->credit_monthly_pay_loan->hide();
  ui->lineEdit_monthly_pay_loan->hide();
  ui->line->hide();
  ui->text_monthlyOut->hide();
  ui->text_total_over->hide();
  ui->text_total_payout->hide();
  ui->lineEdit_overpay->hide();
  ui->lineEdit_payout->hide();
  ui->text_total_pay_term->hide();
  ui->text_total_over_term->hide();
  ui->text_total_term->hide();
  ui->lineEdit_overpay_term->hide();
  ui->lineEdit_fullsum_term->hide();
  ui->lineEdit_term->hide();
  ui->text_total_credit->hide();
  ui->line->hide();
  ui->line_2->hide();
}

void credit::on_pushButton_back_clicked() { this->close(); }

void credit::on_pushButton_choose_type_clicked() {
  ui->pushButton_choose_type->hide();
  ui->listWidget_type->show();
  connect(ui->listWidget_type, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
          this, SLOT(itemClicked(QListWidgetItem*)));
}

void credit::itemClicked(QListWidgetItem* listWidgetItem) {
  ui->pushButton_choose_type->setText(listWidgetItem->text() + " ↓");
  ui->listWidget_type->hide();
  ui->pushButton_choose_type->show();
  if (!QString::compare("Monthly payment",
                        ui->listWidget_type->currentItem()->text())) {
    show_monthly();
  } else if (!QString::compare("Credit term",
                               ui->listWidget_type->currentItem()->text())) {
    show_credit();
  } else {
    show_loan();
  }
}

void credit::show_monthly() {
  hideall();
  ui->pushButton_months->show();
  ui->typeOfMonthly->show();
  ui->amount->show();
  ui->interest_rate->show();
  ui->credit_term->show();
  ui->radioButton_annuity->show();
  ui->radioButton_differ->show();
  ui->lineEdit_amount->show();
  ui->lineEdit_credit->show();
  ui->lineEdit_rate->show();
  ui->rub->show();
  ui->percent->show();
  ui->pushButton_calculate_monthly->show();
  ui->line->show();
  ui->lineEdit_amount->setText("");
  ui->lineEdit_credit->setText("");
  ui->lineEdit_rate->setText("");
  setFixedSize(415, 600);
}

void credit::on_pushButton_calculate_monthly_clicked() {
  ui->listWidget_payment_output->hide();
  ui->listWidget_payment_output->clear();
  if (QString::compare(ui->lineEdit_amount->text(), "") &&
      QString::compare(ui->lineEdit_credit->text(), "") &&
      QString::compare(ui->lineEdit_rate->text(), "")) {
    long double sum = ui->lineEdit_amount->text().toDouble();
    int64_t term = ui->lineEdit_credit->text().toLongLong();
    if (ui->pushButton_months->text().at(0) == 'Y') term *= 12;
    double rate = ui->lineEdit_rate->text().toDouble();
    bool type = true;
    if (ui->radioButton_differ->isChecked()) {
      type = false;
    }
    double payment[term], overpay[term], fullSum[term];
    calc_monthly(sum, term, rate, type, payment, overpay, fullSum);
    for (size_t i = 0; i < (size_t)term; i++) {
      ui->listWidget_payment_output->addItem(QString::number(i + 1) + ". " +
                                             QString::number(payment[i]));
    }
    ui->lineEdit_overpay->setText(QString::number(overpay[term - 1], 'f', 2));
    ui->lineEdit_payout->setText(QString::number(fullSum[term - 1], 'f', 2));
    ui->text_total_over->show();
    ui->text_total_payout->show();
    ui->lineEdit_overpay->show();
    ui->lineEdit_payout->show();
    ui->text_monthlyOut->show();
    ui->listWidget_payment_output->show();
  }
}

void credit::on_pushButton_months_clicked() {
  ui->pushButton_months->hide();
  ui->listWidget_months_years->show();
  connect(ui->listWidget_months_years,
          SIGNAL(itemDoubleClicked(QListWidgetItem*)), this,
          SLOT(itemClickedTwo(QListWidgetItem*)));
}

void credit::itemClickedTwo(QListWidgetItem* listWidgetItem) {
  ui->pushButton_months->setText(listWidgetItem->text() + " ↓");
  ui->listWidget_months_years->hide();
  ui->pushButton_months->show();
}

void credit::show_credit() {
  hideall();
  ui->amount->show();
  ui->interest_rate->show();
  ui->lineEdit_amount->show();
  ui->lineEdit_monthly_pay->show();
  ui->lineEdit_rate->show();
  ui->rub->show();
  ui->rub_2->show();
  ui->credit_monthly_pay->show();
  ui->percent->show();
  ui->pushButton_calculate_credit->show();
  ui->line_2->show();
  ui->lineEdit_amount->setText("");
  ui->lineEdit_monthly_pay->setText("");
  ui->lineEdit_rate->setText("");
  setFixedSize(415, 500);
}

void credit::on_pushButton_calculate_credit_clicked() {
  if (QString::compare(ui->lineEdit_amount->text(), "") &&
      QString::compare(ui->lineEdit_monthly_pay->text(), "") &&
      QString::compare(ui->lineEdit_rate->text(), "")) {
    ui->lineEdit_term->setText("");
    long double sum = ui->lineEdit_amount->text().toDouble();
    double pay = ui->lineEdit_monthly_pay->text().toDouble();
    double rate = ui->lineEdit_rate->text().toDouble();
    int64_t term = 0;
    double overpay = 0, payout = 0;
    calc_term(sum, pay, rate, &term, &overpay, &payout);
    int64_t years = term / 12;
    if (years > 0) {
      ui->lineEdit_term->insert(QString::number(years) + " year");
      if (years > 1) ui->lineEdit_term->insert("s");
      ui->lineEdit_term->insert(" ");
    }
    if (term % 12 != 0)
      ui->lineEdit_term->insert(QString::number(term % 12) + " month");
    if (term % 12 > 1) ui->lineEdit_term->insert("s");
    ui->lineEdit_overpay_term->setText(QString::number(overpay, 'f', 2));
    ui->lineEdit_fullsum_term->setText(QString::number(payout, 'f', 2));
    double first = (sum * (100 + rate / 12) / 100) - sum;
    if (first >= pay)
      QMessageBox::warning(this, "Error",
                           "Monthly payment (" + QString::number(pay) +
                               ") less than accrued interest (" +
                               QString::number(first) + ")");
    ui->text_total_pay_term->show();
    ui->text_total_over_term->show();
    ui->text_total_term->show();
    ui->lineEdit_overpay_term->show();
    ui->lineEdit_fullsum_term->show();
    ui->lineEdit_term->show();
  }
}

void credit::show_loan() {
  hideall();
  ui->credit_monthly_pay_loan->show();
  ui->lineEdit_monthly_pay_loan->show();
  ui->credit_term->show();
  ui->lineEdit_monthly_pay->show();
  ui->pushButton_months->show();
  ui->interest_rate->show();
  ui->lineEdit_rate->show();
  ui->percent->show();
  ui->rub->show();
  ui->pushButton_calculate_loan->show();
  ui->line_2->show();
  ui->lineEdit_monthly_pay_loan->setText("");
  ui->lineEdit_monthly_pay->setText("");
  ui->lineEdit_rate->setText("");
  ui->lineEdit_overpay_term->setText("");
  ui->lineEdit_fullsum_term->setText("");
  ui->lineEdit_term->setText("");
  setFixedSize(415, 500);
}

void credit::on_pushButton_calculate_loan_clicked() {
  if (QString::compare(ui->lineEdit_monthly_pay_loan->text(), "") &&
      QString::compare(ui->lineEdit_monthly_pay->text(), "") &&
      QString::compare(ui->lineEdit_rate->text(), "")) {
    long double sum = ui->lineEdit_monthly_pay_loan->text().toDouble();
    int64_t term = ui->lineEdit_monthly_pay->text().toLongLong();
    if (ui->pushButton_months->text().at(0) == 'Y') term *= 12;
    double rate = ui->lineEdit_rate->text().toDouble();
    double credit = 0, overpay = 0, payout = 0;
    calc_loan(sum, term, rate, &credit, &overpay, &payout);
    ui->lineEdit_term->setText(QString::number(credit, 'f', 2));
    ui->lineEdit_overpay_term->setText(QString::number(overpay, 'f', 2));
    ui->lineEdit_fullsum_term->setText(QString::number(payout, 'f', 2));
    ui->text_total_pay_term->show();
    ui->text_total_over_term->show();
    ui->text_total_credit->show();
    ui->lineEdit_overpay_term->show();
    ui->lineEdit_fullsum_term->show();
    ui->lineEdit_term->show();
  }
}
