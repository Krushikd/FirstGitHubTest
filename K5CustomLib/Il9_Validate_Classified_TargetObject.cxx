/**= ============================================================================
                  ECM
================================================================================
File description :

Filename: Validate_targetObject_classified.cxx

	Action      : Rule Handler
	HandlerName :Validate_targetObject_classified
	Description : - Validate if the target object is classified.

	@History
	================================================================================================
	Date                        Name

	4 - 04 - 2025            Krushik DM
	==================================================================================================*/
#include "Il9_Generichandlers.hxx"

EPM_decision_t Il9_Validate_Classified_TargetObject(EPM_rule_message_t msg)
{
	int iStatus = ITK_ok;

	EPM_decision_t decision = EPM_nogo; // Default to EPM_nogo

	try
	{
		int iArgCount = 0;
		int nAttachmentCnt = 0;
		scoped_smptr<char*> cpArgNames;
		scoped_smptr<char*> cpArgValues;
		scoped_smptr<tag_t> tAttachments;
		scoped_smptr<char> chrValue;
		// Process rule arguments
		EC_TRACE_AND_LOG(EPM_args_process_args(msg.task, msg.arguments, &iArgCount, &cpArgNames, &cpArgValues));
		// Get workflow attachments
		EC_TRACE_AND_LOG(EPM_ask_attachments(msg.task, EPM_target_attachment, &nAttachmentCnt, &tAttachments));

		if (nAttachmentCnt == 0 || tAttachments == NULL)
		{
			TC_write_syslog("\n Local Debug :: No attachments found.");
			return decision;
		}

		EC_TRACE_AND_LOG(AOM_ask_value_string(tAttachments[0], PROP_NAMECLASS, &chrValue));

		if (chrValue != NULL && strcmp(chrValue.getString(), CLASSIFIEDPROPVALUE) == 0) {
			TC_write_syslog("\n Local Debug ::The Property value is not Null.");
			return EPM_go;
		}
		else {
			TC_write_syslog("\n Local Debug ::The Property value is Null.");
			EC_TRACE_AND_LOG(EMH_store_error_s1(EMH_severity_error, PLM_error3, chrValue.getString()));
			return decision;
		}
	}

	catch (const int& ex) {
		TC_write_syslog("Error in method %s: %d", __FUNCTION__, ex);
		throw ex;  // Propagate the error code
	}

}