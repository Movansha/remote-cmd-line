#include "config.h"
#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <QFile>

//----------

int main(int argc, char *argv[])
{
    setup_json();

    QApplication app(argc, argv);
    MainWindow window;

    window.load_cfg_into_ui();

    //----------

    QFile dark_theme(":/themes/theme/dark.qss");
    QFile light_theme(":/themes/theme/light.qss");

    if (config_json["dark-mode"] == true) {
        app.setStyle("Fusion");
        if (dark_theme.open(QFile::ReadOnly)) window.setStyleSheet(dark_theme.readAll());
    }

    else {
        app.setStyle("WindowsVista");
        if (light_theme.open(QFile::ReadOnly)) window.setStyleSheet(light_theme.readAll());
    }

    dark_theme.close();
    light_theme.close();

    //----------

    window.show();
    return app.exec();
}
