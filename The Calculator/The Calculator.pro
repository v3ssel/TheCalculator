QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 app_bundle
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CalculatorSource/stack.c \
    CalculatorSource/calc.cpp \
    Credit/credit_calc.c \
    Credit/credit.cpp \
    Deposit/deposit_calc.c \
    Deposit/deposit.cpp \
    Graphics/qcustomplot.cpp \
    main.cpp

HEADERS += \
    CalculatorSource/stack.h \
    CalculatorSource/calc.h \
    Credit/credit.h \
    Deposit/deposit.h \
    Graphics/qcustomplot.h

FORMS += \
    CalculatorSource/calc.ui \
    Credit/credit.ui \
    Deposit/deposit.ui

ICON = icon.icns

# Default rules for deployment.
qnx: target.path = ${HOME}/$${TARGET}/
else: unix:!android: target.path = /${HOME}/$${TARGET}/
!isEmpty(target.path): INSTALLS += target
