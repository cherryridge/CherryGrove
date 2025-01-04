#pragma once
#include <string>

#include "pack.hpp"

namespace PackStatus {

	using std::string, std::to_string;

	struct PackStatusFlag {
		uuid uuid_f;
		u32 packVersion;
		u16 statusFlags;
	};

	void refreshStatus(const char* rootDir);
	u16 getStatusFlags(const PackIdentifier& id);
	void saveStatus(const PackStatusFlag& raw);

	const auto configSchema_raw = string(R"(
{
	"$schema": "http://json-schema.org/draft-07/schema#",
	"type": "object",
	"properties": {
		"formatVersion": {
			"type": "integer",
			"const": )") + to_string(LATEST_FORMAT_VERSION_INTERNAL) + string(R"(
		},
		"data": {
			"type": "array",
			"items": {
				"type": "object",
				"properties": {
					"uuid": {
						"type": "string",
						"format": "uuid"
					},
					"version": {
						"type": "integer",
						"minimum": 1
					},
					"valid": {
						"type": "boolean"
					},
					"enabled": {
						"type": "boolean"
					},
					"globalEnabled": {
						"type": "boolean"
					}
				},
				"required": [
					"uuid",
					"version",
					"valid",
					"enabled",
					"globalEnabled"
				]
			}
		}
	},
	"required": [
		"formatVersion",
		"data"
	]
})");
}