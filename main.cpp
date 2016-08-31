#include <QApplication>
#include "SettingsWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SettingsWindow window;

    return app.exec();
}
