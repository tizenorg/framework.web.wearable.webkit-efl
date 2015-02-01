/*
 * Copyright (C) 2012 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "AudioSessionManagerGStreamerTizen.h"

#if ENABLE(TIZEN_GSTREAMER_AUDIO)

#include "GStreamerUtilities.h"
#include "GStreamerVersioning.h"
#if ENABLE(TIZEN_EXTENSIBLE_API)
#include "TizenExtensibleAPI.h"
#endif
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mm_sound.h>
#include <unistd.h>

namespace WebCore {

void AudioSessionManagerGStreamerTizen::setVolumeSessionToMediaType()
{
    volume_type_t volumeType;
#if ENABLE(TIZEN_EXTENSIBLE_API)
    if (TizenExtensibleAPI::extensibleAPI().mediaStreamRecord()) {
        int error = mm_sound_volume_get_current_playing_type(&volumeType);
        LOG(Media, "AudioSessionManagerGStreamerTizen::setVolumeSessionToMediaType - get current playing type error %x", error);
        if (!error && volumeType != VOLUME_TYPE_MEDIA)
            mm_sound_volume_primary_type_set(VOLUME_TYPE_MEDIA);
    }
#endif
}

void AudioSessionManagerGStreamerTizen::clearVolumeSessionFromMediaType()
{
#if ENABLE(TIZEN_EXTENSIBLE_API)
    if (TizenExtensibleAPI::extensibleAPI().mediaStreamRecord())
        mm_sound_volume_primary_type_clear();
#endif
}

AudioSessionManagerGStreamerTizen::AudioSessionManagerGStreamerTizen()
    : m_eventType(ASM_EVENT_MEDIA_WEBKIT)
    , m_handle(-1)
    , m_notifyCallback(0)
    , m_processIdentifier(getpid())
    , m_stateType(ASM_STATE_NONE)
#if ENABLE(TIZEN_TV_ASM_READY)
    , m_allocated(false)
    , m_resources()
#endif
{
}

AudioSessionManagerGStreamerTizen::~AudioSessionManagerGStreamerTizen()
{
#if ENABLE(TIZEN_TV_ASM_READY)
    if (m_allocated)
        deallocateResources();
#endif

    unregisterAudioSessionManager();
}

bool AudioSessionManagerGStreamerTizen::registerAudioSessionManager(ASM_sound_cb_t notifyCallback, void* callbackData)
{
    m_notifyCallback = notifyCallback;

    int error = 0;
    if (!ASM_register_sound(m_processIdentifier, &m_handle, m_eventType, m_stateType, m_notifyCallback, callbackData, ASM_RESOURCE_NONE, &error)) {
        LOG_MEDIA_MESSAGE("register is failed. errcode = 0x%X", error);
        return false;
    }

#if ENABLE(TIZEN_EXTENSIBLE_API)
    if (TizenExtensibleAPI::extensibleAPI().soundMode()) {
        bool ret = ASM_set_session_option(m_handle, ASM_SESSION_OPTION_PAUSE_OTHERS, &error);
        if (ret) {
            TIZEN_LOGI("AudioSession mode has been set to ASM_SESSION_OPTION_PAUSE_OTHERS.");
        } else {
            TIZEN_LOGE("AudioSession mode has been not set to ASM_SESSION_OPTION_PAUSE_OTHERS with error = 0x%X", error);
        }
    }
#endif

    return true;
}

bool AudioSessionManagerGStreamerTizen::unregisterAudioSessionManager()
{
    int error = 0;
    if (!ASM_unregister_sound(m_handle, m_eventType, &error)) {
        LOG_MEDIA_MESSAGE("unregister() is failed 0x%X\n", error);
        return false;
    }

    return true;
}

int AudioSessionManagerGStreamerTizen::getSoundState(ASM_sound_states_t* state)
{
    int error = 0;
    if (!ASM_get_sound_state( m_handle, m_eventType, state, &error))
        LOG_MEDIA_MESSAGE("getSoundState state is failed 0x%X", error);

    return error;
}

bool AudioSessionManagerGStreamerTizen::setSoundState(ASM_sound_states_t state)
{
    int error = 0;
    if (!ASM_set_sound_state(m_handle, m_eventType, state, ASM_RESOURCE_NONE, &error)) {
        LOG_MEDIA_MESSAGE("setSoundState state = [%d] failed 0x%X", state, error);
        return false;
    }

    return true;
}

#if ENABLE(TIZEN_TV_ASM_READY)
bool AudioSessionManagerGStreamerTizen::allocateResources(GstElement* pipeline)
{
    TIZEN_LOGD("Will allocate ASM resources.");
    GList* list = NULL;
    gst_element_query_resource(pipeline, &list);
    m_resources.requested_resources_num = 0;
    for (GList* element(list); element != NULL; element = element->next) {
        m_resources.device_category[m_resources.requested_resources_num] = static_cast<ASM_resource_t>(GPOINTER_TO_INT(element->data));
        m_resources.access_right[m_resources.requested_resources_num] = ASM_RESOURCE_ACCESS_RIGHT_EXCLUSIVE;
        ++m_resources.requested_resources_num;
    }
    g_list_free(list);
    int error(0);
    m_allocated = ASM_allocate_resources(m_handle, &m_resources, false, &error);
    if (error != 0)
        TIZEN_LOGD("Error allocating ASM resources.");
    return m_allocated;
}

bool AudioSessionManagerGStreamerTizen::deallocateResources()
{
    TIZEN_LOGD("Will deallocate ASM resources.");
    ASSERT(m_allocated);
    int error(0);
    // No good rollback from failure during resource deallocation.
    // The only smart move in this case would be to kill process,
    // but do we really want to do it?
    // Hence we ignore possible errors during resource deallocation.
    m_allocated = !ASM_deallocate_resources(m_handle, &m_resources, &error);
    if (error != 0)
        TIZEN_LOGD("Error deallocating ASM resources.");
    return !m_allocated;
}
#endif // ENABLE(TIZEN_TV_ASM_READY)

}
#endif
