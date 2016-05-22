//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "vpfHeader2Math0_General.h"
#include <math.h>
ProjectInformationInstance projectInfoInstanceFloatingPoint(__FILE__, "Built in c++ floating point routines wrapper.");

double FloatingPoint::sqrt(double argument)
{ return ::sqrt(argument);
}

double FloatingPoint::sin(double argument)
{ return ::sin(argument);
}

std::string FloatingPoint::DoubleToString(double input)
{ std::stringstream out;
  out << std::fixed << input;
  std::string result=out.str();
//  stOutput << "got as input " << input << ", before resizing got: " << result << "";
  bool hasDot=false;
  for (int i=result.size()-1; i>=0; i--)
    if (result[i]=='.')
    { hasDot=true;
      break;
    }
  if (!hasDot)
    return result;
  int firstNonZeroIndex=0;
  for (firstNonZeroIndex=result.size()-1; firstNonZeroIndex>=0; firstNonZeroIndex--)
  { if (result[firstNonZeroIndex]=='.')
    { result.resize(firstNonZeroIndex);
//      result+="+\\varepsilon";
      return result;
      //firstNonZeroIndex--;
      //break;
    }
    if (result[firstNonZeroIndex]!='0')
      break;
  }
  result.resize(firstNonZeroIndex+1);
  //stOutput << "got as input " << input << ", am producing: " << result << " as output";
  return result;
}

double FloatingPoint::cos(double argument)
{ return ::cos(argument);
}

double FloatingPoint::log(double argument)
{ return ::log(argument);
}

double FloatingPoint::arctan(double argument)
{ return ::atan(argument);
}

double FloatingPoint::arccos(double argument)
{ return ::acos(argument);
}

double FloatingPoint::arcsin(double argument)
{ return ::asin(argument);
}

double FloatingPoint::floor(double argument)
{ return ::floor(argument);
}

double FloatingPoint::power(double base, double exponent)
{ return ::pow(base, exponent);
}
