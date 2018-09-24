#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>

class MyLineEdit:public QLineEdit
{
public:
    explicit MyLineEdit(QWidget *parent = 0);
    ~MyLineEdit();
protected:
    void keyPressEvent(QKeyEvent *);
private:
    bool keyPressFlag;
    bool textClearFlag;
    bool upCase;
};

#endif // MYLINEEDIT_H
