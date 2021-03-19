//==============================================================================
// Module       : gheader_info.h
// Date         : 4/17/21 (SG)
// Description  : Read header info from a binary GeoFLOW data file
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
// Derived From : none
//==============================================================================

#ifndef GHEADERINFO_H
#define GHEADERINFO_H

#include "gtypes.h"
#include <vector>

#define DEFAULT_DATA_SIZE 8

using namespace std;

struct GHeaderInfo
{
    // Data explicitly stored in header
    GUINT         version;       // version num; exp order: 0=constant, 1=varies
    GUINT         dim;           // dimension of elements (2 or 3)
    GSIZET        nElems;        // number of elements
    vector<GUINT> polyOrder;    // poly order of each dim, array of sz dim
    GUINT         gridType;      // grid type of GeoFLOW type GElemType
    GSIZET        timeCycle;     // time cycle stamp
    GSIZET        timeStamp;     // time stamp
    GUINT         hasMultVars;   // multiple fields in file?

    // Derived header data
    GSIZET        nHeaderBytes;  // total byte size of header
    GSIZET        nDataBytes;    // total byte size of data
    GSIZET        dataSize;      // size of data value in bytes (4 or 8)
    GUINT         nNodesPerElem; // num nodes per element (incl. sub nodes)
    GSIZET        nNodes;        // total nodes in file (incl. sub nodes)
};

#endif
