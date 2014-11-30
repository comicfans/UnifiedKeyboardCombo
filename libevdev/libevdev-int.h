/*
 * Copyright © 2013 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef LIBEVDEV_INT_H
#define LIBEVDEV_INT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "libevdev-util.h"

#define MAX_NAME 256
#define ABS_MT_MIN ABS_MT_SLOT
#define ABS_MT_MAX ABS_MT_TOOL_Y
#define ABS_MT_CNT (ABS_MT_MAX - ABS_MT_MIN + 1)
#define LIBEVDEV_EXPORT __attribute__((visibility("default")))
#define ALIAS(_to) __attribute__((alias(#_to)))

/**
 * Sync state machine:
 * default state: SYNC_NONE
 *
 * SYNC_NONE → SYN_DROPPED or forced sync → SYNC_NEEDED
 * SYNC_NEEDED → libevdev_next_event(LIBEVDEV_READ_FLAG_SYNC) → SYNC_IN_PROGRESS
 * SYNC_NEEDED → libevdev_next_event(LIBEVDEV_READ_FLAG_SYNC_NONE) → SYNC_NONE
 * SYNC_IN_PROGRESS → libevdev_next_event(LIBEVDEV_READ_FLAG_SYNC_NONE) → SYNC_NONE
 * SYNC_IN_PROGRESS → no sync events left → SYNC_NONE
 *
 */
enum SyncState {
	SYNC_NONE,
	SYNC_NEEDED,
	SYNC_IN_PROGRESS,
};

struct mt_sync_state {
	int code;
	int val[];
};

#define max_mask(uc, lc) \
	case EV_##uc: \
			*mask = dev->lc##_bits; \
			max = libevdev_event_type_get_max(type); \
		break;




#undef max_mask
#endif

