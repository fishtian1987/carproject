#ifndef MYSPINBOX_H
#define MYSPINBOX_H

#include <QSpinBox>

class MySpinBox:public QSpinBox
{
    Q_OBJECT
public:
    explicit MySpinBox(QWidget *parent = 0);
    ~MySpinBox();
    void setValues(int);
public slots:
    void valueChange(int);
protected:
    void keyPressEvent(QKeyEvent *);
private:
    bool keyPressFlag;
    int valuebuf;
};

#endif // MYSPINBOX_H
