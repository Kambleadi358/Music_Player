#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Music Player - DSA Project");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationName("DSA Students");
    a.setOrganizationDomain("dsa.project");



    try {
        MainWindow w;
        w.show();
        return a.exec();
    } catch (const std::exception& e) {
        qCritical() << "Fatal error:" << e.what();
        QMessageBox::critical(nullptr, "Fatal Error", e.what());
        return 1;
    } catch (...) {
        qCritical() << "Unknown fatal error";
        QMessageBox::critical(nullptr, "Fatal Error", "Unknown fatal error occurred.");
        return 2;
    }
}
