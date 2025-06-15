QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QT += core gui widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts


SOURCES += \
    Controller.cpp \
    Modules/BasalPump.cpp \
    Modules/Battery.cpp \
    Modules/BolusPump.cpp \
    Modules/Cartridge.cpp \
    Modules/CgmDataManager.cpp \
    Modules/FileLogger.cpp \
    Modules/Injector.cpp \
    Modules/ProfileDB.cpp \
    Modules/SessionRecorder.cpp \
    Person.cpp \
    Prediction/ControlIQ.cpp \
    Prediction/HoltLinear.cpp \
    Profile.cpp \
    Session.cpp \
    UI/PinManager.cpp \
    UI/datastoragescreen.cpp \
    UI/homescreen.cpp \
    UI/mainwindow.cpp \
    UI/manualbolusscreen.cpp \
    UI/offscreen.cpp \
    UI/passwordscreen.cpp \
    UI/personalprofilescreen.cpp \
    UI/pumpsettingsscreen.cpp \
    UI/windowmanager.cpp \
    main.cpp \

HEADERS += \
    Controller.h \
    DeviceSettings.h \
    Enums/Thresholds.h \
    Interfaces/Pump.h \
    Modules/BasalPump.h \
    Modules/Battery.h \
    Modules/BolusPump.h \
    Modules/Cartridge.h \
    Modules/CgmDataManager.h \
    Modules/FileLogger.h \
    Modules/Injector.h \
    Modules/ProfileDB.h \
    Modules/SessionRecorder.h \
    Person.h \
    Prediction/ControlIQ.h \
    Prediction/HoltLinear.h \
    Profile.h \
    Session.h \
    UI/PinManager.h \
    UI/datastoragescreen.h \
    UI/homescreen.h \
    UI/mainwindow.h \
    UI/manualbolusscreen.h \
    UI/offscreen.h \
    UI/passwordscreen.h \
    UI/personalprofilescreen.h \
    UI/pumpsettingsscreen.h \
    UI/windowmanager.h \
    Utils/Duration.h \
    Utils/FileWriter.h \
    Utils/RNG.h \
    Utils/RangeCompare.h

FORMS += \
    UI/datastoragescreen.ui \
    UI/homescreen.ui \
    UI/mainwindow.ui \
    UI/manualbolusscreen.ui \
    UI/offscreen.ui \
    UI/passwordscreen.ui \
    UI/personalprofilescreen.ui \
    UI/pumpsettingsscreen.ui \


RESOURCES += resources.qrc



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
