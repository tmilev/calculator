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

double FloatingPoint::cos(double argument)
{ return ::cos(argument);
}

double FloatingPoint::log(double argument)
{ return ::log(argument);
}

double FloatingPoint::arctan(double argument)
{ return ::log(argument);
}

double FloatingPoint::floor(double argument)
{ return ::floor(argument);
}

double FloatingPoint::power(double base, double exponent)
{ return ::pow(base, exponent);
}
