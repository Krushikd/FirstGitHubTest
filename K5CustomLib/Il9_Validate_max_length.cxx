/**= ============================================================================
ECM
================================================================================
File description :

Filename: Validate_max_length_handler.cxx

	Action      : Rule Handler
	HandlerName :Validate_max_length_handler
	Description : - Validation step to check the maximum length of the given attribute on the given secondary object type.
	@History
	================================================================================================
	Date                        Name

	21 - 03 - 2025            Krushik DM
	==================================================================================================*/



#include "Il9_Generichandlers.hxx"
#include "Il9_Validate_max_length.hxx"

EPM_decision_t Il9_Validate_max_length(EPM_rule_message_t msg)
{
	int iStatus = ITK_ok;
	ResultStatus status;
	EPM_decision_t decision = EPM_go; // Default to success
	tag_t root_task = NULLTAG;
	tag_t* attachments = NULL;
	int attachment_count = 0;
	int arg_count = 0;
	scoped_smptr<char*> arg_names;
	scoped_smptr<char*> arg_values;
	tag_t relation_type_tag = NULLTAG;
	tag_t* sec_objects = NULL;
	int sec_count = 0;
	int max_length = 10;

	try
	{
		// Retrieve root task
		EC_TRACE_AND_LOG(EPM_ask_root_task(msg.task, &root_task));

		// Get attachments of type "Target"
		EC_TRACE_AND_LOG(EPM_ask_attachments(root_task, EPM_target_attachment, &attachment_count, &attachments));

		// Retrieve workflow arguments
		EC_TRACE_AND_LOG(EPM_args_process_args(msg.task, msg.arguments, &arg_count, &arg_names, &arg_values));
		scoped_smptr<char> secondary_type;
		scoped_smptr<char> property_name;
		scoped_smptr<char> relation_type_name;

		// Loop through arguments to find SecondaryType, PropertyName, and RelationType
		for (int i = 0; i < arg_count; i++)
		{
			if (strcmp(arg_names[i], SECONDARYTYPEARG) == 0)
			{
				secondary_type = arg_values[i]; // Get secondary object type
			}
			else if (strcmp(arg_names[i], PROPERTYNAMEARG) == 0)
			{
				property_name = arg_values[i]; // Get property name
			}
			else if (strcmp(arg_names[i], RELATIONTYPEARG) == 0)
			{
				relation_type_name = arg_values[i]; // Get relation type
			}
		}

		// Validate arguments
		if (secondary_type == NULL || property_name == NULL || relation_type_name == NULL)
		{
			TC_write_syslog("Local Debug :: Missing workflow arguments - SecondaryType, PropertyName, or RelationType.\n");
			return EPM_nogo;
		}

		if (attachment_count == 0)
		{
			TC_write_syslog("Local Debug :: No target attachments found.\n");
			return EPM_nogo;
		}

		// Find relation type dynamically
		EC_TRACE_AND_LOG(GRM_find_relation_type(relation_type_name.getString(), &relation_type_tag));

		// Iterate through each target object
		for (int i = 0; i < attachment_count; i++)
		{
			// Get secondary objects related through the user-specified relation type
			EC_TRACE_AND_LOG(GRM_list_secondary_objects_only(attachments[i], relation_type_tag, &sec_count, &sec_objects));

			if (sec_count == 0)
			{
				TC_write_syslog("Local Debug :: No Secondary Objects found for relation type: %s.\n", relation_type_name.getString());
				return EPM_nogo;
			}

			for (int j = 0; j < sec_count; j++)
			{
				scoped_smptr<char> type_name;
				scoped_smptr<char> attribute_value;

				// Get the object type of the secondary object
				EC_TRACE_AND_LOG(AOM_ask_value_string(sec_objects[j], SECONDARYOBJECTTYPE, &type_name));

				// Validate if it's the expected secondary type
				if (type_name != NULL && strcmp(secondary_type.getString(), type_name.getString()) == 0)
				{
					// Get the specified property value
					EC_TRACE_AND_LOG(AOM_ask_value_string(sec_objects[j], property_name.getString(), &attribute_value));

					// Check if property exceeds max length (default: 5)
					if (attribute_value != NULL && strlen(attribute_value.getString()) > max_length)
					{
						TC_write_syslog("Local Debug :: Validation failed - Property: %s, Length: %zu\n",
							property_name.getString(), strlen(attribute_value.getString()));
						EC_TRACE_AND_LOG(EMH_store_error_s2(EMH_severity_error, PLM_error1, property_name.getString(), attribute_value.getString()));
						return EPM_nogo;  // Prevent workflow progression
					}
					else
					{
						TC_write_syslog("Local Debug :: Property %s length is valid: %zu\n", property_name.getString(), strlen(attribute_value.getString()));
					}
				}
				else
				{
					TC_write_syslog("Local Debug :: Property %s length is valid: %zu\n", type_name.getString());
					EC_TRACE_AND_LOG(EMH_store_error_s2(EMH_severity_error, PLM_error2, type_name.getString(), attribute_value.getString()));
					return EPM_nogo;
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