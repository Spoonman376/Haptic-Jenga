//
//

#ifndef GripperTool_h
#define GripperTool_h

#include <stdio.h>
#include "PhysXUtilities.h"
#include "Tool.h"


class GripperTool : public Tool
{
private:
  
public:
  GripperTool();
  ~GripperTool();
  
  void grip();
};


#endif /* GripperTool_h */
