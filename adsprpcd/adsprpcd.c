/*
 * Copyright (c) 2013-2016, 2020, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define LOG_TAG "audioadsprpcd"
#define ATRACE_TAG (ATRACE_TAG_HAL)
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <log/log.h>

#ifndef ADSP_DEFAULT_LISTENER_NAME
#define ADSP_DEFAULT_LISTENER_NAME "libadsp_default_listener.so"
#endif

typedef int (*adsp_default_listener_start_t)(int argc, char *argv[]);

int main(int argc, char *argv[]) {

  int nErr = 0;
  void *adsphandler = NULL;
  adsp_default_listener_start_t listener_start;

  ALOGI("audio adsp daemon starting");
  while (1) {
    if(NULL != (adsphandler = dlopen(ADSP_DEFAULT_LISTENER_NAME, RTLD_NOW))) {
      if(NULL != (listener_start =
        (adsp_default_listener_start_t)dlsym(adsphandler, "adsp_default_listener_start"))) {
        ALOGI("adsp_default_listener_start called");
        nErr = listener_start(argc, argv);
      }
      if(0 != dlclose(adsphandler)) {
        ALOGE("dlclose failed");
      }
    } else {
      ALOGE("audio adsp daemon error %s", dlerror());
    }
    if (nErr == 0x072) {
      ALOGE("fastRPC device driver is disabled, retrying...");
    }
    ALOGE("audio adsp daemon will restart after 25ms...");
    usleep(25000);
  }
  ALOGI("audio adsp daemon exiting %x", nErr);

  return nErr;
}
