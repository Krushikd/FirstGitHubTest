#pragma once
/**==========================================================================================
									Il9_Generichandlers
===============================================================================================
File description:

   Action      : Rule Handler
   Object      : Item Revision
   Description : This handler validate the Project/Program is attached to target.
================================================================================================
Date                        Name
04-04-2025                  Drusti P Dev
==================================================================================================*/

#include "Il9_Generichandlers.hxx"





EPM_decision_t Il9_ValidateTargetProjects(EPM_rule_message_t msg)
{
	EPM_decision_t decision = EPM_go;
	int iStatus = ITK_ok;


	try
	{
		TC_write_syslog("\n Entering %s\n", __FUNCTION__);
		int iAttachmentCnt = 0;
		tag_t tRoottask = NULLTAG;
		tag_t* tAttachmentTags = NULL;
		scoped_smptr<char> cValue;



		// Process arguments of the rule message task
		EC_TRACE_AND_LOG(EPM_ask_root_task(msg.task, &tRoottask));

		//// Retrieve attachments of type EPM_target_attachment from the task
		EC_TRACE_AND_LOG(EPM_ask_attachments(tRoottask, EPM_target_attachment, &iAttachmentCnt, &tAttachmentTags));
		for (int i = 0; i < iAttachmentCnt; i++) {


			EC_TRACE_AND_LOG(AOM_ask_value_string(tAttachmentTags[i], PROJECT, &cValue));

			if (tc_strcmp(cValue.getString(), " ") == 0)
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
	catch (const int& ex)
	{
		TC_write_syslog("Error in method %s: %d", __FUNCTION__, ex);
		throw ex;  // Propagate the error code
	}
	return decision;

}