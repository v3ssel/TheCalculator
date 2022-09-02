// by stevenso aka v3ssel
#include "../Deposit/deposit.h"

#include "../CalculatorSource/calc.h"
#include "ui_deposit.h"

deposit::deposit(QWidget *parent) : QDialog(parent), ui(new Ui::deposit) {
  ui->setupUi(this);
  font = QFont("Monaco");
  ui->listWidget_months_years->hide();
  ui->listWidget_rateType->hide();
  ui->listWidget_period->hide();
  ui->lineEdit_depends->hide();
  ui->pushButton_addMore->hide();
  ui->text_dependsOut->hide();
  createLists();

  ui->lineEdit_dep_term->setValidator(new QIntValidator(0, (int)1e10, this));
  ui->lineEdit_dep_amount->setValidator(new QDoubleValidator(0, 1e10, 3, this));
  ui->lineEdit_depends->setValidator(new QIntValidator(0, (int)1e10, this));
  ui->lineEdit_rate->setValidator(new QDoubleValidator(0, 1e10, 3, this));
  ui->lineEdit_taxRate->setValidator(new QDoubleValidator(0, 1e10, 3, this));

  QTextCharFormat fmt;
  fmt.setForeground(QBrush(Qt::black));
  ui->dateEdit->calendarWidget()->setWeekdayTextFormat(Qt::Saturday, fmt);
  ui->dateEdit->calendarWidget()->setWeekdayTextFormat(Qt::Sunday, fmt);
  ui->dateEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
  current = QDate::currentDate();
  ui->dateEdit->setDate(current);
}

deposit::~deposit() { delete ui; }

void deposit::createLists() {
  ui->listWidget_months_years->addItem("Days");
  ui->listWidget_months_years->addItem("Months");
  ui->listWidget_months_years->addItem("Years");
  ui->listWidget_rateType->addItem("Fixed");
  ui->listWidget_rateType->addItem("Depends on the amount");
  ui->listWidget_rateType->addItem("Depends on the term");
  ui->listWidget_period->addItem("Everyday");
  ui->listWidget_period->addItem("Every week");
  ui->listWidget_period->addItem("Once a month");
  ui->listWidget_period->addItem("Once a quarter");
  ui->listWidget_period->addItem("Semiannually");
  ui->listWidget_period->addItem("Once a year");
  ui->listWidget_period->addItem("At the end of the term");
}

void deposit::on_pushButton_back_clicked() { this->close(); }

