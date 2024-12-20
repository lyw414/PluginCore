//#include "TaskPool/TaskPool.h"
//#include "NodeList/NodeList.h"

#include "TaskPool/Thread.h"

#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>


namespace LYW_PLUGIN_CORE
{
    class A : public Thread
    {
        public:
            A() : Thread(2, 8)
            {
                srand(time(NULL));
            }
            
            virtual int32 TaskResourceAssessment()
            {
                return 0;
            }

            virtual pvoid WaitTask(int32 timeout)
            {
                usleep((uint32)rand() % 50000);
                pvoid t = (pvoid)((uint64)rand()%50000);
                if (timeout != 0)
                {
                    //sleep(100000);
                }
                return t;
            }

            virtual void ExcuteTask(pvoid taskNode)
            {
                uint64 t = (uint64)taskNode;
                usleep(t);
            }
    };
}

int main()
{
    LYW_PLUGIN_CORE::A a;

    while (true)
    {
        usleep(100000);
        a.Daemon();
    }

    return 0;
}
