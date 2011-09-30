#pragma once
//#SRCHEADER
#ifdef LOCO_GUI
#include <QApplication>
typedef QApplication LocoQtApp;
#else
#include <QCoreApplication>
typedef QCoreApplication LocoQtApp;
#endif