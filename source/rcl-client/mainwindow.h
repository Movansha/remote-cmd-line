#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"

#include <QString>
#include <QCloseEvent>

//---------

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow* instance;

    void load_cfg_into_ui();

signals:
    void add_to_output(const QString& text);
    void disable_disconnect(const bool& status);

private slots:
    void handle_add_to_output(const QString& text);
    void handle_disable_disconnect(const bool& status);

    void switch_auto_connect(const bool& status);
    void change_preferences();

    void connect_to_server();
    void disconnect_from_server();

    void switch_dark_mode(const bool& status);

private:
    Ui::MainWindowClass ui;
    void connect_slots();

protected:
    void closeEvent(QCloseEvent* event) override;
};
