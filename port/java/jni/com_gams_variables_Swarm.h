/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include "gams/GAMS_Export.h"
/* Header for class com_gams_variables_Swarm */

#ifndef _Included_com_gams_variables_Swarm
#define _Included_com_gams_variables_Swarm
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_gams_variables_Swarm
 * Method:    jni_Swarm
 * Signature: ()J
 */
GAMS_Export jlong JNICALL Java_com_gams_variables_Swarm_jni_1Swarm__
  (JNIEnv *, jobject);

/*
 * Class:     com_gams_variables_Swarm
 * Method:    jni_Swarm
 * Signature: (J)J
 */
GAMS_Export jlong JNICALL Java_com_gams_variables_Swarm_jni_1Swarm__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_gams_variables_Swarm
 * Method:    jni_freeSwarm
 * Signature: (J)V
 */
GAMS_Export void JNICALL Java_com_gams_variables_Swarm_jni_1freeSwarm
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_gams_variables_Swarm
 * Method:    jni_init
 * Signature: (JJJLjava/lang/String;)V
 */
GAMS_Export void JNICALL Java_com_gams_variables_Swarm_jni_1init
  (JNIEnv *, jobject, jlong, jlong, jlong, jstring);

/*
 * Class:     com_gams_variables_Swarm
 * Method:    jni_toString
 * Signature: (J)Ljava/lang/String;
 */
GAMS_Export jstring JNICALL Java_com_gams_variables_Swarm_jni_1toString
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_gams_variables_Swarm
 * Method:    jni_getCommand
 * Signature: (J)J
 */
GAMS_Export jlong JNICALL Java_com_gams_variables_Swarm_jni_1getCommand
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_gams_variables_Swarm
 * Method:    jni_getArgs
 * Signature: (J)J
 */
GAMS_Export jlong JNICALL Java_com_gams_variables_Swarm_jni_1getArgs
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_gams_variables_Swarm
 * Method:    jni_getMinAlt
 * Signature: (J)J
 */
GAMS_Export jlong JNICALL Java_com_gams_variables_Swarm_jni_1getMinAlt
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_gams_variables_Swarm
 * Method:    jni_getSize
 * Signature: (J)J
 */
GAMS_Export jlong JNICALL Java_com_gams_variables_Swarm_jni_1getSize
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
