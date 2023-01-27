TEMPLATE = app
CONFIG += c++17
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -march=native -pipe

SOURCES += \
        main.cpp

unix|win32: LIBS += -lSDL2

unix|win32: LIBS += -lSDL2_ttf

unix|win32: LIBS += -lSDL2_image