void deposit::on_pushButton_calculate_clicked() {
  long double amount = ui->lineEdit_dep_amount->text().toDouble();
  int64_t term = ui->lineEdit_dep_term->text().toLongLong();
  QDate startTerm = ui->dateEdit->date();
  if (ui->pushButton_termType->text().at(0) == 'Y')
    term = startTerm.daysTo(startTerm.addYears(term));
  if (ui->pushButton_termType->text().at(0) == 'M')
    term = startTerm.daysTo(startTerm.addMonths(term));
  _Bool rateType =
      ui->pushButton_rateType->text().contains("term") ? true : false;

  size_t rateCounter = ui->widgets_frame->layout()->count();
  int64_t daysOrAmountRate[rateCounter];
  long double interestRate[rateCounter + 1];
  interestRate[0] = 0, interestRate[1] = ui->lineEdit_rate->text().toDouble();
  daysOrAmountRate[0] = 1;

  bool fields = true;
  if (ui->pushButton_rateType->text().at(0) != 'F') {
    fields = RateFill(daysOrAmountRate, rateCounter, interestRate);
  }

  long double taxRate = ui->lineEdit_taxRate->text().toDouble();
  short period = 1;
  setPeriod(term, &period);
  bool capitalism = ui->checkBox_capitalism->isChecked() ? true : false;

  if ((ui->lineEdit_dep_amount->text() == "") ||
      (ui->lineEdit_dep_term->text() == "") ||
      (ui->lineEdit_rate->text() == "") ||
      (ui->lineEdit_taxRate->text() == "")) {
    fields = false;
  }

  QVBoxLayout *vbox_rep =
      qobject_cast<QVBoxLayout *>(ui->widgets_frame_rep->layout());
  QVBoxLayout *vbox_with =
      qobject_cast<QVBoxLayout *>(ui->widgets_frame_with->layout());
  int64_t replCounter = vbox_rep->count();
  QDate RepWithDates[replCounter + vbox_with->count() - 1];
  long double RepWithValues[replCounter + vbox_with->count() - 1];
  size_t toI = 1;
  if (replCounter > 1)
    fields = RepAndWithFill(replCounter - 1, RepWithDates, RepWithValues,
                            vbox_rep, &toI, false);

  if (vbox_with->count() > 1)
    fields = RepAndWithFill(vbox_with->count() - 1, RepWithDates, RepWithValues,
                            vbox_with, &toI, true);

  size_t size = (replCounter - 1 + vbox_with->count() - 1);
  if (replCounter > 1 || vbox_with->count() > 1) {
    SortAndDelDuples(RepWithDates, RepWithValues, &size);
  }
  int64_t DatesInDays[size];
  for (size_t i = 0; i < size; i++) {
    DatesInDays[i] = startTerm.daysTo(RepWithDates[i]);
  }

  if (startTerm > RepWithDates[0] &&
      (replCounter > 1 || vbox_with->count() > 1)) {
    QMessageBox::warning(this, "Error", "Wrong dates");
    return;
  }
  if (!fields) {
    QMessageBox::warning(this, "Error", "All fields must be completed right!");
    return;
  }
  long double AccRate = 0, taxPaid = 0, resultSum = amount;
  if (size) {
    for (size_t i = 0; capitalism && i < size; i++) amount += RepWithValues[i];
    deposit_calc(amount, term, rateType, daysOrAmountRate, rateCounter, size,
                 interestRate, taxRate, period, capitalism, DatesInDays,
                 RepWithValues, &AccRate, &taxPaid, &resultSum);
  } else {
    deposit_calc(amount, term, rateType, daysOrAmountRate, rateCounter, size,
                 interestRate, taxRate, period, capitalism, NULL, NULL,
                 &AccRate, &taxPaid, &resultSum);
  }
  QMessageBox::information(
      this, "Result",
      "Accrued interest: " +
          (AccRate < 0 ? "0.00" : QString::number((double)AccRate, 'f', 2)) +
          "\n" + "Tax amount: " +
          (taxPaid < 0 ? "0.00" : QString::number((double)taxPaid, 'f', 2)) +
          "\n" + "Amount on account: " +
          (resultSum < 0 ? "0.00"
                         : QString::number((double)resultSum, 'f', 2)) +
          "\n");
}

void deposit::SortAndDelDuples(QDate *RepWithDates, long double *RepWithValues,
                               size_t *size) {
  for (size_t i = 0; i < *size; i++) {
    for (size_t j = i + 1; j < *size; j++) {
      QDate temp = current;
      long double tmp = 0;
      if (RepWithDates[j] < RepWithDates[i]) {
        temp = RepWithDates[i];
        RepWithDates[i] = RepWithDates[j];
        RepWithDates[j] = temp;

        tmp = RepWithValues[i];
        RepWithValues[i] = RepWithValues[j];
        RepWithValues[j] = tmp;
      }
    }
  }

  for (size_t counter1 = 0; counter1 < *size; counter1++) {
    for (size_t counter2 = counter1 + 1; counter2 < *size; counter2++) {
      if (RepWithDates[counter1] == RepWithDates[counter2]) {
        RepWithValues[counter1] += RepWithValues[counter2];
        for (size_t counter_shift = counter2; counter_shift < *size - 1;
             counter_shift++) {
          RepWithDates[counter_shift] = RepWithDates[counter_shift + 1];
          RepWithValues[counter_shift] = RepWithValues[counter_shift + 1];
        }
        (*size)--;
        if (RepWithDates[counter1] == RepWithDates[counter2]) {
          counter2--;
        }
      }
    }
  }
}

