
/**=============================================================================
								   Generic Workflow Handlers
===============================================================================
File description:

   Filename: Il9_Generichandlers.hxx

@History
===============================================================================
Date                          Name                      Description
19-02-2025                    Karthik M                 Initial Creation
===============================================================================*/


#include "Il9_Generichandlers.hxx"

int iStatus = ITK_ok;



 int Il9_Generichandlers_register_callbacks()
{
	CUSTOM_register_exit(IL9_GENERIC_HANDLERS, USER_INIT_MODULE, (CUSTOM_EXIT_ftn_t)Il9_Generichandlers_register);

	TC_write_syslog("\n\n*****************Il9_Generichandlers Registration successful.***********************\n");

	return(ITK_ok);
}


extern DLLAPI int Il9_Generichandlers_register(int* decision, va_list args)
{
	*decision = ALL_CUSTOMIZATIONS;
	//regestering rule handler
	EC_TRACE_AND_LOG(EPM_register_rule_handler(K5_EPM_VALIDATE_CHECKIN_TARGETS, K5_EPM_VALIDATE_CHECKIN_TARGETS, (EPM_rule_handler_t)Il9_ValidateTargetWithChildRev));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(CHECKOBJECTPROPERTIES, RULEOBJDESC, (EPM_rule_handler_t)Il9_CheckObjectProperties));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(IL9_CHECK_SIGNOFF_COMMENTS, RULEOBJDESC, (EPM_rule_handler_t)Il9_SignoffComments));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(EPM_CHECK_PARTICIPANT_COUNT, RULE_HANDLER_GET_PROPERTIES, (EPM_rule_handler_t)Il9__Get_Participant));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(EPM_CHECK_TARGET_ATTACHMENT, EPM_CHECK_TARGET_ATTACHMENT, (EPM_rule_handler_t)Il9_ValidateTargetAttachment));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(VALIDATE_MAX_LENGTH, VALIDATE_MAX_LENGTHDESC, (EPM_rule_handler_t)Il9_Validate_max_length));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(EPM_Validate_NODUPLICATEAPPROVER, NODUPLICATEAPPROVERDESC, (EPM_rule_handler_t)Il9_Validate_NoDuplicateApprover));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(EPM_MANDATORY_PARTICIPANT, EPM_MANDATORY_PARTICIPANT, (EPM_rule_handler_t)Il9_CheckMandatoryParticipantsAttached));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(VALIDATE_TARGETCLASSIFIED, VALIDATE_TARGETCLASSIFIEDDESC, (EPM_rule_handler_t)Il9_Validate_Classified_TargetObject));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(EPM_VALIDATE_ASSEMBLY_STATUS, RULE_HANDLER_VALIDATE_ASSEMBLY_STATUS, (EPM_rule_handler_t)Il9_ValidateAssemblyStatus));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(EPM_VALIDATE_TRANSLATED_OBJECT, RULE_HANDLER_VALIDATE_TRANSLATED_OBJECT, (EPM_rule_handler_t)Il9_ValidateTranslatedObject));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(EPM_VALIDATETARGETPROJECT, VALIDATETARGETPROJECTDESC, (EPM_rule_handler_t)Il9_ValidateTargetProjects));
	EC_TRACE_AND_LOG(EPM_register_rule_handler(EPM_VALIDATE_DATASETTYPE, VALIDATE_DATASETTYPETDESC, (EPM_rule_handler_t)Il9_validate_datasettype_with_namedreferences));
	return iStatus;

}

