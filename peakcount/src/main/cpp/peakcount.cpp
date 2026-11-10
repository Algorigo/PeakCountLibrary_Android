#include <jni.h>
#include <vector>
#include "signalgo.h"

extern "C" JNIEXPORT jint JNICALL
Java_com_algorigo_peakcount_PeakCount_peakCount(
        JNIEnv* env,
        jclass /* this */,
        jobject values,
        jdouble subtractAlpha,
        jdouble emaAlpha) {

    try {
        jclass jclassList = env->GetObjectClass(values);
        jmethodID jmethodIdSize = env->GetMethodID(jclassList, "size", "()I");
        jmethodID jmethodIdGet = env->GetMethodID(jclassList, "get", "(I)Ljava/lang/Object;");
        jclass jclassDouble = env->FindClass("java/lang/Double");
        jmethodID jmethodIdDoubleValue = env->GetMethodID(jclassDouble, "doubleValue", "()D");
        auto size = env->CallIntMethod(values, jmethodIdSize);
        std::vector<double> data(size);
        for (auto index=0; index<size; index++) {
            jobject jdoubleObject = env->CallObjectMethod(values, jmethodIdGet, index);

            // Null 체크
            if (jdoubleObject == nullptr) {
                jclass exceptionClass = env->FindClass("java/lang/IllegalArgumentException");
                env->ThrowNew(exceptionClass, "List contains null at index");
                return 0;
            }

            data[index] = env->CallDoubleMethod(jdoubleObject, jmethodIdDoubleValue);
            env->DeleteLocalRef(jdoubleObject);
        }
        env->DeleteLocalRef(jclassDouble);
        env->DeleteLocalRef(jclassList);

        auto substracted = signalgo::subtract_ema(data, subtractAlpha);
        auto ema = signalgo::ema(substracted, emaAlpha);
        auto peakCount = signalgo::get_peaks_count(size, ema.data());

        return peakCount;
    } catch (const std::exception& e) {
        // C++ 표준 예외 처리
        jclass exceptionClass = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(exceptionClass, e.what());
        return 0;
    } catch (...) {
        // 알려지지 않은 예외 처리
        jclass exceptionClass = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(exceptionClass, "Unknown error occurred in native code");
        return 0;
    }
}
