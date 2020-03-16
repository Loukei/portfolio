#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent):
    QTabWidget (parent)
{

}

TabWidget::~TabWidget()
{

}

bool TabWidget::isEmpty()
{
    //* /brief: return if TabWidget have tab or not
    return count() ==0;
}
