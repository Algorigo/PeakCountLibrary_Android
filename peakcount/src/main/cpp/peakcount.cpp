#include <jni.h>
#include <exception>
#include <string>
#include <utility>
#include <vector>
#include "signalgo.h"

namespace {

void throwJavaException(JNIEnv* env, const char* className, const char* message) {
    if (env->ExceptionCheck()) {
        return;
    }
    jclass exceptionClass = env->FindClass(className);
    if (exceptionClass != nullptr) {
        env->ThrowNew(exceptionClass, message);
        env->DeleteLocalRef(exceptionClass);
    }
}

} // namespace

extern "C" JNIEXPORT jint JNICALL
Java_com_algorigo_peakcount_PeakCount_peakCount(
        JNIEnv* env,
        jclass /* this */,
        jobject values,
        jdouble subtractAlpha,
        jdouble emaAlpha) {

    if (values == nullptr) {
        throwJavaException(env, "java/lang/IllegalArgumentException", "List must not be null");
        return 0;
    }

    try {
        jclass jclassList = env->GetObjectClass(values);
        jmethodID jmethodIdSize = env->GetMethodID(jclassList, "size", "()I");
        jmethodID jmethodIdGet = env->GetMethodID(jclassList, "get", "(I)Ljava/lang/Object;");
        env->DeleteLocalRef(jclassList);
        if (env->ExceptionCheck()) {
            return 0;
        }

        const jint size = env->CallIntMethod(values, jmethodIdSize);
        if (env->ExceptionCheck() || size <= 0) {
            return 0;
        }

        jclass jclassDouble = env->FindClass("java/lang/Double");
        if (jclassDouble == nullptr) {
            return 0;
        }
        jmethodID jmethodIdDoubleValue = env->GetMethodID(jclassDouble, "doubleValue", "()D");
        env->DeleteLocalRef(jclassDouble);
        if (env->ExceptionCheck()) {
            return 0;
        }

        std::vector<double> data(static_cast<size_t>(size));
        for (jint index = 0; index < size; ++index) {
            jobject jdoubleObject = env->CallObjectMethod(values, jmethodIdGet, index);
            if (env->ExceptionCheck()) {
                return 0;
            }

            if (jdoubleObject == nullptr) {
                std::string message = "List contains null at index " + std::to_string(index);
                throwJavaException(env, "java/lang/IllegalArgumentException", message.c_str());
                return 0;
            }

            data[index] = env->CallDoubleMethod(jdoubleObject, jmethodIdDoubleValue);
            env->DeleteLocalRef(jdoubleObject);
            if (env->ExceptionCheck()) {
                return 0;
            }
        }

        auto subtracted = signalgo::subtract_ema(std::move(data), subtractAlpha);
        auto ema = signalgo::ema(std::move(subtracted), emaAlpha);
        return signalgo::get_peaks_count(size, ema.data());
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    } catch (...) {
        throwJavaException(env, "java/lang/RuntimeException", "Unknown error occurred in native code");
        return 0;
    }
}