bool deposit::RateFill(int64_t *daysOrAmountRate, size_t rateCounter,
                       long double *interestRate) {
  bool ret = true;
  daysOrAmountRate[0] = ui->lineEdit_depends->text().toLongLong();
  QVBoxLayout *vbox = qobject_cast<QVBoxLayout *>(ui->widgets_frame->layout());
  for (size_t i = 1; i < rateCounter; i++) {
    QLayout *hb = vbox->itemAt(i)->layout();
    QWidget *am = hb->itemAt(0)->widget();
    QLineEdit *ame = dynamic_cast<QLineEdit *>(am);
    QWidget *rat = hb->itemAt(1)->widget();
    QLineEdit *rate = dynamic_cast<QLineEdit *>(rat);
    if ((ame->text() == "") || (rate->text() == "") ||
        (ui->lineEdit_depends->text() == ""))
      ret = false;
    daysOrAmountRate[i] = ame->text().toLongLong();
    interestRate[i + 1] = rate->text().toDouble();
  }

  for (size_t i = 0; i < rateCounter; i++) {
    for (size_t j = i + 1; j < rateCounter; j++) {
      int64_t temp;
      long double tmp = 0;
      if (daysOrAmountRate[j] < daysOrAmountRate[i]) {
        temp = daysOrAmountRate[i];
        daysOrAmountRate[i] = daysOrAmountRate[j];
        daysOrAmountRate[j] = temp;

        tmp = interestRate[i + 1];
        interestRate[i + 1] = interestRate[j + 1];
        interestRate[j + 1] = tmp;
      }
    }
  }
  return ret;
}

bool deposit::RepAndWithFill(size_t count, QDate *RepWithDates,
                             long double *RepWithValues, QVBoxLayout *vbox_rep,
                             size_t *toI, bool withdraw) {
  bool ret = true;
  for (size_t i = 1; i < count + 1; i++) {
    QLayout *hbox_rep = vbox_rep->itemAt(i)->layout();
    QWidget *dat = hbox_rep->itemAt(0)->widget();
    QDateEdit *date = dynamic_cast<QDateEdit *>(dat);
    QWidget *amo = hbox_rep->itemAt(1)->widget();
    QLineEdit *amn = dynamic_cast<QLineEdit *>(amo);
    if (amn->text() == "") ret = false;
    RepWithDates[*toI - 1] = date->date();
    RepWithValues[*toI - 1] =
        withdraw ? -amn->text().toDouble() : amn->text().toDouble();
    (*toI)++;
  }
  return ret;
}

void deposit::setPeriod(int64_t term, short *period) {
  if (ui->pushButton_periodType->text().contains("Everyday")) *period = 1;
  if (ui->pushButton_periodType->text().contains("Every week")) *period = 7;
  if (ui->pushButton_periodType->text().contains("Once a month")) *period = 30;
  if (ui->pushButton_periodType->text().contains("Once a quarter"))
    *period = 365 / 4;
  if (ui->pushButton_periodType->text().contains("Semiannually"))
    *period = 365 / 2;
  if (ui->pushButton_periodType->text().contains("Once a year")) *period = 365;
  if (ui->pushButton_periodType->text().contains("At the end of the term"))
    *period = term;
}

void deposit::on_pushButton_termType_clicked() {
  ui->listWidget_months_years->show();
  connect(ui->listWidget_months_years,
          SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
          SLOT(itemDClickedTERM()));
}

void deposit::itemDClickedTERM() {
  ui->listWidget_months_years->hide();
  ui->pushButton_termType->setText(
      ui->listWidget_months_years->currentItem()->text() + " ↓");
}

void deposit::on_pushButton_rateType_clicked() {
  ui->listWidget_rateType->show();
  connect(ui->listWidget_rateType, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          this, SLOT(itemDClickedRATE()));
}

