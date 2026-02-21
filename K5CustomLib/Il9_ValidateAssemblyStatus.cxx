/**=============================================================================
ECM
================================================================================
File description :
Filename: Il9_ValidateAssemblyStatus.cxx
Action : Rule Handler
Object : Item Revision
Description :  - Validate assembly status : throw error if immediate children have the status higher than that of the assembly

- Rule Handler: EPM_Validate_Assembly_Status
  - Preference :     Pre Release-10
					 Release Process-30
					 Pre Production-50
					 Production-100
@History
============================================================================== =
Date                        Name
19/03/2025             K Shreyas Pai
==================================================================================================*/
#include "Il9_Generichandlers.hxx"
#include "Il9_ValidateAssemblyStatus.hxx"
int Il9_ValidateAssemblyStatus(EPM_rule_message_t msg) {
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
			scoped_smptr<char> cobjecttype;
			EC_TRACE_AND_LOG(AOM_ask_value_string(tAttachments[i], OBJECT_TYPE, &cobjecttype));
			if (tc_strcmp(cobjecttype.getString(), "") == 0 || tc_strcmp(cobjecttype.getString(), ITEM_REVISION) != 0)
			{
				continue;
			}
			TC_write_syslog("Processing attachment index: %d\n", i);

			int ParentReleaseNumber = 0;
			GetStatusValue(tAttachments[i], ParentReleaseNumber);
			if (ParentReleaseNumber == 0)
			{
				TC_write_syslog("Parent release number is zero. Returning EPM_nogo.\n");
				return EPM_nogo;
			}

			// Create BOM Windown
			EC_TRACE_AND_LOG(BOM_create_window(&tBOMWindow));
			TC_write_syslog("BOM window created successfully.\n");

			// Set the BOM top line (assuming tAttachments is already assigned a valid BOM line)
			EC_TRACE_AND_LOG(BOM_set_window_top_line(tBOMWindow, NULLTAG, tAttachments[0], NULLTAG, &tBOMTopLine));
			// Get child lines of the BOM top line
			EC_TRACE_AND_LOG(BOM_line_ask_all_child_lines(tBOMTopLine, &iChildCount, &tChildTags));
			TC_write_syslog("Child count in BOM: %d\n", iChildCount);

			// Check if there are child lines
			if (iChildCount > 0) {
				for (int k = 0; k < iChildCount; k++) {
					int childValue = 0;
					scoped_smptr<char> cItemId;
					scoped_smptr<char> cRevId;
					EC_TRACE_AND_LOG(AOM_ask_value_string(tChildTags[k], ITEM_ID, &cItemId));
					EC_TRACE_AND_LOG(AOM_ask_value_string(tChildTags[k], ITEM_REV_ID, &cRevId));
					if (tc_strcmp(cItemId.getString(), "") == 0 || tc_strcmp(cRevId.getString(), "") == 0)
					{
						continue;
					}
					tag_t tRevision = NULLTAG;
					EC_TRACE_AND_LOG(ITEM_find_rev(cItemId.getString(), cRevId.getString(), &tRevision));
					if (tRevision == NULLTAG)
					{
						continue;
					}
					GetStatusValue(tRevision, childValue);
					if (childValue >= ParentReleaseNumber)
					{
						iStatus = PLM_error;
						EC_TRACE_AND_LOG(EMH_clear_errors());
						EC_TRACE_AND_LOG(EMH_store_error(EMH_severity_error, PLM_error));
						TC_write_syslog("Error: Child release status is greater than or equal to parent.\n");
						return iStatus;
					}
				}
			}
			// Close BOM Window to release resources
			EC_TRACE_AND_LOG(BOM_close_window(tBOMWindow));
			TC_write_syslog("BOM window closed.\n");
		}

		return decision; // Return final decision
	}
	catch (const int& ex) {
		TC_write_syslog("Error in method %s: %d", __FUNCTION__, ex);
		throw ex;  // Propagate the error code
	}

	return EPM_nogo; // Default return in case of exception
}


int GetStatusValue(tag_t Object, int &ParentReleaseNumber)
{
	int iStatus = ITK_ok;  // Initialize failure status
	try
	{
		int nPreferenceCount = 0;
		scoped_smptr<char*> cPrefrenceValues;
		EC_TRACE_AND_LOG(PREF_ask_char_values(VALIDATE_PREFERENCE, &nPreferenceCount, &cPrefrenceValues));
		TC_write_syslog("Number of preference values retrieved: %d\n", nPreferenceCount);
		if (nPreferenceCount == 0 || cPrefrenceValues == NULL)
		{
			return iStatus;
		}
		int nReleaseStatusNumber = 0;
		scoped_smptr<tag_t> cReleaseValue;
		EC_TRACE_AND_LOG(AOM_ask_value_tags(Object, RELEASE_STATUS_LIST, &nReleaseStatusNumber, &cReleaseValue));
		TC_write_syslog("Number of release statuses found: %d\n", nReleaseStatusNumber);
		if (nReleaseStatusNumber == 0 || cReleaseValue == NULL || cReleaseValue[0] == NULLTAG)
		{
			return iStatus;
		}
		scoped_smptr<char> cReleaseStatusName;
		EC_TRACE_AND_LOG(RELSTAT_ask_release_status_type(cReleaseValue[0], &cReleaseStatusName));
		TC_write_syslog("Release status name: %s\n", cReleaseStatusName.getString());
		if (tc_strcmp(cReleaseStatusName.getString(), "") == 0)
		{
			return iStatus;
		}
		for (int j = 0; j < nPreferenceCount; j++)
		{
			if (tc_strcmp(cPrefrenceValues[j], "") != 0)
			{
				// Convert scoped_smptr to standard char pointer
				char* preferenceValue = cPrefrenceValues[j];
				vector<string> StatusPrefValue = splitToVector(preferenceValue, '-');

				// Check if preferenceValue matches cReleaseStatusName
				if (tc_strcmp(StatusPrefValue[0].c_str(), cReleaseStatusName.getString()) == 0)
				{
					ParentReleaseNumber = atoi(StatusPrefValue[1].c_str());
				}
			}
		}
	}
	catch (const int& ex) {
		TC_write_syslog("Error in method %s: %d", __FUNCTION__, ex);
		throw ex;  // Propagate the error code
	}
	return iStatus;
}


std::vector<std::string> splitToVector(const char* input, char delimiter) {
	std::vector<std::string> tokens;  // Vector to store results
	std::string str(input);  // Convert char* to std::string
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);  // Store each part in vector
	}

	return tokens;
}