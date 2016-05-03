#include <jni.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include "helper.h"
#include "ENC_CPP/enc.hpp"

X264_Encoder *encoder = NULL;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring Java_it_polito_mad_streamsender_encoding_StreamSenderJNI_hello(JNIEnv* env, jobject javaThis){
    jstring result;
    const char msg[60] = "ciaociao";
    result = env->NewStringUTF(msg);
    return result;
}


JNIEXPORT void Java_it_polito_mad_streamsender_encoding_StreamSenderJNI_nativeInitEncoder(JNIEnv* env, jobject thiz
        //, const jint width, const jint height
    ){
    //init_encoder(width, height);
    encoder = new X264_Encoder();
}

JNIEXPORT void Java_it_polito_mad_streamsender_encoding_StreamSenderJNI_nativeReleaseEncoder(JNIEnv* env, jobject thiz){
    //release_encoder();
    delete encoder;
}

JNIEXPORT jboolean Java_it_polito_mad_streamsender_encoding_StreamSenderJNI_nativeApplyParams(JNIEnv* env, jobject thiz,
        const jint width, const jint height, const jint bitrateKbps){
    return (jboolean) encoder->applyParams(width, height, bitrateKbps);
}

JNIEXPORT jbyteArray Java_it_polito_mad_streamsender_encoding_StreamSenderJNI_nativeDoEncode(JNIEnv* env, jobject thiz,
        const jint width, const jint height, jbyteArray _yuv, const jint bitrate){
    //jboolean isCopy = JNI_TRUE;
    jbyte *yuvData = env->GetByteArrayElements(_yuv, NULL);
    jsize yuvSize = env->GetArrayLength(_yuv);
    //LOGD("yuv size: %d -> (%dx%d) %d kbps", yuvSize, width, height, bitrate);
    //jbyte *nalsData = env->GetByteArrayElements(_outBuffer, &isCopy);

    uint8_t *native_yuvData = (uint8_t*)yuvData;
    uint8_t native_nalsData[yuvSize];
    //uint8_t *native_nalsData = (uint8_t*)nalsData;
    int ret = encoder->encode(native_yuvData, width, height, bitrate, native_nalsData);
    //env->ReleaseByteArrayElements(_outBuffer, nalsData, 0);
    env->ReleaseByteArrayElements(_yuv, yuvData, JNI_ABORT);

    jbyteArray result = env->NewByteArray(ret);
    env->SetByteArrayRegion(result, 0, ret, (jbyte *)native_nalsData);

    return result;
}

JNIEXPORT jobjectArray Java_it_polito_mad_streamsender_encoding_StreamSenderJNI_nativeGetHeaders(JNIEnv* env, jobject thiz){
    uint8_t *sps = NULL, *pps = NULL;
    size_t sps_size = 0, pps_size = 0;
    encoder->getConfigBytes(&sps, &sps_size, &pps, &pps_size);

    jbyteArray jni_SPS = env->NewByteArray(sps_size);
    env->SetByteArrayRegion(jni_SPS, 0, sps_size, (jbyte *)sps);

    jbyteArray jni_PPS = env->NewByteArray(pps_size);
    env->SetByteArrayRegion(jni_PPS, 0, pps_size, (jbyte *)pps);


    jobjectArray result = env->NewObjectArray(2, env->GetObjectClass(jni_SPS), NULL);

    env->SetObjectArrayElement(result, 0, jni_SPS);
    env->SetObjectArrayElement(result, 1, jni_PPS);
    return result;
}

/*
static struct sigaction old_sa[NSIG];
static jmethodID nativeCrashed;
static JNIEnv *env;
static jclass cls;

void android_sigaction(int signal, siginfo_t *info, void *reserved){
    LOGE("ERROR!!!!!");
    char *msg = NULL;
    if (info && info->si_errno){
        msg = strerror(info->si_errno);
    }
    LOGE("ERROR!!!!!");
	env->CallStaticVoidMethod(cls, nativeCrashed, msg);
	old_sa[signal].sa_handler(signal);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved){
    if (jvm->GetEnv((void **)&env, JNI_VERSION_1_2)) return JNI_ERR;
    cls = env->FindClass("it/polito/mad/streamsender/encoding/StreamSenderJNI");
    nativeCrashed = env->GetStaticMethodID(cls, "nativeCrashed", "(Ljava/lang/String;)V");
    struct sigaction handler;
	memset(&handler, 0, sizeof(struct sigaction));
	handler.sa_sigaction = android_sigaction;
	handler.sa_flags = SA_RESETHAND | SA_SIGINFO;
#define CATCHSIG(X) sigaction(X, &handler, &old_sa[X])
	CATCHSIG(SIGILL);
	CATCHSIG(SIGABRT);
	CATCHSIG(SIGBUS);
	CATCHSIG(SIGFPE);
	CATCHSIG(SIGSEGV);
	CATCHSIG(SIGSTKFLT);
	CATCHSIG(SIGPIPE);

    LOGD("Loaded jni");
	return JNI_VERSION_1_2;
}
*/

#ifdef __cplusplus
}
#endif