void deposit::itemDClickedRATE() {
  ui->pushButton_rateType->setText(
      ui->listWidget_rateType->currentItem()->text() + " ↓");
  if (ui->listWidget_rateType->currentItem()->text().at(0) == 'F') {
    ui->groupBox->move(10, 210);
    ui->listWidget_rateType->hide();
    ui->pushButton_rateType->setFixedWidth(100);
    ui->text_rateSym->move(310, 190);
    ui->lineEdit_rate->move(240, 190);
    ui->lineEdit_depends->hide();
    ui->text_dependsOut->hide();
    ui->pushButton_addMore->hide();
  } else if (ui->listWidget_rateType->currentItem()->text().contains(
                 "amount")) {
    ui->groupBox->move(10, 248);
    ui->listWidget_rateType->hide();
    ui->pushButton_rateType->setFixedWidth(200);
    ui->text_rateSym->move(306, 210);
    ui->lineEdit_rate->move(311, 240);
    ui->lineEdit_depends->show();
    ui->text_dependsOut->setText("Amount with which the rate is valid");
    ui->text_dependsOut->setFontWeight(9);
    ui->text_dependsOut->show();
    ui->pushButton_addMore->show();
  } else if (ui->listWidget_rateType->currentItem()->text().contains("term")) {
    ui->groupBox->move(10, 248);
    ui->listWidget_rateType->hide();
    ui->pushButton_rateType->setFixedWidth(200);
    ui->text_rateSym->move(306, 210);
    ui->lineEdit_rate->move(311, 240);
    ui->lineEdit_depends->show();
    ui->text_dependsOut->setText(
        "Number of the day from which the rate is valid");
    ui->text_dependsOut->setFontWeight(9);
    ui->text_dependsOut->show();
    ui->pushButton_addMore->show();
  }
}

void deposit::on_pushButton_addMore_clicked() {
  if (this->height() < 1300) {
    QVBoxLayout *layout =
        qobject_cast<QVBoxLayout *>(ui->widgets_frame->layout());
    QHBoxLayout *newLayout = new QHBoxLayout(ui->widgets_frame);

    QLineEdit *depends_new = new QLineEdit(ui->widgets_frame);
    depends_new->setStyleSheet(ui->lineEdit_dep_amount->styleSheet());
    depends_new->setFixedSize(170, 30);
    depends_new->setFont(font);
    depends_new->setValidator(new QDoubleValidator(0, 1e10, 3, this));
    newLayout->addWidget(depends_new);

    QLineEdit *rate_new = new QLineEdit(ui->widgets_frame);
    rate_new->setFixedSize(65, 30);
    rate_new->setStyleSheet(ui->lineEdit_dep_amount->styleSheet());
    rate_new->setFont(font);
    rate_new->setValidator(new QDoubleValidator(0, 1e10, 3, this));
    newLayout->addWidget(rate_new);

    QPushButton *button = new QPushButton("X", ui->widgets_frame);
    button->setFixedSize(20, 32);
    newLayout->addWidget(button);
    layout->addLayout(newLayout);

    mButtonToLayoutMap.insert(button, newLayout);

    ui->widgets_frame->setFixedHeight(ui->widgets_frame->height() + 14);
    this->setFixedHeight(this->height() + 42);
    ui->groupBox->move(10,
                       ui->widgets_frame->y() + ui->widgets_frame->height());

    QObject::connect(button, &QPushButton::clicked, this, &deposit::delOneRate);
    connect(ui->listWidget_rateType,
            SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
            SLOT(delAllRate()));
  } else {
    ui->pushButton_addMore->hide();
  }
}

