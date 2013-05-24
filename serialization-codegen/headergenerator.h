#ifndef HEADERGENERATOR_H
#define HEADERGENERATOR_H

#include "generator.h"

class HeaderGenerator : public Generator
{
public:
    HeaderGenerator();
    void run();

private:
    void declareRecord();
};

#endif // HEADERGENERATOR_H
