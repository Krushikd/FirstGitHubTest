/**= ============================================================================
ECM
================================================================================
File description :

Filename: I3CustomWorkflowHandler.cxx

	Action     : Rule Handler
	HandlerName:EPM-Check-Object-Properties
	Description :
	   - Validate the Primary Objects with Multiple Properties with Multiple Values considering the object_type .
	   - Validate the Secondary Objects with relation_type and Multiple Properties with Multiple Values considering the object_type .

	@History
	================================================================================================
	Date                        Name

	24 - 02 - 2025            Krushik DM
	==================================================================================================*/


#include "Il9_CheckObjectProperties.hxx"
#include "Il9_Generichandlers.hxx"

EPM_decision_t Il9_CheckObjectProperties(EPM_rule_message_t msg)

{
	int iStatus = ITK_ok;
	try
	{
		tag_t root_task = NULL;
		tag_t* attachments = NULL;
		int attachment_count = 0;
		EPM_decision_t decision = EPM_nogo;
		int arg_count = 0;
		scoped_smptr<char*> arg_names;
		scoped_smptr<char*> arg_values;
		tag_t relation_type_tag = NULLTAG;

		// Retrieve root task
		EC_TRACE_AND_LOG(EPM_ask_root_task(msg.task, &root_task));
		// Get attachments of type "Target"
		EC_TRACE_AND_LOG(EPM_ask_attachments(root_task, EPM_target_attachment, &attachment_count, &attachments));
		// Retrieve workflow arguments
		EC_TRACE_AND_LOG(EPM_args_process_args(msg.task, msg.arguments, &arg_count, &arg_names, &arg_values));

		// Store arguments in a map for easy access
		std::map<std::string, std::string> args_map;
		for (int i = 0; i < arg_count; i++)
		{
			args_map[arg_names[i]] = arg_values[i];
		}

		// Validate if primary object exists
		if (attachment_count == 0)
		{
			TC_write_syslog("\nLocal Debug :: No primary object found.");
			return decision;
		}
		tag_t primary_object = attachments[0];

		// Validate primary object type
		scoped_smptr<char> object_type;
		EC_TRACE_AND_LOG(AOM_ask_value_string(primary_object, OBJECTTYPE, &object_type));
		if (strcmp(args_map[OBJECTTYPEARGUMENT].c_str(), object_type.getString()) != 0)
		{
			TC_write_syslog("\nLocal Debug :: Primary object type mismatch. Expected: %s, Found: %s",
				args_map[OBJECTTYPEARGUMENT].c_str(), object_type.getString());
			return decision;
		}

		// Parse multiple properties
		std::vector<std::string> properties = split(args_map[PRIMARYOBJECTPROPERTY], '/');
		std::vector<std::string> values;
		bool is_value_provided = false;
		if (args_map.find(PRIMARYOBJECTVALUE) != args_map.end() && !args_map[PRIMARYOBJECTVALUE].empty())
		{
			values = split(args_map[PRIMARYOBJECTVALUE], '/');
			is_value_provided = true;
		}

		// Validate properties
		for (size_t i = 0; i < properties.size(); i++)
		{
			scoped_smptr<char> property_value;
			EC_TRACE_AND_LOG(AOM_ask_value_string(primary_object, properties[i].c_str(), &property_value));
			if (property_value.getString() == NULL || strlen(property_value.getString()) == 0)
			{
				TC_write_syslog("\nLocal Debug :: Property %s is NULL or empty on primary object.", properties[i].c_str());
				return decision;
			}
		}

		// Retrieve relation type for secondary objects
		if (args_map.find(RELATIONTYPE) != args_map.end() && !args_map[RELATIONTYPE].empty())
		{
			EC_TRACE_AND_LOG(GRM_find_relation_type(args_map[RELATIONTYPE].c_str(), &relation_type_tag));

			// Validate relation type
			if (relation_type_tag == NULLTAG)
			{
				TC_write_syslog("\nLocal Debug :: Relation type not found: %s", args_map[RELATIONTYPE].c_str());
				return decision;
			}
		}

		// Fetch secondary objects linked via the specified relation
		int sec_count = 0;
		tag_t* sec_objects = NULL;
		EC_TRACE_AND_LOG(GRM_list_secondary_objects_only(primary_object, relation_type_tag, &sec_count, &sec_objects));
		if (sec_count == 0)
		{
			TC_write_syslog("\nLocal Debug :: No secondary objects found.");
			return decision;
		}

		// Validate secondary object type
		if (args_map.find(SECONDARYOBJECTTYPE) != args_map.end() && !args_map[SECONDARYOBJECTTYPE].empty())
		{
			for (int i = 0; i < sec_count; i++)
			{
				scoped_smptr<char> sec_object_type;
				EC_TRACE_AND_LOG(AOM_ask_value_string(sec_objects[i], OBJECTTYPE, &sec_object_type));
				if (args_map[SECONDARYOBJECTTYPE] != sec_object_type.getString())
				{
					TC_write_syslog("\nLocal Debug :: Secondary object type mismatch. Expected: %s, Found: %s", args_map[SECONDARYOBJECTTYPE].c_str(), sec_object_type.getString());
					return decision;
				}
			}
		}

		// Validate secondary object Properties
		std::vector<std::string> sec_properties = split(args_map[SECONDARYOBJECTPROPERTY], '/');
		std::vector<std::string> sec_values;
		bool is_sec_value_provided = false;
		if (args_map.find(SECONDARYOBJECTVALUE) != args_map.end() && !args_map[SECONDARYOBJECTVALUE].empty())
		{
			sec_values = split(args_map[SECONDARYOBJECTVALUE], '/');
			is_sec_value_provided = true;
		}
		for (int i = 0; i < sec_count; i++)
		{
			for (size_t j = 0; j < sec_properties.size(); j++)
			{
				scoped_smptr<char> sec_property_value;
				EC_TRACE_AND_LOG(AOM_ask_value_string(sec_objects[i], sec_properties[j].c_str(), &sec_property_value));
				if (sec_property_value.getString() == NULL || strlen(sec_property_value.getString()) == 0)
				{
					TC_write_syslog("\nLocal Debug :: Property %s is NULL or empty on secondary object.", sec_properties[j].c_str());
					return decision;
				}
				if (is_sec_value_provided && sec_property_value.getString() != sec_values[j])

				{
					TC_write_syslog("\nLocal Debug :: Secondary property mismatch. Property: %s, Expected: %s, Found: %s",
						sec_properties[j].c_str(), sec_values[j].c_str(), sec_property_value.getString());
					return decision;
				}
			}
		}
		// If all validations pass, allow workflow continuation
	}
	catch (const int& ex) {
		TC_write_syslog("Error in method %s: %d", __FUNCTION__, ex);
		throw ex;  // Propagate the error code
	}
	return EPM_go;
}

std::vector<std::string> split(const std::string& str, char delimiter)

{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}