void deposit::delOneRate() {
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  QHBoxLayout *layout = mButtonToLayoutMap.take(button);

  while (layout->count() != 0) {
    QLayoutItem *item = layout->takeAt(0);
    delete item->widget();
    delete item;
  }
  delete layout;

  ui->widgets_frame->setFixedHeight(ui->widgets_frame->height() - 42);
  this->setFixedHeight(this->height() - 42);
  ui->groupBox->move(10, ui->groupBox->y() - 42);
  maxSize();
}

void deposit::delAllRate() {
  QVBoxLayout *vbox = qobject_cast<QVBoxLayout *>(ui->widgets_frame->layout());
  while (!vbox->isEmpty()) {
    QLayout *hb = vbox->takeAt(vbox->count() - 1)->layout();
    while (!hb->isEmpty()) {
      QWidget *w = hb->takeAt(0)->widget();
      delete w;
    }
    delete hb;
  }
  ui->widgets_frame->setFixedSize(291, 8);
  maxSize();
  this->setFixedHeight(ui->groupBox->height() + ui->groupBox->y());
}

void deposit::on_pushButton_periodType_clicked() {
  ui->listWidget_period->show();
  connect(ui->listWidget_period, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          this, SLOT(itemDClickedPERIOD()));
}

void deposit::itemDClickedPERIOD() {
  ui->listWidget_period->hide();
  ui->pushButton_periodType->setText(
      ui->listWidget_period->currentItem()->text() + " ↓");
}

void deposit::on_pushButton_addMoreRep_clicked() {
  if (this->height() < 1300) {
    QVBoxLayout *V_layout =
        qobject_cast<QVBoxLayout *>(ui->widgets_frame_rep->layout());
    QHBoxLayout *H_Layout = new QHBoxLayout(ui->widgets_frame_rep);

    QDateEdit *calendar = new QDateEdit(ui->widgets_frame_rep);
    calendar->setFixedSize(120, 25);
    calendar->setCalendarPopup(true);
    calendar->setDisplayFormat("dd.MM.yyyy");
    calendar->setDate(current);
    calendar->setFont(font);
    calendar->setStyleSheet(ui->dateEdit->styleSheet());
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(Qt::black));
    calendar->calendarWidget()->setWeekdayTextFormat(Qt::Saturday, fmt);
    calendar->calendarWidget()->setWeekdayTextFormat(Qt::Sunday, fmt);
    calendar->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    H_Layout->addWidget(calendar);

    QLineEdit *line = new QLineEdit(ui->widgets_frame_rep);
    line->setFixedSize(130, 25);
    line->setStyleSheet(ui->lineEdit_dep_amount->styleSheet());
    line->setFont(font);
    line->setValidator(new QDoubleValidator(0, 1e10, 3, this));
    line->setPlaceholderText("Amount");
    H_Layout->addWidget(line);

    QPushButton *x = new QPushButton("X", ui->widgets_frame_rep);
    x->setFixedSize(20, 32);
    H_Layout->addWidget(x);

    V_layout->addLayout(H_Layout);
    mButtonToLayoutMap.insert(x, H_Layout);

    ui->widgets_frame_rep->setFixedHeight(ui->widgets_frame_rep->height() + 42);
    this->setFixedHeight(this->height() + 42);
    ui->groupBox->setFixedHeight(ui->groupBox->height() + 42);
    ui->groupBox_2->move(ui->groupBox_2->x(), ui->groupBox_2->y() + 42);
    ui->pushButton_calculate->move(ui->pushButton_calculate->x(),
                                   ui->pushButton_calculate->y() + 42);

    QObject::connect(x, &QPushButton::clicked, this, &deposit::delOneRep);
  } else {
    ui->pushButton_addMoreRep->hide();
  }
}

void deposit::delOneRep() {
  QPushButton *x = qobject_cast<QPushButton *>(sender());
  QHBoxLayout *layout = mButtonToLayoutMap.take(x);

  while (layout->count() != 0) {
    QLayoutItem *item = layout->takeAt(0);
    delete item->widget();
    delete item;
  }
  delete layout;
  ui->widgets_frame_rep->setFixedHeight(ui->widgets_frame_rep->height() - 42);
  this->setFixedHeight(this->height() - 42);
  ui->groupBox->setFixedHeight(ui->groupBox->height() - 42);
  ui->groupBox_2->move(ui->groupBox_2->x(), ui->groupBox_2->y() - 42);
  ui->pushButton_calculate->move(ui->pushButton_calculate->x(),
                                 ui->pushButton_calculate->y() - 42);
  maxSize();
}

