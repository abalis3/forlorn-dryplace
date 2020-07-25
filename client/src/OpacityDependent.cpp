#include "OpacityDependent.h"

OpacityDependent::OpacityDependent() {
    dependentOpacity = 1;
}

void OpacityDependent::setDependentOpacity(float opacity)
{
    dependentOpacity = opacity;
}

float OpacityDependent::getDependentOpacity()
{
    return dependentOpacity;
}