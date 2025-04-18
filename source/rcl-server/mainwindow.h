#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"

#include <QString>
#include <QCloseEvent>

//----------

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow* instance;

    void load_cfg_into_ui();

signals:
    void add_to_output(const QString& text);
    void disable_func_buttons(const bool& status);

private slots:
    void handle_add_to_output(const QString& text);
    void handle_disable_buttons(const bool& status);

    void change_port(const int& value);
    void stop_server();
    void start_server();

    void change_password();

    void beep();
    void message();

    void open_folder();
    void switch_theme();

    void send_command();

private:
    Ui::MainWindowClass ui;
    void connect_slots();

protected:
    void closeEvent(QCloseEvent* event) override;
};