void deposit::on_pushButton_addMoreWith_clicked() {
  if (this->height() < 1300) {
    QVBoxLayout *V_layout =
        qobject_cast<QVBoxLayout *>(ui->widgets_frame_with->layout());
    QHBoxLayout *H_Layout = new QHBoxLayout(ui->widgets_frame_with);

    QDateEdit *calendar = new QDateEdit(ui->widgets_frame_with);
    calendar->setFixedSize(120, 25);
    calendar->setCalendarPopup(true);
    calendar->setDisplayFormat("dd.MM.yyyy");
    calendar->setDate(current);
    calendar->setFont(font);
    calendar->setStyleSheet(ui->dateEdit->styleSheet());
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(Qt::black));
    calendar->calendarWidget()->setWeekdayTextFormat(Qt::Saturday, fmt);
    calendar->calendarWidget()->setWeekdayTextFormat(Qt::Sunday, fmt);
    calendar->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    H_Layout->addWidget(calendar);

    QLineEdit *line = new QLineEdit(ui->widgets_frame_with);
    line->setFixedSize(130, 25);
    line->setStyleSheet(ui->lineEdit_dep_amount->styleSheet());
    line->setFont(font);
    line->setValidator(new QDoubleValidator(0, 1e10, 3, this));
    line->setPlaceholderText("Amount");
    H_Layout->addWidget(line);

    QPushButton *x = new QPushButton("X", ui->widgets_frame_with);
    x->setFixedSize(20, 32);
    H_Layout->addWidget(x);

    V_layout->addLayout(H_Layout);
    mButtonToLayoutMap.insert(x, H_Layout);

    ui->groupBox->setFixedHeight(ui->groupBox->height() + 37);
    ui->groupBox_2->setFixedHeight(ui->groupBox_2->height() + 37);
    ui->widgets_frame_with->setFixedHeight(ui->widgets_frame_with->height() +
                                           37);
    this->setFixedHeight(this->height() + 37);
    ui->pushButton_calculate->move(ui->pushButton_calculate->x(),
                                   ui->pushButton_calculate->y() + 37);

    QObject::connect(x, &QPushButton::clicked, this, &::deposit::delOneWith);
  } else {
    ui->pushButton_addMoreWith->hide();
  }
}

void deposit::delOneWith() {
  QPushButton *x = qobject_cast<QPushButton *>(sender());
  QHBoxLayout *layout = mButtonToLayoutMap.take(x);

  while (layout->count() != 0) {
    QLayoutItem *item = layout->takeAt(0);
    delete item->widget();
    delete item;
  }
  delete layout;
  ui->groupBox->setFixedHeight(ui->groupBox->height() - 37);
  ui->groupBox_2->setFixedHeight(ui->groupBox_2->height() - 37);
  ui->widgets_frame_with->setFixedHeight(ui->widgets_frame_with->height() - 37);
  ui->pushButton_calculate->move(ui->pushButton_calculate->x(),
                                 ui->pushButton_calculate->y() - 37);
  this->setFixedHeight(this->height() - 37);
  maxSize();
}

void deposit::maxSize() {
  if (this->height() < 1300) {
    if (!(ui->pushButton_rateType->text().at(0) == 'F'))
      ui->pushButton_addMore->show();
    ui->pushButton_addMoreRep->show();
    ui->pushButton_addMoreWith->show();
  } else {
    ui->pushButton_addMore->hide();
    ui->pushButton_addMoreRep->hide();
    ui->pushButton_addMoreWith->hide();
  }
}
