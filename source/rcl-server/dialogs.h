#pragma once

#include <QtWidgets/QDialog>

#include "ui_change-password.h"

#include "ui_send-beep.h"
#include "ui_send-message.h"

#include <string>

//----------

extern std::string msgbox_title;
extern std::string msgbox_message;

//----------

class PsswdDialog : public QDialog {
	Q_OBJECT

public:
	explicit PsswdDialog(QWidget* parent);
	~PsswdDialog();

private slots:
	void set_password();

private:
	Ui::PsswdDialogClass ui;
};

//----------

class BeepDialog : public QDialog {
	Q_OBJECT

public:
	explicit BeepDialog(QWidget* parent);
	~BeepDialog();

private slots:
	void set_values();

private:
	Ui::BeepDialogClass ui;
};

//----------

class MsgDialog : public QDialog {
	Q_OBJECT

public:
	explicit MsgDialog(QWidget* parent);
	~MsgDialog();

private slots:
	void set_values();

private:
	Ui::MsgDialogClass ui;
};
