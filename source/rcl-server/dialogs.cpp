#include "config.h"
#include "dialogs.h"

#include <QString>
#include <string>

//----------

std::string msgbox_title;
std::string msgbox_message;

//----------

PsswdDialog::PsswdDialog(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);
	ui.edit_password->setText(QString::fromStdString(config_json["password"]));

	connect(ui.button_ok, &QPushButton::clicked, this, &PsswdDialog::set_password);
	connect(ui.button_cancel, &QPushButton::clicked, this, [&]() { this->close(); });
}

PsswdDialog::~PsswdDialog() {}

void PsswdDialog::set_password() {
	config_json["password"] = ui.edit_password->text().toStdString();
	write_json();

	this->close();
}

//----------

BeepDialog::BeepDialog(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);
	ui.spinbox_freq->setValue(config_json["beep-freq"]);
	ui.spinbox_duration->setValue(config_json["beep-duration"]);

	connect(ui.button_ok, &QPushButton::clicked, this, &BeepDialog::set_values);
	connect(ui.button_cancel, &QPushButton::clicked, this, [&]() { this->reject(); });
}

BeepDialog::~BeepDialog() {}

void BeepDialog::set_values() {
	config_json["beep-freq"] = ui.spinbox_freq->value();
	config_json["beep-duration"] = ui.spinbox_duration->value();
	write_json();

	this->accept();
}

//----------

MsgDialog::MsgDialog(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);

	connect(ui.button_ok, &QPushButton::clicked, this, &MsgDialog::set_values);
	connect(ui.button_cancel, &QPushButton::clicked, this, [&]() { this->reject(); });
}

MsgDialog::~MsgDialog() {}

void MsgDialog::set_values() {
	msgbox_title = ui.edit_title->text().toStdString();
	msgbox_message = ui.textedit_message->toPlainText().toStdString();

	this->accept();
}
