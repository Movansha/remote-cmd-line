#pragma once

#include <QtWidgets/QDialog>
#include "ui_preferences.h"

//----------

class PrefDialog : public QDialog {
	Q_OBJECT

public:
	explicit PrefDialog(QWidget* parent);
	~PrefDialog();

private:
	Ui::PrefDialogClass ui;

	void set_values();
};
