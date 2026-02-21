/**=============================================================================
ECM
================================================================================
File description :
Filename: Il9_Get_Participant.cxx
Action : Rule Handler
Object : Item Revision
Description : The handler checks if at least one participant is assigned.
Ensures the participant count matches the given input(default count: 4) or more than 4.
@History
============================================================================== =
Date                        Name
19 - 02 - 2025                  K Shreyas Pai
==================================================================================================*/
#include "Il9_Generichandlers.hxx"
#include "Il9__Get_Participant.hxx"
EPM_decision_t Il9__Get_Participant(EPM_rule_message_t msg) {
	int iStatus = ITK_ok;  // Initialize failure status

	try {
		int iArgCount = 0;  // Initialize argument count
		int nNumber = 0;    // Initialize number of participants
		int nAttachmentCnt = 0; // Number of attachments
		int checkcount = 0; // Counter to check participant requirement

		// Declare smart pointers to manage memory automatically
		scoped_smptr<char*> cpargNames;     // Pointer for argument names
		scoped_smptr<char*> cpargValues;    // Pointer for argument values
		scoped_smptr<tag_t> tAttachments;   // Pointer for attachments
		tag_t tParticpant = NULLTAG;    // Pointer for responsible participant
		scoped_smptr<tag_t> tParticpants;   // Pointer to hold participants

		EPM_decision_t decision = EPM_nogo; // Default decision is "No-Go"
		tag_t root_task = NULLTAG;          // Initialize root task to null

		// Retrieve the root task of the workflow
		EC_TRACE_AND_LOG(EPM_ask_root_task(msg.task, &root_task));

		// Retrieve the attachments associated with the task
		EC_TRACE_AND_LOG(EPM_ask_attachments(msg.task, EPM_target_attachment, &nAttachmentCnt, &tAttachments));

		// Process arguments received from the workflow handler
		EC_TRACE_AND_LOG(EPM_args_process_args(msg.task, msg.arguments, &iArgCount, &cpargNames, &cpargValues));

		// Define an array to store participant values from arguments
		const int MAX_VALUES = 10;
		char* participantvalues[MAX_VALUES];
		int count = 0;

		// Split the second argument (comma-separated participants) into tokens
		char* token = strtok(cpargValues[1], ",");
		while (token != nullptr && count < MAX_VALUES) {
			participantvalues[count] = new char[strlen(token) + 1]; // Allocate memory
			strcpy(participantvalues[count], token); // Copy token into allocated memory
			count++;
			token = strtok(nullptr, ","); // Get next token
		}

		// Iterate through the extracted participant values
		for (int i = 0; i < count; i++) {
			TC_write_syslog("participantvalues[i]\n"); // Log participant values

			// Check if the participant role is "Approver"
			if (tc_strcmp(cpargValues[i], "Approver") == 0)
			{
				// Retrieve the participants associated with epm_proposed_reviewers
				EC_TRACE_AND_LOG(AOM_ask_value_tags(tAttachments[0], EPM_PROPOSED_REVIEWERS, &nNumber, &tParticpants));
			}
			//checks the the participants associated with epm_proposed_responsible_party
			EC_TRACE_AND_LOG(AOM_ask_value_tag(tAttachments[0], EPM_PROPOSED_RESPONSIBLE_PARTY, &tParticpant));
			if (tParticpant != NULLTAG)
			{
				TC_write_syslog("checks count of participants as one required\n");
				// Update the participant count
				checkcount = nNumber + 1;
			}
			else
			{
				TC_write_syslog("checks count of participants which is not required\n");
				checkcount = nNumber;
			}
		}

		// Free allocated memory for participant values
		for (int i = 0; i < count; i++) {
			delete[] participantvalues[i];
		}

		// If arguments are provided, check if the required number of participants is assigned
		if (iArgCount > 0) {
			if (checkcount < stoi(cpargValues[0])) {
				TC_write_syslog("Less than the required number of participants assigned\n");
				decision = EPM_nogo;  // Set decision to "No-Go"
			}
			else {
				TC_write_syslog("Required number of participants are assigned\n");
				decision = EPM_go;  // Set decision to "Go"
			}
		}
		else {
			// No argument case (default behavior)
			if (checkcount == 1) {
				TC_write_syslog("At least one participant is assigned\n");
				decision = EPM_go;
			}
			else {
				TC_write_syslog("No participants assigned\n");
				decision = EPM_nogo;
			}
		}

		return decision;  // Return final decision

	}
	catch (const int& ex) {
		TC_write_syslog("Error in method %s: %d", __FUNCTION__, ex);
		throw ex;  // Propagate the error code
	}
}