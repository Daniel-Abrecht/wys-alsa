/*
 * copyright (C) 2018, 2019 Purism SPC
 *
 * This file is part of Wys.
 *
 * Wys is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Wys is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Wys.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Bob Ham <bob.ham@puri.sm>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "wys-audio.h"
#include "util.h"

#include <glib/gi18n.h>
#include <glib-object.h>

struct alsaloop {
  int alsaloop_pid;
};

struct _WysAudio
{
  GObject parent_instance;

  gchar *codec;
  gchar *modem;
  
  struct alsaloop modem_to_speaker;
  struct alsaloop mic_to_modem;
};

G_DEFINE_TYPE (WysAudio, wys_audio, G_TYPE_OBJECT);

enum {
  PROP_0,
  PROP_CODEC,
  PROP_MODEM,
  PROP_LAST_PROP,
};
static GParamSpec *props[PROP_LAST_PROP];

static void
set_property (GObject      *object,
              guint         property_id,
              const GValue *value,
              GParamSpec   *pspec)
{
  WysAudio *self = WYS_AUDIO (object);

  (void)self; // Currently unused, may become useful later

  switch (property_id) {
  case PROP_CODEC:
    self->codec = g_value_dup_string (value);
    break;

  case PROP_MODEM:
    self->modem = g_value_dup_string (value);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
constructed (GObject *object)
{
  GObjectClass *parent_class = g_type_class_peek (G_TYPE_OBJECT);
  WysAudio *self = WYS_AUDIO (object);

  (void)self; // Currently unused, may become useful later

  parent_class->constructed (object);
}

static void wys_destroy_alsaloop (struct alsaloop *aloop);

static void
dispose (GObject *object)
{
  GObjectClass *parent_class = g_type_class_peek (G_TYPE_OBJECT);
  WysAudio *self = WYS_AUDIO (object);

  wys_destroy_alsaloop(&self->modem_to_speaker);
  wys_destroy_alsaloop(&self->mic_to_modem);

  parent_class->dispose (object);
}


static void
finalize (GObject *object)
{
  GObjectClass *parent_class = g_type_class_peek (G_TYPE_OBJECT);
  WysAudio *self = WYS_AUDIO (object);

  g_free (self->modem);
  g_free (self->codec);

  parent_class->finalize (object);
}


static void
wys_audio_class_init (WysAudioClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = set_property;
  object_class->constructed  = constructed;
  object_class->dispose      = dispose;
  object_class->finalize     = finalize;

  props[PROP_CODEC] =
    g_param_spec_string ("codec",
                         _("Codec"),
                         _("The ALSA card name for the codec"),
                         "sgtl5000",
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);

  props[PROP_MODEM] =
    g_param_spec_string ("modem",
                         _("Modem"),
                         _("The ALSA card name for the modem"),
                         "SIMcom SIM7100",
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, PROP_LAST_PROP, props);
}

static void
wys_audio_init (WysAudio *self)
{
  self->modem_to_speaker.alsaloop_pid = -1;
  self->mic_to_modem.alsaloop_pid = -1;
}

WysAudio *
wys_audio_new (const gchar *codec,
               const gchar *modem)
{
  return g_object_new (WYS_TYPE_AUDIO,
                       "codec", codec,
                       "modem", modem,
                       NULL);
}

static void 
wys_create_alsaloop (struct alsaloop *aloop, const gchar *from, const gchar *to)
{
  int pid = fork();
  if(pid == -1)
    return;
  if(pid){
    aloop->alsaloop_pid = pid;
  }else{
    execlp("wys-connect", "wys-connect", from, to, (char*)0);
    abort();
  }
}

void
wys_audio_ensure_loopback (WysAudio     *self,
                           WysDirection  direction)
{
  switch(direction){
    case WYS_DIRECTION_FROM_NETWORK: wys_create_alsaloop(&self->modem_to_speaker, self->modem, self->codec); break;
    case WYS_DIRECTION_TO_NETWORK:   wys_create_alsaloop(&self->mic_to_modem    , self->codec, self->modem); break;
  }
}

static void
wys_destroy_alsaloop (struct alsaloop *aloop)
{
  if(aloop->alsaloop_pid <= 0)
    return;
  kill(aloop->alsaloop_pid, SIGTERM);
  aloop->alsaloop_pid = -1;
}


void
wys_audio_ensure_no_loopback (WysAudio     *self,
                              WysDirection  direction)
{
  switch(direction){
    case WYS_DIRECTION_FROM_NETWORK: wys_destroy_alsaloop(&self->modem_to_speaker); break;
    case WYS_DIRECTION_TO_NETWORK:   wys_destroy_alsaloop(&self->mic_to_modem    ); break;
  }
}
