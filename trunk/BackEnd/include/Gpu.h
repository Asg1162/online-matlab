#pragma once

// GPU support definitions are here
#include <cula.hpp>

int initGpu(int);

void finalizeGpu();


void checkStatus(culaStatus status);
