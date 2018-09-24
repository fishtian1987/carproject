#include "mylineedit.h"
#include <QKeyEvent>

MyLineEdit::MyLineEdit(QWidget *parent) :
      QLineEdit(parent)
{
    keyPressFlag=false;
    textClearFlag=false;
    upCase=false;
}

MyLineEdit::~MyLineEdit()
{

}

void MyLineEdit::keyPressEvent(QKeyEvent *event)
{
    if(keyPressFlag){
        if(!textClearFlag){
            clear();
            textClearFlag=true;
        }
        switch(event->key()){
        case Qt::Key_Escape:{
             clearFocus();
        }break;
        case Qt::Key_CapsLock:{
            upCase=!upCase;
        }break;
        case Qt::Key_Minus:{
            setText(text()+"-");
        }break;
        case Qt::Key_Comma:{
            setText(text()+",");
        }break;
        case Qt::Key_Left:{
            setCursorPosition(cursorPosition()-1);
        }break;
        case Qt::Key_Right:{
            setCursorPosition(cursorPosition()+1);
        }break;
        case Qt::Key_Delete:{
            int index=cursorPosition();
            QString str = text();
            QString str1 = str.mid(0, index);
            QString str2 = str.mid(index+1,str.length());
            setText(str1+str2);
            setCursorPosition(index);
        }break;
        case Qt::Key_Backspace:{
            QString str=text();
            str.chop(1);
            setText(str);
        }break;
        case Qt::Key_Period:{
            setText(text()+".");
        }break;
        case Qt::Key_0:case Qt::Key_1:case Qt::Key_2:case Qt::Key_3:case Qt::Key_4:
        case Qt::Key_5:case Qt::Key_6:case Qt::Key_7:case Qt::Key_8:case Qt::Key_9:{
            setText(text()+QString::number(event->key()-Qt::Key_0));
        }break;
        case Qt::Key_A:case Qt::Key_B:case Qt::Key_C:case Qt::Key_D:case Qt::Key_E:
        case Qt::Key_F:case Qt::Key_G:case Qt::Key_H:case Qt::Key_I:case Qt::Key_J:
        case Qt::Key_K:case Qt::Key_L:case Qt::Key_M:case Qt::Key_N:case Qt::Key_O:
        case Qt::Key_P:case Qt::Key_Q:case Qt::Key_R:case Qt::Key_S:case Qt::Key_T:
        case Qt::Key_U:case Qt::Key_V:case Qt::Key_W:case Qt::Key_X:case Qt::Key_Y:
        case Qt::Key_Z:{
            if(!upCase)
                setText(text()+QChar(event->key()-Qt::Key_A+'a'));
            else
                setText(text()+QChar(event->key()-Qt::Key_A+'A'));
        }break;
        default:
            break;
        }
        keyPressFlag=false;
    }else{
     keyPressFlag=true;
    }
}

