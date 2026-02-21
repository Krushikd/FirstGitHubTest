
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

#ifdef __cplusplus
#include <vector>
#include<map>
#include <base_utils/ScopedSmPtr.hxx>
#include <sstream>
using namespace std;
using namespace Teamcenter;
extern "C" {

#include <tccore/custom.h>
#include <ict/ict_userservice.h>
#include <epm/epm.h>
#include <base_utils/mem.h>
#include <tccore/aom.h>
#include <iostream>
#include <sstream>
#include <tccore/workspaceobject.h>
#include <user_exits/epm_toolkit_utils.h>
#include <tccore/item.h>
#include <tccore/tctype.h>
#include <sa/role.h>
#include <epm/epm_task_template_itk.h>
#include <epm/signoff.h>
#include <tccore/aom_prop.h>
#include <tc/preferences.h>
#include <tccore/grm.h>
#include <ae/ae.h>
#include <tccore/aom.h>
#include <ps/ps.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tc/folder.h>
#include <res/res_itk.h>
#include <sys/types.h>
#include <Fnd0Participant/participant.h>
#include <cm/cm.h>
#include <ecm/ecm.h>
#include <tc/emh.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <epm/epm.h>
#include<epm/epm_toolkit_tc_utils.h>
#include <base_utils/TcResultStatus.hxx>
#include <TC/preferences.h>
#include <tc/emh.h>
#include <AE/dataset.h>
#include <TCCORE/aom_prop.h>


#define IL9_GENERIC_HANDLERS                            ("Il9_Generichandlers")
#define USER_INIT_MODULE                                ("USER_init_module")
#define K5_EPM_VALIDATE_CHECKIN_TARGETS                 ("EPM-Validate-Checkin")
#define K5CAR_REL                                       ("K5CarRel")
#define  K5_BUGGATTIREV                                 ("K5BugattiRevision")
#define  FROM_RELATION                                  ("from_relation")
#define  SECONDARY_OBJ                                  ("secondary_type")
#define  MANUAL                                         ("K5Manual")

#define CHECKOBJECTPROPERTIES                           "EPM-CheckObjectProperties"
#define CHECKOBJECTPROPERTESDESC                        "Property_Value_Check"
#define OBJECTTYPEARGUMENT                              "include_type"
#define OBJECTTYPE                                      "object_type"
#define PRIMARYOBJECTPROPERTY                           "property"
#define PRIMARYOBJECTVALUE                              "value"
#define RELATIONTYPE                                    "relation_type"
#define SECONDARYOBJECTTYPE                             "secondary_type"
#define SECONDARYOBJECTPROPERTY                         "secondary_property"
#define SECONDARYOBJECTVALUE                            "secondary_value"



#define IL9_CHECK_SIGNOFF_COMMENTS                      "EPM-Check-Signoff-Comments"
#define RULEOBJDESC                                     "Property value check"
#define CONTEXTRULEHANDLER                              "SignoffComment"
#define  EQUAL                                          ("=")
#define  RESULT                                         ("-result")
#define COMMENTS                                        "comments"
#define EPMDOTASK                                       "EPMDoTask"
#define EPMCONDITIONTASK                                "EPMConditionTask"

#define RULE_HANDLER_GET_PROPERTIES                     "Rule handler getproperties"
#define EPM_CHECK_PARTICIPANT_COUNT                     "EPM_Check_Particpant_Count"
#define GET_PARTICIPANTS                                "Get_Participants"
#define PARTICIPANTS                                    "participants"
#define EPM_PROPOSED_REVIEWERS                          "epm_proposed_reviewers"
#define EPM_PROPOSED_RESPONSIBLE_PARTY                  "epm_proposed_responsible_party"


#define EPM_CHECK_TARGET_ATTACHMENT                     "Check-attachmnet-target-wrkflw" 
#define FND0_WRKFLW_PROCESS                              ("fnd0MyWorkflowTasks")


#define VALIDATE_MAX_LENGTH                              "Validate_max_length_handler"
#define VALIDATE_MAX_LENGTHDESC                          "check the maximum length"
#define SECONDARYTYPEARG                                 "SecondaryType"
#define PROPERTYNAMEARG                                  "PropertyName"
#define RELATIONTYPEARG                                  "RelationType"
#define SECONDARYOBJECTTYPE                               "object_type"
#define PLM_error1 919008
#define PLM_error2 919009


#define EPM_Validate_NODUPLICATEAPPROVER                  "EPM-Validate-NoDuplicateApprover"
#define NODUPLICATEAPPROVERDESC                           "Validates for Duplicate Approver"
#define STATUS                                            "fnd0Status"
#define STATUS_STARTED                                     "Started"
#define STATUS_COMPLETED                                    "Completed"
#define STATUS_PENDING                                      "Pending"
#define STATUS_APPROVED                                   "Approved"
#define STATUS_REJECTED                                    "Rejected"
#define EPMSELECTSIGNOFFTASK                              "EPMSelectSignoffTask"
#define EPMPERFORMSIGNOFFTASK                             "EPMPerformSignoffTask"

#define EPM_MANDATORY_PARTICIPANT                         ("epm_mandatory_participant")
#define STAGE                                              ("stage")  
#define APPROVER                                           ("approver") 
#define PROPESSED_RESPONSIBLE_PARTY                       ("proposed_responsible_party")

#define VALIDATE_TARGETCLASSIFIED                           "Validate_targetObject_classified"
#define VALIDATE_TARGETCLASSIFIEDDESC                       "Validate target Object classified Description"
#define PROP_NAMECLASS                                     "ics_classified"
#define CLASSIFIEDPROPVALUE                                   "YES"
#define PLM_error3                                            919010

#define PLM_error (EMH_USER_error_base +702)
#define RULE_HANDLER_VALIDATE_ASSEMBLY_STATUS           "Rule_handler_Validate_Assembly_Status"
#define EPM_VALIDATE_ASSEMBLY_STATUS                    "EPM_Validate_Assembly_Status"
#define VALIDATE_ASSEMBLY_STATUS                        "Il9_ValidateAssemblyStatus"
#define OBJECT_TYPE                                     "object_type"
#define ITEM_REVISION                                   "ItemRevision"
#define VALIDATE_PREFERENCE                             "ValidatePreference"
#define RELEASE_STATUS_LIST                             "release_status_list"
#define ITEM_ID                                         "bl_item_item_id"
#define ITEM_REV_ID                                     "bl_rev_item_revision_id"


#define RULE_HANDLER_VALIDATE_TRANSLATED_OBJECT         "Rule_handler_Validate_Translated_Object"
#define EPM_VALIDATE_TRANSLATED_OBJECT                  "EPM_Validate_Translated_Object"
#define VALIDATE_TRANSLATED_OBJECT                      "Il9_ValidateTranslatedObject"
#define IMAN_REFERENCE                                  "IMAN_reference"
#define OBJECT_TYPE                                     "object_type"
#define PDF                                             "PDF"
#define WORD                                            "MSWordX"
#define EXCEL                                           "MSExcelX"
#define IMANFILE_NAME                                   "file_name"


#define PROJECT                                            "projects_list"
#define EPM_VALIDATETARGETPROJECT                          "EPM_ValidateTargetProject"
#define VALIDATETARGETPROJECTDESC                          "Validates Target Project"
#define EPM_VALIDATE_DATASETTYPE                           "EPM-Check-Valid-NamedReferences-Attach"
#define VALIDATE_DATASETTYPETDESC                          "EPM check Valid NamedReferences Attach"
#define DATASET                                             "Dataset"
#define PREFERENCE                                          "Datsettype_With_Named_Reference"
#define PLM_Error_3                                         919010
#define PLM_Error_4                                         919011

	extern DLLAPI int Il9_Generichandlers_register_callbacks();
	extern DLLAPI int Il9_Generichandlers_register(int* decision, va_list args);


	//registering rule handler
	EPM_decision_t Il9_ValidateTargetWithChildRev(EPM_rule_message_t msg);
	EPM_decision_t Il9_CheckObjectProperties(EPM_rule_message_t msg);
	EPM_decision_t Il9_SignoffComments(EPM_rule_message_t msg);
	EPM_decision_t Il9__Get_Participant(EPM_rule_message_t msg);
	EPM_decision_t Il9_ValidateTargetAttachment(EPM_rule_message_t msg);
	EPM_decision_t Il9_Validate_max_length(EPM_rule_message_t msg);
	EPM_decision_t Il9_Validate_NoDuplicateApprover(EPM_rule_message_t msg);
	EPM_decision_t Il9_CheckMandatoryParticipantsAttached(EPM_rule_message_t msg);
	EPM_decision_t Il9_Validate_Classified_TargetObject(EPM_rule_message_t msg);
	int Il9_ValidateAssemblyStatus(EPM_rule_message_t msg);
	EPM_decision_t Il9_ValidateTranslatedObject(EPM_rule_message_t msg);
	EPM_decision_t Il9_ValidateTargetProjects(EPM_rule_message_t msg);
	EPM_decision_t Il9_validate_datasettype_with_namedreferences(EPM_rule_message_t msg);



#define EC_TRACE_AND_LOG(function) {           \
    char *err_string;             \
    if( (iStatus = (function)) != ITK_ok)   \
    {                             \
    EMH_ask_error_text (iStatus, &err_string);                 \
	printf ("ERROR: %d ERROR MSG: %s.\n", iStatus, err_string);           \
    printf ("FUNCTION: %s\nFILE: %s LINE: %d\n",#function, __FILE__, __LINE__); \
    TC_write_syslog ("ERROR: %d ERROR MSG: %s.\n", iStatus, err_string);           \
    TC_write_syslog ("FUNCTION: %s\nFILE: %s LINE: %d\n",#function, __FILE__, __LINE__); \
    if(err_string) MEM_free(err_string);                                \
    }                                                                    \
}


}
#endif