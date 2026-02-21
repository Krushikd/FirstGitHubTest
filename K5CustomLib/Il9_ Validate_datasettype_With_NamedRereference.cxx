/**=============================================================================
									Il9_ Validate_datasettype_With_NamedRereference
===============================================================================
File description:

   Filename    : Il9_ Validate_datasettype_With_NamedRereference.cxx
   Action      : Rule Handler
   Object      : Item Revision
   Description : Validate if given dataset type attached to the target ItemRevision has the valid named reference type. If no throw error.
@History
===============================================================================
Date                        Name
07-04-2025                  SYED
==================================================================================================*/
#include "Il9_Generichandlers.hxx"
#include "Il9_ Validate_datasettype_With_NamedRereference.hxx "

EPM_decision_t Il9_validate_datasettype_with_namedreferences(EPM_rule_message_t msg)
{
	EPM_decision_t decision = EPM_go;
	int iStatus = ITK_ok;

	try
	{
		int iAttachmentCnt = 0;
		tag_t *tAttachedDatasets = NULL;
		int iDatasetCount = 0;
		scoped_smptr<tag_t> tAttachments;

		EC_TRACE_AND_LOG(EPM_ask_attachments(msg.task, EPM_target_attachment, &iAttachmentCnt, &tAttachments));

		for (int iVal = 0; iVal < iAttachmentCnt; iVal++)
		{
			scoped_smptr<char> cobjectType;
			EC_TRACE_AND_LOG(WSOM_ask_object_type2(tAttachments[iVal], &cobjectType));

			if (strcmp(cobjectType.getString(), ITEM_REVISION) == 0)
			{
				EC_TRACE_AND_LOG(GRM_list_secondary_objects_only(tAttachments[iVal], NULLTAG, &iDatasetCount, &tAttachedDatasets));

				for (int iCnt = 0; iCnt < iDatasetCount; iCnt++) {

					// Check attachedDatasets are dataset by getting the class of it and then comparing class name with dataset
					tag_t tDataset = tAttachedDatasets[iCnt];
					tag_t tReference_object = NULLTAG;
					scoped_smptr<char> cDatasetType;
					scoped_smptr<char> cClass_name;
					scoped_smptr<char> cReference_name;

					int matched = 0;
					int ref_count = 0;

					AE_reference_type_t reference_type;

					// Fetch dataset type
					EC_TRACE_AND_LOG(AOM_ask_value_string(tDataset, OBJECT_TYPE, &cDatasetType));

					// Fetch class of the object
					tag_t class_id = NULLTAG;
					EC_TRACE_AND_LOG(POM_class_of_instance(tDataset, &class_id));
					EC_TRACE_AND_LOG(POM_name_of_class(class_id, &cClass_name));

					// Check if the class name is dataset
					if (strcmp(cClass_name.get(), DATASET) == 0) {
						// Fetch named reference type
						EC_TRACE_AND_LOG(AE_ask_dataset_ref_count(tDataset, &ref_count));

						for (int iRef = 0; iRef < ref_count; iRef++) {
							// Fetch each named reference
							EC_TRACE_AND_LOG(AE_find_dataset_named_ref2(tDataset, iRef, &cReference_name, &reference_type, &tReference_object));

							// Fetch object name related to reference
							scoped_smptr<char> cReference_object_name;
							EC_TRACE_AND_LOG(AOM_ask_value_string(tReference_object, IMANFILE_NAME, &cReference_object_name));

							// Extract file extension (part after the last period ".")
							const char* extension = strrchr(cReference_object_name.get(), '.');  // Change char* to const char*
							if (extension != nullptr) {
								extension++;  // Skip the period ('.')
							}
							else {
								extension = "";  // In case there is no extension
							}

							// Use PREF_ask_char_values to get preference values
							int iPref_count = 0;
							scoped_smptr<char*> cPref_values;
							EC_TRACE_AND_LOG(PREF_ask_char_values(PREFERENCE, &iPref_count, &cPref_values));

							// Flag to check if we find a match for datasetType
							bool isValid = false;

							// Iterate through all the preference values to check datasetType and reference_object_name match
							for (int p = 0; p < iPref_count; p++) {
								// Split the pref_values into key and value parts (i.e., "PDF" and "PDF.reference")

								char* pref_entry = cPref_values[p];

								// Find the position of '=' in the string to separate the key and value

								char* delimiter_pos = strchr(pref_entry, '=');
								if (delimiter_pos != nullptr) {
									// Temporarily terminate the string at the '=' to get the key part
									*delimiter_pos = '\0';

									char* key = pref_entry;
									char* value = delimiter_pos + 1; // The value is after the '='

									// Compare the datasetType (key) with the pref_values key
									if (strcmp(cDatasetType.getString(), key) == 0) {
										// If the key matches, compare the corresponding value with the file extension
										if (strcmp(extension, value) != 0) {
											// If there is a mismatch in the extension, set decision to EPM_no_go
											TC_write_syslog("Mismatch error: Dataset type '%s' reference '%s' extension '%s' does not match expected value '%s'.", cDatasetType.get(), cReference_object_name.get(), extension, value);

											EC_TRACE_AND_LOG(EMH_store_error(EMH_severity_error, PLM_Error_3));
											decision = EPM_nogo;

											return decision;
										}
										else {
											// If there is a match, set isValid to true and exit the loop
											isValid = true;
											break;
										}
									}
								}
							}

							// After checking all pref_values, if no valid match is found, throw an error
							if (!isValid) {
								TC_write_syslog("Mismatch error: Dataset type '%s' extension '%s' does not match any reference values.", cDatasetType.get(), extension);
								EC_TRACE_AND_LOG(EMH_store_error(EMH_severity_error, PLM_Error_4));
								decision = EPM_nogo;
								return decision;
							}
						}
					}
				}
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
