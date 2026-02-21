#pragma once
/**=============================================================================
ECM
================================================================================
File description :
Filename: Il9_ValidateAssemblyStatus.hxx
Action : Rule Handler
Object : Item Revision
Description :  - Validate assembly status : throw error if immediate children have the status higher than that of the assembly

- Rule Handler: EPM_Validate_Assembly_Status
  - Preference :     Pre Release-10
					 Release Process-30
					 Pre Production-50
					 Production-100
@History
============================================================================== =
Date                        Name
19/03/2025             K Shreyas Pai
==================================================================================================*/
#include "Il9_Generichandlers.hxx"

/******************************************************************************************
* Function    :  GetStatusValue
* Description :  Retrieves the status value of an object
* Input       :  tag_t Object
* Output      :  int& ParentReleaseNumber
******************************************************************************************/
int GetStatusValue(tag_t Object, int &ParentReleaseNumber);

/******************************************************************************************
* Function    :  splitToVector
* Description :  Splits a string into a vector based on a delimiter
* Input       :  const char* input, char delimiter
* Output      :  std::vector<std::string>
******************************************************************************************/
std::vector<std::string> splitToVector(const char* input, char delimiter);