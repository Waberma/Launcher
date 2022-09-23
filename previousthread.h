#ifndef PREVIOUSTHREAD_H
#define PREVIOUSTHREAD_H

#include <QThread>
#include "pagination.h"
class PreviousThread : public QThread, public Pagination
{
public:
    PreviousThread(int a, int b);
    void GetNewPreviousEl(int startindex, int endindex);

};

#endif // PREVIOUSTHREAD_H
