#include "myspinbox.h"
#include <QKeyEvent>
#include <QDebug>

MySpinBox::MySpinBox(QWidget *parent):
    QSpinBox(parent)
{

}

MySpinBox::~MySpinBox()
{

}

void MySpinBox::valueChange(int data)
{
    valuebuf=data;
}

void MySpinBox::setValues(int data)
{
  setValue(data);
  valuebuf=0;
}

void MySpinBox::keyPressEvent(QKeyEvent *event)
{
    if(keyPressFlag){
        switch(event->key()){
        case Qt::Key_Escape:{
             clearFocus();
        }break;
        case Qt::Key_Delete:
        case Qt::Key_Backspace:{
           clear();
           valuebuf=0;
        }break;
        case Qt::Key_0:case Qt::Key_1:case Qt::Key_2:case Qt::Key_3:case Qt::Key_4:
        case Qt::Key_5:case Qt::Key_6:case Qt::Key_7:case Qt::Key_8:case Qt::Key_9:{
            int temp=valuebuf*10+event->key()-Qt::Key_0;
            if(temp>99)
                break;
            setValue(temp);
        }break;
        default:
            break;
        }
        keyPressFlag=false;
    }else{
     keyPressFlag=true;
    }
}
