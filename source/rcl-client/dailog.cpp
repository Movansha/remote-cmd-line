#include "config.h"
#include "dialog.h"

#include <QString>

//----------

PrefDialog::PrefDialog(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.edit_ip_adress->setText(QString::fromStdString(config_json["ip-adress"]));
	ui.spinbox_port->setValue(config_json["port-number"]);
	ui.edit_password->setText(QString::fromStdString(config_json["password"]));

	connect(ui.button_ok, &QPushButton::clicked, this, &PrefDialog::set_values);
	connect(ui.button_cancel, &QPushButton::clicked, this, [&]() { this->close(); });
}

PrefDialog::~PrefDialog() {}

void PrefDialog::set_values() {
	config_json["ip-adress"] = ui.edit_ip_adress->text().toStdString();
	config_json["port-number"] = ui.spinbox_port->value();
	config_json["password"] = ui.edit_password->text().toStdString();

	write_json();
	this->close();
}
