#include <jni.h>
#include <jvmti.h>
#include <iostream>     
#include "InstrumentationManager.h" 
#include "jplis.h"

// Macro definitions from the JDK (JPLISAgent.h)
#define JPLIS_INSTRUMENTIMPL_CLASSNAME                        "sun/instrument/InstrumentationImpl"
#define JPLIS_INSTRUMENTIMPL_CONSTRUCTOR_METHODNAME           "<init>"
#define JPLIS_INSTRUMENTIMPL_CONSTRUCTOR_METHODSIGNATURE      "(JZZ)V"
#define JPLIS_INSTRUMENTIMPL_TRANSFORM_METHODNAME             "transform"
#define JPLIS_INSTRUMENTIMPL_TRANSFORM_METHODSIGNATURE        "(Ljava/lang/ClassLoader;Ljava/lang/String;Ljava/lang/Class;Ljava/security/ProtectionDomain;[BZ)[B"

// Function from the JDK (JavaExceptions.c)
jboolean checkForAndClearThrowable(JNIEnv* jnienv) {
	jboolean result = jnienv->ExceptionCheck();
	if (result) {
		jnienv->ExceptionClear();
	}
	return result;
}

jobject createInstrumentation(JavaVM* vm, JNIEnv* env, jvmtiEnv* jvmti) {
	JPLISAgent* agent = getDummyAgent(vm, jvmti);

	jclass instrumentationClass = env->FindClass(JPLIS_INSTRUMENTIMPL_CLASSNAME);
	if (checkForAndClearThrowable(env) || instrumentationClass == 0) {
		std::cout << "Failed to get instrumentation class";
		return 0;
	}

	jmethodID constructor = env->GetMethodID(
		instrumentationClass, 
		JPLIS_INSTRUMENTIMPL_CONSTRUCTOR_METHODNAME, 
		JPLIS_INSTRUMENTIMPL_CONSTRUCTOR_METHODSIGNATURE);

	if (checkForAndClearThrowable(env) || constructor == 0) {
		std::cout << "Failed to get instrumentation constructor";
		return 0;
	}

	jlong agentPtr = (jlong)(intptr_t)agent;
	jobject instrumentationLocal = env->NewObject(
		instrumentationClass,
		constructor,
		agentPtr,
		agent->mRedefineAdded,
		agent->mNativeMethodPrefixAdded);

	if (checkForAndClearThrowable(env) || instrumentationLocal == 0) {
		std::cout << "Failed to get instrumentation object (local ref)";
		return 0;
	}

	jobject instrumentation = env->NewGlobalRef(instrumentationLocal);

	if (checkForAndClearThrowable(env) || instrumentation == 0) {
		std::cout << "Failed to make instrumentation ref global";
		return 0;
	}

	jmethodID transformMethodID = env->GetMethodID(instrumentationClass, JPLIS_INSTRUMENTIMPL_TRANSFORM_METHODNAME, JPLIS_INSTRUMENTIMPL_TRANSFORM_METHODSIGNATURE);

	if (checkForAndClearThrowable(env) || transformMethodID == 0) {
		std::cout << "Failed to get transformMethodID";
		return 0;
	}

	agent->mInstrumentationImpl = instrumentation;
	agent->mTransform = transformMethodID;

	return instrumentation;
}


JNIEXPORT jobject JNICALL Java_InstrumentationManager_getInstrumentation(JNIEnv* env, jclass klass) {
	jsize vmCount{};
	JavaVM* vmPtr{};
	jvmtiEnv* jvmti{};

	// logical OR processed from left to right
	if (JNI_GetCreatedJavaVMs(&vmPtr, 1, &vmCount) != JNI_OK 
		|| vmCount < 1
		|| vmPtr->GetEnv((void**)&jvmti, JVMTI_VERSION_1_1) != JNI_OK) {
		std::cout << "Failed to get javaVM or jvmti" << std::endl;
		return 0;
	}

	jvmtiCapabilities capabilities{};
	capabilities.can_redefine_classes = 1;
	capabilities.can_redefine_any_class = 1;
	capabilities.can_retransform_classes = 1;
	capabilities.can_retransform_any_class = 1;
	capabilities.can_set_native_method_prefix = 1;

	jvmtiError error = jvmti->AddCapabilities(&capabilities);
	if (error != JVMTI_ERROR_NONE) {
		std::cout << "Failed to add capabilities: " << error << std::endl;
		return 0;
	}

	return createInstrumentation(vmPtr, env, jvmti);;
}
