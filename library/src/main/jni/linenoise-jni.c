/*
 * Copyright 2020 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <string.h>

#include <jni.h>

#include <android/log.h>

#include "linenoise.h"

#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define LOG_TAG "linenoise-jni"

static jclass findClass(JNIEnv *env, const char *name) {
    jclass localClass = (*env)->FindClass(env, name);
    jclass globalClass = (*env)->NewGlobalRef(env, localClass);
    (*env)->DeleteLocalRef(env, localClass);
    if (!globalClass) {
        ALOGE("Failed to find class '%s'", name);
        abort();
    }
    return globalClass;
}

static jfieldID findField(JNIEnv *env, jclass clazz, const char *name, const char *signature) {
    jfieldID field = (*env)->GetFieldID(env, clazz, name, signature);
    if (!field) {
        ALOGE("Failed to find field '%s' '%s'", name, signature);
        abort();
    }
    return field;
}

static jmethodID findMethod(JNIEnv *env, jclass clazz, const char *name, const char *signature) {
    jmethodID method = (*env)->GetMethodID(env, clazz, name, signature);
    if (!method) {
        ALOGE("Failed to find method '%s' '%s'", name, signature);
        abort();
    }
    return method;
}

static jclass getListClass(JNIEnv *env) {
    static jclass listClass = NULL;
    if (!listClass) {
        listClass = findClass(env, "java/util/List");
    }
    return listClass;
}

static jint callListSize(JNIEnv *env, jobject list) {
    static jmethodID listSizeMethod = NULL;
    if (!listSizeMethod) {
        listSizeMethod = findMethod(env, getListClass(env), "size", "()I");
    }
    return (*env)->CallIntMethod(env, list, listSizeMethod);
}

static jobject callListGet(JNIEnv *env, jobject list, jint index) {
    static jmethodID listGetMethod = NULL;
    if (!listGetMethod) {
        listGetMethod = findMethod(env, getListClass(env), "get", "(I)Ljava/lang/Object;");
    }
    return (*env)->CallObjectMethod(env, list, listGetMethod, index);
}

static jobject callCompleterComplete(JNIEnv *env, jobject completer, jstring input) {
    static jclass completerClass = NULL;
    if (!completerClass) {
        completerClass = findClass(env, "me/zhanghai/android/linenoise/Completer");
    }
    static jmethodID completerCompleteMethod = NULL;
    if (!completerCompleteMethod) {
        completerCompleteMethod = findMethod(env, completerClass, "complete",
                "(Ljava/lang/String;)Ljava/util/List;");
    }
    return (*env)->CallObjectMethod(env, completer, completerCompleteMethod, input);
}

static jobject callHinterHint(JNIEnv *env, jobject hinter, jstring input) {
    static jclass hinterClass = NULL;
    if (!hinterClass) {
        hinterClass = findClass(env, "me/zhanghai/android/linenoise/Hinter");
    }
    static jmethodID hinterHintMethod = NULL;
    if (!hinterHintMethod) {
        hinterHintMethod = findMethod(env, hinterClass, "hint",
                "(Ljava/lang/String;)Lme/zhanghai/android/linenoise/Hint;");
    }
    return (*env)->CallObjectMethod(env, hinter, hinterHintMethod, input);
}

static void getHintFields(JNIEnv *env, jobject *hint, jstring *text, jint *color, jboolean *bold) {
    static jclass hintClass = NULL;
    if (!hintClass) {
        hintClass = findClass(env, "me/zhanghai/android/linenoise/Hint");
    }
    static jfieldID hintTextField = NULL;
    if (!hintTextField) {
        hintTextField = findField(env, hintClass, "text", "Ljava/lang/String;");
    }
    static jfieldID hintColorField = NULL;
    if (!hintColorField) {
        hintColorField = findField(env, hintClass, "color", "I");
    }
    static jfieldID hintBoldField = NULL;
    if (!hintBoldField) {
        hintBoldField = findField(env, hintClass, "bold", "Z");
    }
    *text = (*env)->GetObjectField(env, hint, hintTextField);
    *color = (*env)->GetIntField(env, hint, hintColorField);
    *bold = (*env)->GetBooleanField(env, hint, hintBoldField);
}

JNIEXPORT jstring JNICALL
Java_me_zhanghai_android_linenoise_Linenoise_readLine(
        JNIEnv *env, jclass clazz, jstring javaPrompt) {
    const char *prompt = (*env)->GetStringUTFChars(env, javaPrompt, NULL);
    if (!prompt) {
        return NULL;
    }
    char *line = linenoise(prompt);
    (*env)->ReleaseStringUTFChars(env, javaPrompt, prompt);
    if (!line) {
        return NULL;
    }
    jstring javaLine = (*env)->NewStringUTF(env, line);
    linenoiseFree(line);
    return javaLine;
}

JNIEXPORT void JNICALL
Java_me_zhanghai_android_linenoise_Linenoise_setMultiLine(
        JNIEnv *env, jclass clazz, jboolean javaMultiLine) {
    int multiLine = javaMultiLine == JNI_TRUE ? 1 : 0;
    linenoiseSetMultiLine(multiLine);
}

JNIEXPORT jboolean JNICALL
Java_me_zhanghai_android_linenoise_Linenoise_addHistory(
        JNIEnv *env, jclass clazz, jstring javaLine) {
    const char *line = (*env)->GetStringUTFChars(env, javaLine, NULL);
    if (!line) {
        return JNI_FALSE;
    }
    int result = linenoiseHistoryAdd(line);
    (*env)->ReleaseStringUTFChars(env, javaLine, line);
    jboolean javaResult = result ? JNI_TRUE : JNI_FALSE;
    return javaResult;
}

JNIEXPORT jboolean JNICALL
Java_me_zhanghai_android_linenoise_Linenoise_setHistoryMaxLength(
        JNIEnv *env, jclass clazz, jint javaLength) {
    int length = javaLength;
    int result = linenoiseHistorySetMaxLen(length);
    jboolean javaResult = result ? JNI_TRUE : JNI_FALSE;
    return javaResult;
}

JNIEXPORT jboolean JNICALL
Java_me_zhanghai_android_linenoise_Linenoise_saveHistory(
        JNIEnv *env, jclass clazz, jstring javaPath) {
    const char *path = (*env)->GetStringUTFChars(env, javaPath, NULL);
    if (!path) {
        return JNI_FALSE;
    }
    int result = linenoiseHistorySave(path);
    (*env)->ReleaseStringUTFChars(env, javaPath, path);
    jboolean javaResult = result == 0 ? JNI_TRUE : JNI_FALSE;
    return javaResult;
}

JNIEXPORT jboolean JNICALL
Java_me_zhanghai_android_linenoise_Linenoise_loadHistory(
        JNIEnv *env, jclass clazz, jstring javaPath) {
    const char *path = (*env)->GetStringUTFChars(env, javaPath, NULL);
    if (!path) {
        return JNI_FALSE;
    }
    int result = linenoiseHistoryLoad(path);
    (*env)->ReleaseStringUTFChars(env, javaPath, path);
    jboolean javaResult = result == 0 ? JNI_TRUE : JNI_FALSE;
    return javaResult;
}

JNIEXPORT void JNICALL
Java_me_zhanghai_android_linenoise_Linenoise_setMaskModeEnabled(
        JNIEnv *env, jclass clazz, jboolean javaEnabled) {
    if (javaEnabled == JNI_TRUE) {
        linenoiseMaskModeEnable();
    } else {
        linenoiseMaskModeDisable();
    }
}

static JNIEnv *sCallbackEnv = NULL;

static jobject sJavaCompleter = NULL;

static void completionCallback(const char *input, linenoiseCompletions *completions) {
    if (!sJavaCompleter) {
        return;
    }
    JNIEnv *env = sCallbackEnv;
    jstring javaInput = (*env)->NewStringUTF(env, input);
    jobject javaCompletions = callCompleterComplete(env, sJavaCompleter, javaInput);
    (*env)->DeleteLocalRef(env, javaInput);
    if ((*env)->ExceptionCheck(env)) {
        return;
    }
    jint javaCompletionsSize = callListSize(env, javaCompletions);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->DeleteLocalRef(env, javaCompletions);
        return;
    }
    for (jint javaIndex = 0; javaIndex < javaCompletionsSize; ++javaIndex) {
        jstring javaCompletion = callListGet(env, javaCompletions, javaIndex);
        if ((*env)->ExceptionCheck(env)) {
            (*env)->DeleteLocalRef(env, javaCompletions);
            return;
        }
        const char *completion = (*env)->GetStringUTFChars(env, javaCompletion, NULL);
        if (!completion) {
            (*env)->DeleteLocalRef(env, javaCompletion);
            (*env)->DeleteLocalRef(env, javaCompletions);
            return;
        }
        linenoiseAddCompletion(completions, completion);
        (*env)->ReleaseStringUTFChars(env, javaCompletion, completion);
        (*env)->DeleteLocalRef(env, javaCompletion);
    }
    (*env)->DeleteLocalRef(env, javaCompletions);
}

JNIEXPORT void JNICALL
Java_me_zhanghai_android_linenoise_Linenoise_setCompleter(
        JNIEnv *env, jclass clazz, jobject javaCompleter) {
    if (sJavaCompleter) {
        (*env)->DeleteGlobalRef(env, sJavaCompleter);
    }
    if (!javaCompleter) {
        linenoiseSetCompletionCallback(NULL);
        sJavaCompleter = NULL;
        return;
    }
    sCallbackEnv = env;
    sJavaCompleter = (*env)->NewGlobalRef(env, javaCompleter);
    linenoiseSetCompletionCallback(completionCallback);
}

static jobject sJavaHinter = NULL;

static char *hintsCallback(const char *input, int *color, int *bold) {
    if (!sJavaHinter) {
        return NULL;
    }
    JNIEnv *env = sCallbackEnv;
    jstring javaInput = (*env)->NewStringUTF(env, input);
    jobject javaHint = callHinterHint(env, sJavaHinter, javaInput);
    (*env)->DeleteLocalRef(env, javaInput);
    if ((*env)->ExceptionCheck(env)) {
        return NULL;
    }
    if (!javaHint) {
        return NULL;
    }
    jstring javaText;
    jint javaColor;
    jboolean javaBold;
    getHintFields(env, javaHint, &javaText, &javaColor, &javaBold);
    (*env)->DeleteLocalRef(env, javaHint);
    const char *javaTextChars = (*env)->GetStringUTFChars(env, javaText, NULL);
    if (!javaTextChars) {
        (*env)->DeleteLocalRef(env, javaText);
        return NULL;
    }
    char *text = strdup(javaTextChars);
    (*env)->ReleaseStringUTFChars(env, javaText, javaTextChars);
    (*env)->DeleteLocalRef(env, javaText);
    if (!text) {
        return NULL;
    }
    *color = javaColor;
    *bold = javaBold == JNI_TRUE ? 1 : 0;
    return text;
}

JNIEXPORT void JNICALL
Java_me_zhanghai_android_linenoise_Linenoise_setHinter(
        JNIEnv *env, jclass clazz, jobject javaHinter) {
    if (sJavaHinter) {
        (*env)->DeleteGlobalRef(env, sJavaHinter);
    }
    if (!javaHinter) {
        linenoiseSetHintsCallback(NULL);
        sJavaHinter = NULL;
        return;
    }
    sCallbackEnv = env;
    sJavaHinter = (*env)->NewGlobalRef(env, javaHinter);
    linenoiseSetHintsCallback(hintsCallback);
    linenoiseSetFreeHintsCallback(free);
}

JNIEXPORT void JNICALL
Java_me_zhanghai_android_linenoise_Linenoise_clearScreen(
        JNIEnv *env, jclass clazz) {
    linenoiseClearScreen();
}
