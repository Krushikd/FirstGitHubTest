/**=============================================================================
									Il9_ValidateTargetAttachment
===============================================================================
File description:

   Filename    : Il9_ValidateTargetWithChildRev.cxx
   Action      : Rule Handler
   Object      : Item Revision
   Description : The handler checks Iteam and all the secondary objects are checked in
@History
===============================================================================
Date                        Name
19-02-2025                  Karthik M
==================================================================================================*/


#include "Il9_Generichandlers.hxx"
#include "Il9_ValidateTargetAttachment.hxx"

EPM_decision_t Il9_ValidateTargetAttachment(EPM_rule_message_t msg)
{   
  int 	iStatus = ITK_ok;
  EPM_decision_t decision = EPM_go;
	try {
		TC_write_syslog("\n Entering %s\n", __FUNCTION__);
		int iAttachmentCnt = 0;
		tag_t tRoottask = NULLTAG;
		tag_t* tAttachmentTags = NULL;
		scoped_smptr<char> cTargetObjType;
		scoped_smptr <char*> cFnd0Wrkflw;
		tag_t* tFnd0Wrkflw;
		int iVal = 0;
		int iArgCnt = 0; 
		int iWrkflwCnt = 0;
		// Process arguments of the rule message task
		EC_TRACE_AND_LOG(EPM_ask_root_task(msg.task, &tRoottask));
		//// Retrieve attachments of type EPM_target_attachment from the task
		EC_TRACE_AND_LOG(EPM_ask_attachments(tRoottask, EPM_target_attachment, &iAttachmentCnt, &tAttachmentTags));
		for (int iVal = 0; iVal < iAttachmentCnt; iVal++) {
			//EC_TRACE_AND_LOG(AOM_ask_value_strings(tAttachmentTags[iVal], FND0_WRKFLW_PROCESS,&iWrkflwCnt, &cFnd0Wrkflw)); 
			EC_TRACE_AND_LOG(AOM_ask_value_tags(tAttachmentTags[iVal], FND0_WRKFLW_PROCESS, &iWrkflwCnt, &tFnd0Wrkflw));
			TC_write_syslog("\n Value of Fnd0WorkflowProcess : %s \n", cFnd0Wrkflw.getString());
			if (tFnd0Wrkflw !=NULL) {
				EMH_store_error(EMH_severity_error, 919702);
				decision = EPM_nogo;
			}
		}

	}
	catch (const int& ex) {
		TC_write_syslog("Error in method %s: %d", __FUNCTION__, ex);
		throw ex;  // Propagate the error code
	}
	return decision;
}