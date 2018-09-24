#include "hintdialog.h"
#include "ui_hintdialog.h"
#include "utils/utils.h"
#include <QDesktopWidget>

HintDialog::HintDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HintDialog)
{
    ui->setupUi(this);
    setWindowTitle("系统提示");
    Utils::formInCenter(this,1);
    QObject::connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(close()));
}

HintDialog::~HintDialog()
{
    delete ui;
}

void HintDialog::setText(const QString &text)
{
    ui->label->setText(text);
}
