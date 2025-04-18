#include "config.h"
#include "mainwindow.h"

#include "server.h"
#include "functions.hpp"

#include "dialogs.h"
#include "shellapi.h"

//*****

#include <QString>
#include <string>

#include <QDialog>
#include <QIcon>
#include <QSize>
#include <QMessageBox>

#include <QCloseEvent>

//----------

using std::string;

//----------

MainWindow* MainWindow::instance = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    instance = this;

    ui.text_output->append("Developed by: Movansha" "\n");
    ui.text_output->append("Check your router settings for port forwarding." "\n");
    ui.text_output->append(
        "[?] Avaible custom commands:" "\n"
        "  clear" "\n"
        "  screenshot" "\n"
        "  disconnect" "\n"
    );

    MainWindow::connect_slots();
    emit MainWindow::disable_func_buttons(true);
}

MainWindow::~MainWindow() {}

//----------

void MainWindow::handle_add_to_output(const QString& text) {
    ui.text_output->append(text);
}

void MainWindow::handle_disable_buttons(const bool& status) {
    if (status == true && connection == false) {
        ui.spinbox_port->setDisabled(false);
        ui.button_start_server->setDisabled(false);
        ui.button_change_pswd->setDisabled(false);
    }

    else {
        ui.spinbox_port->setDisabled(true);
        ui.button_start_server->setDisabled(true);
        ui.button_change_pswd->setDisabled(true);
    }

    if (connection == true) ui.button_stop_server->setDisabled(false);
    else ui.button_stop_server->setDisabled(status);


    ui.button_beep->setDisabled(status);
    ui.button_message->setDisabled(status);

    ui.button_ss->setDisabled(status);
    ui.button_disconnect->setDisabled(status);

    ui.edit_command->setDisabled(status);
    if (status == true) ui.edit_command->clear();

    ui.button_send->setDisabled(status);
}

//----------

// UI interaction -->

void MainWindow::change_port(const int& value) {
    config_json["port-number"] = value;
    if (!write_json()) {
        MainWindow::load_cfg_into_ui();
    }
}

void MainWindow::stop_server() {
    server::stop();
    ui.text_output->append("[-] Server shut down by the user." "\n");

    emit MainWindow::disable_func_buttons(true);
}

void MainWindow::start_server() {
    server::start();

    QString qstr_port = QString::fromStdString(std::to_string(int(config_json["port-number"])));
    QString qstr_password = QString::fromStdString(config_json["password"]);

    ui.text_output->append("----------");
    ui.text_output->append("Port number: " + qstr_port);
    ui.text_output->append("Password: " + qstr_password);
    ui.text_output->append("----------" "\n");

    ui.text_output->append("Waiting for connection..." "\n");

    //*****

    ui.spinbox_port->setDisabled(true);
    ui.button_start_server->setDisabled(true);
    ui.button_change_pswd->setDisabled(true);

    ui.button_stop_server->setEnabled(true);
}

void MainWindow::change_password() {
    PsswdDialog password_dialog(this);

    password_dialog.exec();
}

void MainWindow::beep() {
    BeepDialog beep_dialog(this);

    int result = beep_dialog.exec();
    if (result == QDialog::Accepted) client::send_beep();
}

void MainWindow::message() {
    MsgDialog message_dialog(this);
    int result = message_dialog.exec();

    if (result == QDialog::Accepted) {
        client::send_message(msgbox_title, msgbox_message);
    }
}

void MainWindow::send_command() {
    string command = ui.edit_command->text().toStdString();
    ui.edit_command->clear();

    if (command == "beep" || command == "message") return;

    //*****

    if (command == "clear") {
        ui.text_output->clear();
    }

    else if (command == "screenshot") client::take_screenshot();
    else if (command == "disconnect") client::disconnect();

    else {
        client::send_command(command);
    }
}

void MainWindow::open_folder() {
    ShellExecuteW(0, L"open", saved_ss.c_str(), NULL, NULL, SW_SHOW);
}

void MainWindow::switch_theme() {
    config_json["dark-theme"] = (config_json["dark-theme"] == false) ? true : false;
    write_json();

    QMessageBox message;

    message.setWindowIcon(QIcon(":/icons/resources/rcl-server.ico"));
    message.setWindowTitle("Theme changed!");

    message.setText("You have changed the application theme. It needs to be restared for changes to take effect. Quit now?");
    message.setIcon(QMessageBox::Question);
    message.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    message.setDefaultButton(QMessageBox::Yes);

    int msgbox = message.exec();

    if (msgbox == QMessageBox::Yes) {
        this->close();
    }
}

// UI interaction <--

//----------

void MainWindow::load_cfg_into_ui() {
    ui.spinbox_port->setValue(config_json["port-number"]);
}

void MainWindow::connect_slots() {
    connect(this, &MainWindow::add_to_output, this, &MainWindow::handle_add_to_output);
    connect(this, &MainWindow::disable_func_buttons, this, &MainWindow::handle_disable_buttons);

    connect(ui.spinbox_port, &QSpinBox::valueChanged, this, &MainWindow::change_port);
    connect(ui.button_stop_server, &QPushButton::clicked, this, &MainWindow::stop_server);
    connect(ui.button_start_server, &QPushButton::clicked, this, &MainWindow::start_server);
    connect(ui.button_change_pswd, &QPushButton::clicked, this, &MainWindow::change_password);

    connect(ui.button_beep, &QPushButton::clicked, this, &MainWindow::beep);
    connect(ui.button_message, &QPushButton::clicked, this, &MainWindow::message);
    connect(ui.button_ss, &QPushButton::clicked, this, [&]() { client::take_screenshot(); });
    connect(ui.button_disconnect, &QPushButton::clicked, this, [&]() { client::disconnect(); });

    connect(ui.button_folder, &QPushButton::clicked, this, &MainWindow::open_folder);
    connect(ui.button_theme, &QPushButton::clicked, this, &MainWindow::switch_theme);
    connect(ui.button_clear, &QPushButton::clicked, this, [&]() { ui.text_output->clear(); });

    connect(ui.edit_command, &QLineEdit::returnPressed, this, &MainWindow::send_command);
    connect(ui.button_send, &QPushButton::clicked, this, &MainWindow::send_command);
}

//----------

void MainWindow::closeEvent(QCloseEvent* event) {
    server::stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(555));

    instance = nullptr;
    event->accept();
}
