// by stevenso aka v3ssel
#include <QApplication>

#include "CalculatorSource/calc.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  calc w;
  QFont font("Monospace");
  w.setWindowTitle("Calculator v1.0");
  w.setFixedSize(440, 575);
  w.show();
  return a.exec();
}
