#ifndef COMBINE_TASK_H
#define COMBINE_TASK_H

#include <cmath>
#include <assert.h>

class CombineTask
{
 public:
  int index1;
  int index2;

  int status;

  CombineTask(int index1, int index2) 
    {
      assert(index1 != index2);
      
      this->index1 = fmin(index1, index2);
      this->index2 = fmax(index1, index2);
      
      this->status = 0; //0: not ready, 1:one of the indexes is ready, 2:both indexes are ready, 3:booked or done
    }

  bool hasIndex(int index)
  {
    return ( (this->index1 == index)||
	     (this->index2 == index)
	     );
  }
};


#endif
