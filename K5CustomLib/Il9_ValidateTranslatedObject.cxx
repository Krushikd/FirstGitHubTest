/**=============================================================================
ECM
================================================================================
File description :
Filename:  Il9_ValidateTranslatedObject.cxx
Action:    Rule Handler
Object:    Item Revision
Description :  - Validate or verify if a dataset is attached in mentioned relation with valid named reference

- Rule Handler: EPM_Validate_Translated_Object

@History
================================================================================
Date                        Name
03/04/2025           K Shreyas Pai
==================================================================================================*/
#include "Il9_Generichandlers.hxx"
#include "Il9_ValidateTranslatedObject.hxx"
EPM_decision_t Il9_ValidateTranslatedObject(EPM_rule_message_t msg) {
	int iStatus = ITK_ok;  // Initialize failure status

	try {
		int iChildCount = 0;     // Number of child lines
		int nAttachmentCnt = 0;  // Number of attachments

		// Declare smart pointers to manage memory automatically
		scoped_smptr<tag_t>tAttachments;  // Pointer for attachments
		tag_t tBOMWindow = NULLTAG;  // BOM Window handle
		tag_t tBOMTopLine = NULLTAG;  // Top-level BOM Line
		scoped_smptr<tag_t>tChildTags;  // Array to store child BOM lines
		tag_t root_task = NULLTAG; // Initialize root task to null

		EPM_decision_t decision = EPM_go; // Default decision is "Go"

		// Retrieve the root task of the workflow
		EC_TRACE_AND_LOG(EPM_ask_root_task(msg.task, &root_task));
		TC_write_syslog("Root task retrieved successfully.\n");

		// Retrieve the attachments associated with the task
		EC_TRACE_AND_LOG(EPM_ask_attachments(root_task, EPM_target_attachment, &nAttachmentCnt, &tAttachments));
		TC_write_syslog("Number of attachments found: %d\n", nAttachmentCnt);

		for (int i = 0; i < nAttachmentCnt; i++)
		{
			tag_t target_object = tAttachments[i];
			int dataset_count = 0;
			scoped_smptr<tag_t>dataset_objects;

			// Retrieve the IMAN_reference relation type as a tag_t
			tag_t relation_type = NULLTAG;
			EC_TRACE_AND_LOG(GRM_find_relation_type(IMAN_REFERENCE, &relation_type));

			if (relation_type != NULLTAG) {
				EC_TRACE_AND_LOG(GRM_list_secondary_objects_only(target_object, relation_type, &dataset_count, &dataset_objects));
				if (dataset_count == 0) {

					TC_write_syslog("Validation Failed: No dataset attached to object %u\n");

					return EPM_nogo;

				}
				else {
					bool IsValidDataset = false;
					for (int k = 0; k < dataset_count; k++) {
						scoped_smptr<char> cobjecttype;
						EC_TRACE_AND_LOG(AOM_ask_value_string(dataset_objects[k], OBJECT_TYPE, &cobjecttype));
						if (tc_strcasecmp(cobjecttype.getString(), PDF) == 0 || tc_strcasecmp(cobjecttype.getString(), WORD) == 0 || tc_strcasecmp(cobjecttype.getString(), EXCEL) == 0) {
							IsValidDataset = true;

						}
					}
					if (!IsValidDataset) {
						TC_write_syslog("Validation Failed: No valid dataset type (PDF, WORD, EXCEL) found for object %u\n");
						return EPM_nogo;
					}
				}
			}

			// Validate named reference inside the dataset
			bool valid_reference_found = false;
			for (int j = 0; j < dataset_count; j++) {

				tag_t dataset = dataset_objects[j];
				int named_ref_count = 0;
				scoped_smptr<tag_t>named_refs;
				EC_TRACE_AND_LOG(AE_ask_dataset_named_refs(dataset, &named_ref_count, &named_refs));
				if (named_ref_count > 0) {
					TC_write_syslog("Validation Failed: named reference found in dataset for object %u\n");
					return decision;
				}

				else {
					TC_write_syslog("Validation Failed: No named reference found in dataset for object %u\n");
					return EPM_nogo;
				}
			}
		}

		return decision; // Return final decision

	}
	catch (const int& ex) {
		TC_write_syslog("Error in method %s: %d", __FUNCTION__, ex);
		throw ex;  // Propagate the error code
	}

	return EPM_nogo; // Default return in case of exception
}
