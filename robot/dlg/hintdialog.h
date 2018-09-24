#ifndef HINTDIALOG_H
#define HINTDIALOG_H

#include <QDialog>

namespace Ui {
class HintDialog;
}

class HintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HintDialog(QWidget *parent = 0);
    ~HintDialog();    
    void setText(const QString &text);
private:
    Ui::HintDialog *ui;
};

#endif // HINTDIALOG_H
