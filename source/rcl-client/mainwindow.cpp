#include "config.h"
#include "client.h"

#include "mainwindow.h"
#include "dialog.h"

#include <QString>

//*****

#include <QIcon>
#include <QSize>
#include <QMessageBox>

#include <QCloseEvent>
#include <chrono>

//----------

MainWindow* MainWindow::instance = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    instance = this;

    ui.text_output->append("Check \"Connection preferences\"" "\n for server password and similar settings." "\n");

    MainWindow::connect_slots();
    emit MainWindow::disable_disconnect(true);

    if (config_json["auto-connect"] == true) MainWindow::connect_to_server();
}

MainWindow::~MainWindow() {}

//----------

void MainWindow::handle_add_to_output(const QString& text) {
    ui.text_output->append(text);
}

void MainWindow::handle_disable_disconnect(const bool& status) {
    if (status == true) {
        ui.button_connect->setDisabled(false);
        ui.button_preferences->setDisabled(false);
    }

    else {
        ui.button_connect->setDisabled(true);
        ui.button_preferences->setDisabled(true);
    }

    ui.button_disconnect->setDisabled(status);
}

//----------

// UI interaction -->

void MainWindow::switch_auto_connect(const bool& status) {
    config_json["auto-connect"] = status;
    if (!write_json()) MainWindow::load_cfg_into_ui();
}

void MainWindow::change_preferences() {
    PrefDialog preferences_dialog(this);

    preferences_dialog.exec();
}

void MainWindow::connect_to_server() {
    emit MainWindow::disable_disconnect(false);

    QString qstr_adress = QString::fromStdString(config_json["ip-adress"]);
    QString qstr_port = QString::fromStdString(std::to_string(int(config_json["port-number"])));
    QString qstr_password = QString::fromStdString(config_json["password"]);

    ui.text_output->append("----------");
    ui.text_output->append("Adress: " + qstr_adress);
    ui.text_output->append("Port number: " + qstr_port);
    ui.text_output->append("Password: " + qstr_password);
    ui.text_output->append("----------" "\n");

    //*****

    client::connect();

    ui.text_output->append("[+] Preparing the connection...");
}

void MainWindow::disconnect_from_server() {
    client::disconnect();
    ui.text_output->append("[-] Disconnected by the user." "\n");

    emit MainWindow::disable_disconnect(true);
}

void MainWindow::switch_dark_mode(const bool& status) {
    config_json["dark-mode"] = status;
    if (!write_json()) return;

    QMessageBox message;

    message.setWindowIcon(QIcon(":/icons/resources/rcl-client.ico"));
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
    ui.checkbox_auto->setChecked(config_json["auto-connect"]);
    ui.checkbox_dark->setChecked(config_json["dark-mode"]);
}

void MainWindow::connect_slots() {
    connect(this, &MainWindow::add_to_output, this, &MainWindow::handle_add_to_output);
    connect(this, &MainWindow::disable_disconnect, this, &MainWindow::handle_disable_disconnect);

    connect(ui.button_clear, &QPushButton::clicked, this, [&]() { ui.text_output->clear(); });

    connect(ui.checkbox_auto, &QCheckBox::clicked, this, &MainWindow::switch_auto_connect);
    connect(ui.button_preferences, &QPushButton::clicked, this, &MainWindow::change_preferences);

    connect(ui.button_connect, &QPushButton::clicked, this, &MainWindow::connect_to_server);
    connect(ui.button_disconnect, &QPushButton::clicked, this, &MainWindow::disconnect_from_server);

    connect(ui.checkbox_dark, &QCheckBox::clicked, this, &MainWindow::switch_dark_mode);
}

//----------

void MainWindow::closeEvent(QCloseEvent* event) {
    client::disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(555));

    instance = nullptr;
    event->accept();
}
