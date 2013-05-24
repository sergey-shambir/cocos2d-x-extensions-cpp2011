/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See LICENSE.TXT for details.
**
****************************************************************************/

/// \file recordgenerator.h
/// \brief

#ifndef RECORDGENERATOR_H
#define RECORDGENERATOR_H

#include "generator.h"

class SourceGenerator : public Generator
{
public:
    SourceGenerator();

    void run();

private:
    void defineRecord();
};

#endif // RECORDGENERATOR_H
