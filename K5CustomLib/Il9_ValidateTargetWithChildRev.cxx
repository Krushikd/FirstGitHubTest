/**=============================================================================
									Il9_ValidateTargetWithChildRev
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
#include "Il9_ValidateTargetWithChildRev.hxx"

EPM_decision_t Il9_ValidateTargetWithChildRev(EPM_rule_message_t msg) {
	TC_write_syslog("\n Entering %s\n", __FUNCTION__);
	int iStatus = ITK_ok;
	EPM_decision_t decision = EPM_go;
	try {
		TC_write_syslog("\n Entering %s\n", __FUNCTION__);
		int iAttachmentCnt = 0;
		tag_t tRoottask = NULLTAG;
		tag_t* tAttachmentTags = NULL;
		scoped_smptr<char> cTargetObjType;
		int iVal = 0;
		int iArgCnt = 0;
		scoped_smptr<char*> chr_argNames;
		scoped_smptr<char*> chr_argValues;
		tag_t  tCarRel = NULLTAG;
		int iValues = 0;
		EC_TRACE_AND_LOG(EPM_args_process_args(msg.task, msg.arguments, &iArgCnt, &chr_argNames, &chr_argValues));
		
		TC_write_syslog("char_argnames  %s", chr_argNames.getString());
		if ((tc_strcmp(chr_argNames[0], FROM_RELATION) == 0) || (tc_strcmp(chr_argNames[0], SECONDARY_OBJ) == 0) ||
			(tc_strcmp(chr_argNames[1], FROM_RELATION) == 0) || (tc_strcmp(chr_argNames[1], SECONDARY_OBJ) == 0)) {
				// Process arguments of the rule message task
				EC_TRACE_AND_LOG(EPM_ask_root_task(msg.task, &tRoottask));
				//// Retrieve attachments of type EPM_target_attachment from the task
				EC_TRACE_AND_LOG(EPM_ask_attachments(tRoottask, EPM_target_attachment, &iAttachmentCnt, &tAttachmentTags));

				if (iAttachmentCnt > 0) {
					EC_TRACE_AND_LOG(WSOM_ask_object_type2(tAttachmentTags[iVal], &cTargetObjType)); //get the object type

					if (cTargetObjType != NULL && (tc_strcmp(cTargetObjType.getString(), K5_BUGGATTIREV) == 0)) {
						for (iVal = 0; iVal < iAttachmentCnt; iVal++) {   //iterating through each argument 
							//calling the method
							if ((tc_strcmp(chr_argValues[0], K5CAR_REL) == 0 || (tc_strcmp(chr_argValues[1], MANUAL)))) {
								int valStatus = validtationTargetChild(tAttachmentTags[iVal]);
								if (valStatus == 919701) {
									EMH_store_error(EMH_severity_error, 919701);
									decision = EPM_nogo;
								}
							}

						}

					}
					else {
						TC_write_syslog("\n Object is not matching");
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

int validtationTargetChild(tag_t tItemRev) {
	int iStatus = ITK_ok;
	EPM_decision_t decision = EPM_go;
	try {
		TC_write_syslog("\n Entering validtationTargetChild method %s\n", __FUNCTION__);
		logical lisCheck_out = 0;
		tag_t  tCarRel = NULLTAG;
		int iSecondaryObjCnt = 0;
		tag_t* tManualObj = NULL;
		bool bErrorPresent = false;
		//checking object is checked_out or not 
		EC_TRACE_AND_LOG(RES_is_checked_out(tItemRev, &lisCheck_out));
		if (!lisCheck_out) {
			logical lSec_is_checked_out = 0;
			//getting the relation type 
			EC_TRACE_AND_LOG(GRM_find_relation_type(K5CAR_REL, &tCarRel));
			//getting lst of all Secondary object tag
			EC_TRACE_AND_LOG(GRM_list_secondary_objects_only(tItemRev, tCarRel, &iSecondaryObjCnt, &tManualObj));
			//iterating through each object 
			for (int iSecObj = 0; iSecObj < iSecondaryObjCnt; iSecObj++) {
				//checking secondary object is checked_out or not
				EC_TRACE_AND_LOG(RES_is_checked_out(tManualObj[iSecObj], &lSec_is_checked_out));
				if (lSec_is_checked_out) {
					bErrorPresent = true;  //if obj is checked_out set the value to true
				}
			}
			
		}
		else {
           bErrorPresent = true;
		}

		if (bErrorPresent) {
			iStatus = 919701;

		}
		else {
			iStatus = ITK_ok;
		}

	}
	catch (const int& ex) {
		TC_write_syslog("Error in method %s: %d", __FUNCTION__, ex);
		throw ex;  // Propagate the error code
	}
	return iStatus;
}