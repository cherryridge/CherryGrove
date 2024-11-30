#pragma once
#include <string>
#include <v8/libplatform/libplatform.h>
#include <v8/v8-context.h>
#include <v8/v8-initialization.h>
#include <v8/v8-isolate.h>
#include <v8/v8-local-handle.h>
#include <v8/v8-primitive.h>
#include <v8/v8-script.h>

class JSEngine {
public:
	JSEngine(const char* executionPath);
	~JSEngine();
	void loadFile(const char* filePath);
	void evalToStdout(const char* expression);
private:
	v8::Isolate* V8Isolate;
	v8::Isolate::CreateParams createParams;
	std::unique_ptr<v8::Platform> platform;
};