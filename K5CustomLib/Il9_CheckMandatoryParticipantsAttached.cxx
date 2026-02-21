/**=============================================================================
									Il9_CheckMandatoryParticipantsAttached
===============================================================================
File description:

   Filename    : Il9_CheckMandatoryParticipantsAttached.cxx
   Action      : Rule Handler
   Object      : Item Revision
   Description : Validate if the target item revision object has the mandatory participants attached
@History
===============================================================================
Date                        Name
03-04-2025                  Karthik M
==================================================================================================*/


#include "Il9_Generichandlers.hxx"
#include "Il9_CheckMandatoryParticipantsAttached.hxx"

EPM_decision_t Il9_CheckMandatoryParticipantsAttached(EPM_rule_message_t msg) {
	TC_write_syslog("\n Entering %s\n", __FUNCTION__);
	int iStatus = ITK_ok;
	EPM_decision_t decision = EPM_go;
	try {
		TC_write_syslog("\n Entering %s\n", __FUNCTION__);
		int iAttachmentCnt = 0;
		tag_t tRoottask = NULLTAG;
		tag_t* tAttachmentTags = NULL;
		int iVal = 0;
		int iArgCnt = 0;
		scoped_smptr<char*> chr_argNames;
		scoped_smptr<char*> chr_argValues;
		int iValues = 0;
		int iParticipant = 0;
		scoped_smptr<tag_t>  tParticpants;
		tag_t tParticpant = NULLTAG;
		EC_TRACE_AND_LOG(EPM_args_process_args(msg.task, msg.arguments, &iArgCnt, &chr_argNames, &chr_argValues));

		TC_write_syslog("char_argnames  %s", chr_argNames.getString());
		EC_TRACE_AND_LOG(EPM_ask_root_task(msg.task, &tRoottask));
		//// Retrieve attachments of type EPM_target_attachment from the task
		EC_TRACE_AND_LOG(EPM_ask_attachments(tRoottask, EPM_target_attachment, &iAttachmentCnt, &tAttachmentTags));
		if ((tc_strcmp(chr_argNames[0], STAGE) == 0) && (tc_strcmp(chr_argValues[0], APPROVER) == 0)) {

			for (int iVal = 0; iVal < iAttachmentCnt; iVal++) {
				EC_TRACE_AND_LOG(AOM_ask_value_tags(tAttachmentTags[iVal], EPM_PROPOSED_REVIEWERS, &iParticipant, &tParticpants));
				if (tParticpants == NULL) {
					EMH_store_error(EMH_severity_error, 919703);
					decision = EPM_nogo;
				}
			}
		}
		if ((tc_strcmp(chr_argNames[0], STAGE) == 0) && (tc_strcmp(chr_argValues[0], PROPESSED_RESPONSIBLE_PARTY) == 0)) {
			for (int iVal = 0; iVal < iAttachmentCnt; iVal++) {
				EC_TRACE_AND_LOG(AOM_ask_value_tag(tAttachmentTags[iVal], EPM_PROPOSED_RESPONSIBLE_PARTY, &tParticpant));
				if (tParticpant == NULLTAG) {
					EMH_store_error(EMH_severity_error, 919703);
					decision = EPM_nogo;
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