#pragma once
/**==========================================================================================
									Il9_Generichandlers
===============================================================================================
File description:

   Action      : Rule Handler
   Object      : Item Revision
   Description : This handler validate that no user is repeated as an approver.
================================================================================================
Date                        Name
19-02-2025                  Drusti P Dev
==================================================================================================*/


#include "Il9_Generichandlers.hxx"



EPM_decision_t Il9_Validate_NoDuplicateApprover(EPM_rule_message_t msg)
{
	EPM_decision_t decision = EPM_go;
	int iStatus = ITK_ok;
	

	try
	{

		tag_t tTask = msg.task;
		tag_t tThisJob = NULLTAG;
		tag_t tRootTask = NULLTAG;
		tag_t tInitiatorValue = NULLTAG;


		scoped_smptr<tag_t> tTaskTags;
		scoped_smptr<tag_t> tTaskUsers;
		scoped_smptr<tag_t> tUser;

		scoped_smptr<char> cOwningUser;
		scoped_smptr<char> cThisJob;
		scoped_smptr<char> cValue;	
		scoped_smptr<char> cUser;		
		scoped_smptr<char> cUserID;


		int iNumberOfTasks = 0;
		int iUsercount = 0;
		int iCountUsers = 0;

		// Get the type of task
		EC_TRACE_AND_LOG(EPM_ask_job(tTask, &tThisJob));

		EC_TRACE_AND_LOG(EPM_ask_root_task(tThisJob, &tRootTask));

		EC_TRACE_AND_LOG(WSOM_ask_object_type2(tTask, &cThisJob));

		if (tc_strcmp(cThisJob.getString(), EPMSELECTSIGNOFFTASK) == 0)
		{

			EC_TRACE_AND_LOG(EPM_get_type_tasks(tThisJob, eEPMSelectSignoffTask, &iNumberOfTasks, &tTaskTags));


			for (int i = 0; i < iNumberOfTasks; i++)
			{
				EC_TRACE_AND_LOG(AOM_ask_value_string(tTaskTags[i], STATUS, &cValue));

				if (strcmp(cValue.getString(), STATUS_COMPLETED) == 0 || strcmp(cValue.getString(), STATUS_PENDING) == 0)
				{
					EC_TRACE_AND_LOG(EPM_ask_task_reviewers_users(tTaskTags[i], &iUsercount, &tUser));

					for (int j = 0; j < iUsercount; j++)
					{
						EC_TRACE_AND_LOG(SA_ask_user_identifier2(tUser[j], &cUser));

						TC_write_syslog("\n Reviewer from task: %s\n", cUser.getString());


						// Get current reviewer from the task
						EC_TRACE_AND_LOG(EPM_ask_task_reviewers_users(tTask, &iCountUsers, &tTaskUsers));

						for (int k = 0; k < iCountUsers; k++)
						{

							EC_TRACE_AND_LOG(SA_ask_user_identifier2(tTaskUsers[k], &cUserID));
							TC_write_syslog("\n Reviewer from root task: %s\n", cUserID.getString());

							// Compare the identifiers
							if (tc_strcmp(cUser.getString(), cUserID.getString()) == 0)
							{

								TC_write_syslog("\nError: The selected Reviewer is already assigned as a Reviewer and can not be reupdated: %s\n", cUser.getString()); // Log the error to syslog

								// Clear any existing errors and store the new error with the message
								EMH_clear_errors();
								EMH_store_error_s1(EMH_severity_error, 919707, cUser.getString()); // Include the constructed error message
								decision = EPM_nogo; // Block the task
								return decision;
							}
							else
							{
								// If no duplicates found
								TC_write_syslog("\n No duplicate reviewers detected. Task can proceed.\n");
							}
						}
					}
				}
			}
		}

		if (tc_strcmp(cThisJob.getString(), EPMPERFORMSIGNOFFTASK) == 0)
		{
			EC_TRACE_AND_LOG(EPM_get_type_tasks(tThisJob, eEPMPerformSignoffTask, &iNumberOfTasks, &tTaskTags));

			for (int i = 0; i < iNumberOfTasks; i++)
			{
				EC_TRACE_AND_LOG(AOM_ask_value_string(tTaskTags[i], STATUS, &cValue));

				if (strcmp(cValue.getString(), STATUS_APPROVED) == 0 || strcmp(cValue.getString(), STATUS_REJECTED) == 0)
				{
					EC_TRACE_AND_LOG(EPM_ask_task_reviewers_users(tTaskTags[i], &iUsercount, &tUser));

					for (int j = 0; j < iUsercount; j++)
					{
						EC_TRACE_AND_LOG(SA_ask_user_identifier2(tUser[j], &cUser));

						TC_write_syslog("\n Reviewer from task: %s\n", cUser.getString());


						// Get current reviewer from the task
						EC_TRACE_AND_LOG(EPM_ask_task_reviewers_users(tTask, &iCountUsers, &tTaskUsers));

						for (int k = 0; k < iCountUsers; k++)
						{

							EC_TRACE_AND_LOG(SA_ask_user_identifier2(tTaskUsers[k], &cUserID));
							TC_write_syslog("\n Reviewer from root task: %s\n", cUserID.getString());

							// Compare the identifiers
							if (tc_strcmp(cUser.getString(), cUserID.getString()) == 0)
							{

								TC_write_syslog("\nError: The selected Reviewer is already assigned as a Reviewer and can not be reupdated: %s\n", cUser.getString()); // Log the error to syslog

								// Clear any existing errors and store the new error with the message
								EMH_clear_errors();
								EMH_store_error_s1(EMH_severity_error, 919707, cUser.getString()); // Include the constructed error message
								decision = EPM_nogo; // Block the task
								return decision;
							}
							else
							{
								// If no duplicates found
								TC_write_syslog("\n No duplicate reviewers detected. Task can proceed.\n");
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
