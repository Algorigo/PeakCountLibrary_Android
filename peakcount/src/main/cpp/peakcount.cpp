#include <jni.h>
#include <vector>
#include "../../../../respiration_detect_time_domain_cpp/src/signalgo.h"

extern "C" JNIEXPORT jint JNICALL
Java_com_algorigo_peakcount_PeakCount_peakCount(
        JNIEnv* env,
        jclass /* this */,
        jobject values,
        jdouble subtractAlpha,
        jdouble emaAlpha) {

    jclass jclassList = env->GetObjectClass(values);
    jmethodID jmethodIdSize = env->GetMethodID(jclassList, "size", "()I");
    jmethodID jmethodIdGet = env->GetMethodID(jclassList, "get", "(I)Ljava/lang/Object;");
    jclass jclassDouble = env->FindClass("java/lang/Double");
    jmethodID jmethodIdDoubleValue = env->GetMethodID(jclassDouble, "doubleValue", "()D");
    auto size = env->CallIntMethod(values, jmethodIdSize);
    std::vector<double> data(size);
    for (auto index=0; index<size; index++) {
        jobject jdoubleObject = env->CallObjectMethod(values, jmethodIdGet, index);
        data[index] = env->CallDoubleMethod(jdoubleObject, jmethodIdDoubleValue);
        env->DeleteLocalRef(jdoubleObject);
    }
    env->DeleteLocalRef(jclassDouble);
    env->DeleteLocalRef(jclassList);

    auto substracted = signalgo::subtract_ema(data, subtractAlpha);
    auto ema = signalgo::ema(substracted, emaAlpha);
    auto peakCount = signalgo::get_peaks_count(size, ema.data());

    return peakCount;
}
