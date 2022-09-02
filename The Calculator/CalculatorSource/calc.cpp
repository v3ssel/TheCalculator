// by stevenso aka v3ssel
#include "calc.h"

#include "ui_calc.h"

calc::calc(QWidget *parent) : QMainWindow(parent), ui(new Ui::calc) {
  ui->setupUi(this);
  auto validator = new MyValidator(parent);
  ui->inputField->setValidator(validator);
  ui->pushButton_less->hide();
  ui->pushButton_def->hide();
  ui->lineEdit_xMax->setValidator(new QDoubleValidator(this));
  ui->lineEdit_xMin->setValidator(new QDoubleValidator(this));
  ui->lineEdit_xValue->setValidator(new QDoubleValidator(this));
  on_pushButton_def_clicked();
  ui->graphic->xAxis->setRange(-10, 10);
  ui->graphic->yAxis->setRange(-10, 10);
  connect(ui->inputField, SIGNAL(returnPressed()), this,
          SLOT(on_pushButton_start_clicked()));
}

calc::~calc() { delete ui; }

void calc::on_pushButton_deposit_clicked() {
  dep = new deposit(this);
  dep->setFixedSize(415, 510);
  dep->setWindowTitle("Deposit Calculator");
  dep->show();
}

void calc::on_pushButton_credit_clicked() {
  cred = new credit(this);
  cred->setFixedSize(415, 600);
  cred->setWindowTitle("Credit Calculator");
  cred->show();
}

void calc::on_pushButton_upGraph_clicked() {
  ui->graphic->clearGraphs();
  long double dxMax = ui->lineEdit_xMax->text().toDouble();
  long double dxMin = ui->lineEdit_xMin->text().toDouble();
  if (dxMax < dxMin || dxMin < -100000)
    ui->lineEdit_xMin->setText("Invalid axis");
  if (dxMax > 100000) ui->lineEdit_xMax->setText("Invalid axis");
  if (QString::compare("Invalid axis", ui->lineEdit_xMax->text()) &&
      QString::compare("Invalid axis", ui->lineEdit_xMin->text())) {
    ui->graphic->setInteraction(QCP::iRangeZoom, true);
    ui->graphic->setInteraction(QCP::iRangeDrag, true);
    QByteArray tmp = ui->inputField->text().toLocal8Bit();
    char *exp = tmp.data();
    long double res = 0.0;
    for (double i = dxMin; i <= dxMax; i += 0.1) {
      x.push_back(i);
      if (!entryPoint(exp, &res, i))
        ui->answerField->setText("Invalid expression");
      y.push_back(res);
    }
    ui->graphic->addGraph();
    ui->graphic->graph(0)->addData(x, y);
    ui->graphic->graph(0)->setPen(QPen(Qt::black));
    ui->graphic->replot();
    x.clear();
    y.clear();
  }
}

void calc::on_pushButton_def_clicked() {
  ui->lineEdit_xMax->setText("100");
  ui->lineEdit_xMin->setText("-100");
}

void calc::on_pushButton_more_clicked() {
  setFixedSize(870, 575);
  ui->pushButton_more->hide();
  ui->pushButton_def->show();
  ui->pushButton_less->show();
}

void calc::on_pushButton_less_clicked() {
  setFixedSize(440, 575);
  ui->pushButton_less->hide();
  ui->pushButton_def->hide();
  ui->pushButton_more->show();
}

void calc::on_pushButton_start_clicked() {
  QByteArray tmp = ui->inputField->text().toLocal8Bit();
  char *exp = tmp.data();
  long double value = 0.0, xValue = 0.0;
  if (ui->lineEdit_xValue->text() != "")
    xValue = ui->lineEdit_xValue->text().toDouble();
  if (entryPoint(exp, &value, xValue)) {
    QString strValue = QString::number((double)value, 'f', 7);
    ui->answerField->setText(strValue);
  } else {
    ui->answerField->setText("Invalid expression");
  }
}

void calc::on_pushButton_help_clicked() {
  QMessageBox::information(this, "Calculator helper",
                           ui->pushButton_help->toolTip());
}

void calc::on_pushButton_0_clicked() { ui->inputField->insert("0"); }

void calc::on_pushButton_1_clicked() { ui->inputField->insert("1"); }

void calc::on_pushButton_2_clicked() { ui->inputField->insert("2"); }

void calc::on_pushButton_3_clicked() { ui->inputField->insert("3"); }

void calc::on_pushButton_4_clicked() { ui->inputField->insert("4"); }

void calc::on_pushButton_5_clicked() { ui->inputField->insert("5"); }

void calc::on_pushButton_6_clicked() { ui->inputField->insert("6"); }

void calc::on_pushButton_7_clicked() { ui->inputField->insert("7"); }

void calc::on_pushButton_8_clicked() { ui->inputField->insert("8"); }

void calc::on_pushButton_9_clicked() { ui->inputField->insert("9"); }

void calc::on_pushButton_x_clicked() { ui->inputField->insert("x"); }

void calc::on_pushButton_dot_clicked() { ui->inputField->insert("."); }

void calc::on_pushButton_plus_clicked() { ui->inputField->insert("+"); }

void calc::on_pushButton_minus_clicked() { ui->inputField->insert("-"); }

void calc::on_pushButton_mult_clicked() { ui->inputField->insert(" * "); }

void calc::on_pushButton_div_clicked() { ui->inputField->insert(" / "); }

void calc::on_pushButton_mod_clicked() { ui->inputField->insert(" mod "); }

void calc::on_pushButton_lBracket_clicked() { ui->inputField->insert("("); }

void calc::on_pushButton_rBracket_clicked() { ui->inputField->insert(")"); }

void calc::on_pushButton_sin_clicked() {
  ui->inputField->insert("sin()");
  ui->inputField->cursorBackward(0, 1);
}

void calc::on_pushButton_cos_clicked() {
  ui->inputField->insert("cos()");
  ui->inputField->cursorBackward(0, 1);
}

void calc::on_pushButton_tan_clicked() {
  ui->inputField->insert("tan()");
  ui->inputField->cursorBackward(0, 1);
}

void calc::on_pushButton_asin_clicked() {
  ui->inputField->insert("asin()");
  ui->inputField->cursorBackward(0, 1);
}

void calc::on_pushButton_acos_clicked() {
  ui->inputField->insert("acos()");
  ui->inputField->cursorBackward(0, 1);
}

void calc::on_pushButton_atan_clicked() {
  ui->inputField->insert("atan()");
  ui->inputField->cursorBackward(0, 1);
}

void calc::on_pushButton_ln_clicked() {
  ui->inputField->insert("ln()");
  ui->inputField->cursorBackward(0, 1);
}

void calc::on_pushButton_log_clicked() {
  ui->inputField->insert("log()");
  ui->inputField->cursorBackward(0, 1);
}

void calc::on_pushButton_sqrt_clicked() {
  ui->inputField->insert("sqrt()");
  ui->inputField->cursorBackward(0, 1);
}

void calc::on_pushButton_power_clicked() { ui->inputField->insert("^"); }

void calc::on_pushButton_clearInput_clicked() { ui->inputField->setText(""); }

void calc::on_pushButton_clearAnswer_clicked() { ui->answerField->setText(""); }

void calc::on_pushButton_allclear_clicked() {
  ui->inputField->setText("");
  ui->answerField->setText("");
}
