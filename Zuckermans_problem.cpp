#include "polyhedra.h"

void getZuckermansArrayE8(roots& output)
{ output.MakeActualSizeAtLeastExpandOnTop(70);
  root r;
  r.InitFromIntegers(8, 1,0,0,0,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,0,0,0,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,1,0,0,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,0,0,0,0,0,
                                1); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,0,0,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 1,0,0,0,0,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,1,0,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,0,0,0,1,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,0,0,0,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,1,0,0,0,0,1,
                                0); output.AddObjectOnTop(r);

  r.InitFromIntegers(8, 2,0,0,0,0,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,0,1,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 1,0,1,0,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,2,0,0,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 1,0,0,0,0,1,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,1,0,0,0,0,0,
                                1); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,1,0,0,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,0,0,1,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,0,0,0,0,0,
                                2); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,0,0,0,0,2,
                                0); output.AddObjectOnTop(r);

  r.InitFromIntegers(8, 0,0,0,1,0,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 1,0,1,0,0,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 1,0,0,0,1,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,2,0,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,1,0,0,1,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,0,0,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,1,0,0,0,0,0,
                                1); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,1,0,0,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,1,0,0,1,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,0,0,1,0,0,
                                0); output.AddObjectOnTop(r);

  r.InitFromIntegers(8, 2,0,0,0,0,0,0,
                                2); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 1,0,1,0,0,1,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 1,0,1,0,0,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 1,0,0,0,1,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,2,0,0,0,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,1,0,1,0,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,1,0,0,0,1,0,
                                1); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,1,0,1,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,0,2,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,0,2,0,0,0,2,
                                0); output.AddObjectOnTop(r);

  r.InitFromIntegers(8, 0,0,1,0,1,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,0,0,0,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,1,0,1,0,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,1,0,0,0,1,0,
                                1); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,1,0,1,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,0,2,0,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 1,0,1,0,1,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,1,1,0,1,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,2,0,0,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,1,0,1,0,1,
                                0); output.AddObjectOnTop(r);

  r.InitFromIntegers(8, 2,0,0,0,2,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,1,0,0,0,1,2,
                                1); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,1,0,1,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,0,2,0,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,2,0,0,2,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,2,0,0,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,1,0,1,0,1,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,0,1,0,1,0,
                                1); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,0,0,2,0,0,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,0,0,1,1,1,
                                0); output.AddObjectOnTop(r);

  r.InitFromIntegers(8, 1,0,1,1,0,1,2,
                                1); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 0,2,0,0,2,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,0,1,0,1,2,
                                1); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,0,0,2,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,0,2,0,2,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,2,1,0,1,2,
                                1); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,2,0,2,0,2,
                                0); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,0,2,0,2,2,
                                2); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,2,2,0,2,2,
                                2); output.AddObjectOnTop(r);
  r.InitFromIntegers(8, 2,2,2,2,2,2,2,
                                2); output.AddObjectOnTop(r);


  root tempRoot; tempRoot.SetSizeExpandOnTopLight(8);
  for(int i=0;i<output.size;i++)
  { tempRoot.TheObjects[0]=output.TheObjects[i].TheObjects[6];
    tempRoot.TheObjects[1]=output.TheObjects[i].TheObjects[7];
    tempRoot.TheObjects[2]=output.TheObjects[i].TheObjects[5];
    tempRoot.TheObjects[3]=output.TheObjects[i].TheObjects[4];
    tempRoot.TheObjects[4]=output.TheObjects[i].TheObjects[3];
    tempRoot.TheObjects[5]=output.TheObjects[i].TheObjects[2];
    tempRoot.TheObjects[6]=output.TheObjects[i].TheObjects[1];
    tempRoot.TheObjects[7]=output.TheObjects[i].TheObjects[0];
    output.TheObjects[i].Assign(tempRoot);
  }

}
