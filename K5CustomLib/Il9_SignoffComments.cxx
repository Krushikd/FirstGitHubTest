/**=============================================================================
									Il9_SignoffComments
===============================================================================
File description:

   Action      : Rule Handler
   Object      : Item Revision
   Description : This handler checks if signoff comments are provided for EPMDoTask and EPMConditionTask.
===============================================================================
Date                        Name
19-02-2025                  Drusti P Dev
==================================================================================================*/



#include "Il9_Generichandlers.hxx"
#include "Il9_SignoffComments.hxx"


EPM_decision_t Il9_SignoffComments(EPM_rule_message_t msg)
{
	EPM_decision_t decision = EPM_go;
	int iStatus = ITK_ok;
	
	try
	{
		int iNumberOfTasks = 0;
		int iUsercount = 0;
		int iCountOfUsers = 0;
		int iArgsCnt = 0;
		char* cArg = NULL;

		tag_t tTask = msg.task;
		tag_t tThisJob = NULLTAG;
		tag_t tRootTask = NULLTAG;
		tag_t tInitiatorValue = NULLTAG;

		// Scoped smart pointers to manage memory safely
		scoped_smptr<tag_t> tTaskTags;
		scoped_smptr<tag_t> tUser;
		scoped_smptr<tag_t> tTaskUser;
		scoped_smptr<char> cValue;
		scoped_smptr<char> wValue;
		scoped_smptr<char> tValue;

		string  sHandlerArgument = "";
		string  sHandlerValue = "";



		// Get the type of task
		EC_TRACE_AND_LOG(WSOM_ask_object_type2(tTask, &wValue));

		// Check if task is of type "EPMDoTask"
		if (tc_strcmp(wValue.getString(), EPMDOTASK) == 0) {

			// Fetch comments from the task
			EC_TRACE_AND_LOG(AOM_ask_value_string(tTask, COMMENTS, &cValue));

			// Check if comments field is empty
			if (tc_strcmp(cValue.getString(), "") == 0)
			{
				// Log error and block task execution
				TC_write_syslog("\nError: The comment is mandatory: %s\n", cValue.getString());
				EMH_clear_errors();
				EMH_store_error_s1(EMH_severity_error, 919707, cValue.getString()); // Include the constructed error message
				decision = EPM_nogo;
				return decision;
			}
			else
			{
				// Log success message
				TC_write_syslog("\n comments are added. Task can proceed.\n");
			}

		}

		// Check if task is of type "EPMConditionTask"
		if (tc_strcmp(wValue.getString(), EPMCONDITIONTASK) == 0)
		{
			// Fetch handler arguments
			iArgsCnt = TC_number_of_arguments(msg.arguments);
			for (int iArg = 0; iArg < iArgsCnt; iArg++)
			{
				// Read each argument
				cArg = TC_next_argument(msg.arguments);
				if (cArg != 0)
				{
					// Split argument into key-value pair
					vector <string> vArgValues = splitString(cArg, EQUAL);
					sHandlerArgument.assign(vArgValues[0]);
					sHandlerValue.assign(vArgValues[1]);

					// Log handler argument details
					TC_write_syslog("\n Handler Argument:%s", sHandlerArgument.c_str());
					TC_write_syslog("\n Handler Argument value is :%s", sHandlerValue.c_str());

					// Check if argument key is "RESULT"
					if (tc_strcmp(sHandlerArgument.c_str(), RESULT) == 0)
					{
						// Fetch task result value
						EC_TRACE_AND_LOG(EPM_get_task_result(tTask, &tValue));

						// Compare result value with expected value
						if (tc_strcmp(tValue.getString(), sHandlerValue.c_str()) == 0)
						{
							TC_write_syslog("\n\n working correctly");

							// Fetch comments from the task
							EC_TRACE_AND_LOG(AOM_ask_value_string(tTask, COMMENTS, &cValue));

							// Check if comments field is empty
							if (tc_strcmp(cValue.getString(), "") == 0)
							{
								// Log error and block task execution
								TC_write_syslog("\nError: The comment is mandatory: %s\n", cValue.getString());
								EMH_clear_errors();
								EMH_store_error_s1(EMH_severity_error, 919707, cValue.getString()); // Include the constructed error message
								decision = EPM_nogo;
								return decision;
							}
							else
							{
								// Log success message
								TC_write_syslog("\n comments are added. Task can proceed.\n");
							}
						}
					}
				}
			}
		}
	}
	catch (const int& ex) {
		TC_write_syslog("Error in method %s: %d", __FUNCTION__, ex);
		throw ex;  // Propagate the error code
	}
	return decision;
}

vector<string> splitString(string sInputStr, string sDelimiter)
{
	size_t pos_start = 0, pos_end, delim_len = sDelimiter.length();
	string token;
	vector<string> res;
	//Spliting the strings based on delimiter
	while ((pos_end = sInputStr.find(sDelimiter, pos_start)) != string::npos)
	{
		token = sInputStr.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}
	// Return list of defined properties list in array
	res.push_back(sInputStr.substr(pos_start));
	return res;
}