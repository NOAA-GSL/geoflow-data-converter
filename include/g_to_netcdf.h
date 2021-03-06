//==============================================================================
// Date        : 4/5/21 (SG)
// Description : Writes GeoFLOW data to a NetCDF file. An input JSON property 
//               tree with dimensions, variable definitions and attributes is 
//               used to to write the NetCDF variable metadata. A collection of 
//               nodes or other data types are used to write data values. 
// Copyright   : Copyright 2021. Regents of the University of Colorado. 
//               All rights reserved.
//==============================================================================

#ifndef GTONETCDF_H
#define GTONETCDF_H

#include <map>
#include <vector>
#include <netcdf>

#include "gtypes.h"
#include "pt_util.h"
#include "gnode.h"

using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;

class GToNetCDF
{
public:
    /*!
     * Initialize the GeoFLOW to NetCDF file writer.
     * 
     * @param ptRoot root of a property tree with metadata; file format is JSON
     * @param ncFilename name of NetCDF file to write to with file extension 
     *                   (ex. myfile.nc)
     * @param mode NcFile::FileMode::read (file exists, open read-only), 
     *             NcFile::FileMode::write (file exists, open for writing), 
     *             NcFile::FileMode::replace (create new file, even if it 
     *             exists), 
     *             NcFile::FileMode::newFile (create new file, fail if already 
     *             exists)
     */
    GToNetCDF(pt::ptree& ptRoot,
              const GString& ncFilename,
              NcFile::FileMode mode);

    ~GToNetCDF() {}

    /*!
     * Convert a GeoFLOW data type to a NetCDF NcType.
     * 
     * @param gType GeoFLOW data type
     * @return the NetCDF data type
     */
    NcType toNcType(const GString& gType);

    /*!
     * Helper method that calls the NetCDF putAtt() method with the 
     * appropriate NetCDF type. The attribute's string value string gets 
     * converted to a NetCDF type.
     *
     * @param ncVar the NetCDF variable to add the attribute to
     * @param name name of the attribute
     * @param value value of the attribute
     * @param ncType the NetCDF type of the attribute
     */
    void putAttribute(const NcVar& ncVar, 
                      const GString& name,
                      const GString& value,
                      const NcType& ncType);

    /*!
     * Get the NetCDF type for the input variable.
     *
     * @param varName name of variable
     * @return the NetCDF type of the variable
     */
    NcType getVariableType(const GString& varName);

    /*!
     * Read the "dimensions" array in the property tree and write each 
     * dimension object to the NetCDF file. A dimension gets written in the 
     * form: dimName = dimValue
     */
    void writeDimensions();

    /*!
     * Iterate the "variables" array in the property tree and look for the 
     * varName variable object. Write the variable's defintion to the NetCDF 
     * file. A variable gets written in the form: 
     * varType varName(dim1, dim2, ...)
     *
     * @param varName name of variable
     */
    void writeVariableDefinition(const GString& varName);

    /*!
     * Read the "attributes" array of the varName variable object in the 
     * "variables" array of the property tree and write the variable's 
     * attributes to the NetCDF file. An attribute gets written in the form: 
     * varName:attrName = "attrValue"
     *
     * @param varName name of variable
     */
    void writeVariableAttributes(const GString& varName);

    /*!
     * Write varName's data stored in nodes to the NetCDF file.
     *
     * @param rootVarName name of a variable in the NetCDF file
     * @param fullVarName name of the variable (i.e., rootVarName.timestep) in 
     *                    the list of nodes 
     * @param nodes list of nodes that contains the variable data to write
     */
    /*template <typename T>
    void writeVariableData(const GString& rootVarName, 
                           const GString& fullVarName,
                           const vector<GNode<T>>& nodes)*/
    template <typename T>
    void writeVariableData(const GString& rootVarName, 
                           GUINT varNameIndex,
                           const vector<GNode<T>>& nodes)
    {
        Logger::info(__FILE__, __FUNCTION__, "");
        cout << "Writing NetCDF variable data from nodes for variable: "
             << rootVarName << " at at var index: " << varNameIndex << endl;

        // Get the NcVar associated with this variable
        NcVar ncVar = _nc.getVar(rootVarName);

        // Allocate memory and fill values
        T *data = new T[nodes.size()];
        for (auto i = 0u; i < nodes.size(); ++i)
        {
            data[i] = nodes[i].var(varNameIndex);
        }

        // Write the data to the NetCDF file
        ncVar.putVar(data);

        // Clean memory
        delete [] data;
        data = 0;
    }

    /*!
     * Write varName's single-valued data to the NetCDF file.
     *
     * @param varName name of a variable in the NetCDF file
     * @param varValue value of the variable data to write
     */
    template <typename T>
    void writeVariableData(const GString& varName,
                           const T& varValue)
    {
        Logger::info(__FILE__, __FUNCTION__, "");
        cout << "Writing NetCDF variable data from single-value for variable: "
             << varName << endl;

        // Get the NcVar associated with this variable
        NcVar ncVar = _nc.getVar(varName);

        // Allocate memory and fill value
        T *data = new T[1];
        data[0] = varValue;

        // Write the data to the NetCDF file
        ncVar.putVar(data);

        // Clean memory
        delete [] data;
        data = 0;
    }

    /*!
     * Write varName's data stored in values to the NetCDF file.
     *
     * @param varName name of a variable in the NetCDF file
     * @param values vector of values that contains the variable data to write
     */
    template <typename T>
    void writeVariableData(const GString& varName,
                           const vector<T>& values)
    {
        Logger::info(__FILE__, __FUNCTION__, "");
        cout << "Writing NetCDF variable data from a list of values for "
             << "variable: " << varName << endl;

        // Get the NcVar associated with this variable
        NcVar ncVar = _nc.getVar(varName);

        // Allocate memory and fill values
        T *data = new T[values.size()];
        for (auto i = 0u; i < values.size(); ++i)
        {
            data[i] = values[i];
        }

        // Write the data to the NetCDF file
        ncVar.putVar(data);

        // Clean memory
        delete [] data;
        data = 0;
    }

private:
    pt::ptree _ptRoot; // root of the property tree
    NcFile _nc;        // NetCDF file handle
};

#endif