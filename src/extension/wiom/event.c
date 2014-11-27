/* -*- c-set-style: "K&R"; c-basic-offset: 8 -*-
 *
 * This file is part of WioM.
 *
 * Copyright (C) 2014 STMicroelectronics
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#include <stdarg.h>	/* va_*, */
#include <stdio.h>	/* fprintf(3), */
#include <assert.h>	/* assert(3), */
#include <sched.h>	/* CLONE_THREAD, */
#include <stdbool.h>	/* bool, */
#include <errno.h>	/* E*, */
#include <talloc.h>	/* talloc(3), */
#include <uthash.h>	/* UT*, */
#include <sys/queue.h>	/* SIMPLEQ, */

#include "extension/wiom/event.h"
#include "extension/wiom/wiom.h"
#include "path/path.h"
#include "arch.h"

/*

Coalescing (TODO)
==========

- "*pid* traverses *path*" hides further instances (same *pid*, same
  action, and same *path*)

- "*pid* deletes *path*" unhides all events on *path* for all pids

- "*pid* gets metadata of *path*" hides further instances (same *pid*,
  same action, and same *path*), but unhides "sets metadata of *path*"
  for all pids

- "*pid* sets metadata of *path*" hides further instances (same *pid*,
  same action, and same *path*), but unhides "gets metadata of *path*"
  for all pids

- "sets content of *path*" hides further instances (same *pid*, same
  action, and same *path*), but unhides "gets content of *path*" for
  all pids

- "gets content of *path*" hides further instances (same *pid*, same
  action, and same *path*), but unhides "sets content of *path*" for
  all pids

- "moves *path* to *path2*" unhides all events on *path* and *path2*,
  for all pids.

- "*pid* has exited" unhides all events for *pid*.

*/

/**
 * Allocate a new event, with given @pid and @action, at the end of
 * @config->history.  This function return NULL if an error occurred,
 * otherwise 0.
 */
static Event *new_event(Config *config, pid_t pid, Action action)
{
	size_t length;
	Event *event;
	void *tmp;

	if (config->history == NULL) {
		config->history = talloc_array(config, Event, 1);
		if (config->history == NULL)
			return NULL;
	}

	length = talloc_array_length(config->history);

	tmp = talloc_realloc(NULL, config->history, Event, length + 1);
	if (tmp == NULL)
		return NULL;
	config->history = tmp;

	event = &config->history[length];

	event->pid = pid;
	event->action = action;

	return event;
}

/**
 * Return a copy of @original from @config->strings cache.
 */
static const char *get_string_copy(Config *config, const char *original)
{
	HashedString *entry;

	HASH_FIND_STR(config->strings, original, entry);
	if (entry != NULL)
		return entry->string;

	entry = talloc(config, HashedString);
	if (entry == NULL)
		return NULL;

	entry->string = talloc_strdup(entry, original);
	if (entry->string == NULL)
		return NULL;

	HASH_ADD_KEYPTR(hh, config->strings, entry->string,
			talloc_get_size(entry->string) - 1, entry);

	return entry->string;
}

/**
 * Check whether @path is masked with respect to @config->options.
 */
static bool is_masked(Config *config, const char *path)
{
	bool masked = false;
	Comparison comparison;
	Item *item;

	if (config->options->masked_paths == NULL)
		return false;

	SIMPLEQ_FOREACH(item, config->options->masked_paths, link) {
		comparison = compare_paths(item->load, path);
		if (   comparison == PATHS_ARE_EQUAL
		    || comparison == PATH1_IS_PREFIX) {
			masked = true;
			break;
		}
	}

	if (!masked || config->options->unmasked_paths == NULL)
		return masked;

	SIMPLEQ_FOREACH(item, config->options->unmasked_paths, link) {
		comparison = compare_paths(item->load, path);
		if (   comparison == PATHS_ARE_EQUAL
		    || comparison == PATH1_IS_PREFIX) {
			masked = false;
			break;
		}
	}

	return masked;
}

/**
 * Record event for given @action performed by @pid.  This function
 * return -errno if an error occurred, otherwise 0.
 */
int record_event(Config *config, pid_t pid, Action action, ...)
{
	const char *path2;
	const char *path;
	Event *event;
	va_list ap;

	va_start(ap, action);

	switch (action) {
	case TRAVERSES:
	case CREATES:
	case DELETES:
	case GETS_METADATA_OF:
	case SETS_METADATA_OF:
	case GETS_CONTENT_OF:
	case SETS_CONTENT_OF:
	case EXECUTES:
		path = va_arg(ap, const char *);
		if (is_masked(config, path))
			break;

		event = new_event(config, pid, action);
		if (event == NULL)
			return -ENOMEM;

		event->load.path = get_string_copy(config, path);
		if (event->load.path == NULL)
			return -ENOMEM;

		break;

	case MOVES:
		path  = va_arg(ap, const char *);
		path2 = va_arg(ap, const char *);
		if (is_masked(config, path) && is_masked(config, path2))
			break;

		event = new_event(config, pid, action);
		if (event == NULL)
			return -ENOMEM;

		event->load.path = get_string_copy(config, path);
		if (event->load.path == NULL)
			return -ENOMEM;

		event->load.path2 = get_string_copy(config, path2);
		if (event->load.path2 == NULL)
			return -ENOMEM;
		break;

	case IS_CLONED: {
		event = new_event(config, pid, action);
		if (event == NULL)
			return -ENOMEM;

		event->load.pid = va_arg(ap, pid_t);
		if (event->load.path == NULL)
			return -ENOMEM;

		event->load.thread = (va_arg(ap, word_t) & CLONE_THREAD) != 0;
		if (event->load.path == NULL)
			return -ENOMEM;

		break;
	}

	case HAS_EXITED: {
		event = new_event(config, pid, action);
		if (event == NULL)
			return -ENOMEM;

		event->load.status = va_arg(ap, word_t);
		if (event->load.path == NULL)
			return -ENOMEM;
	}

	default:
		assert(0);
		break;
	}

	va_end(ap);

	return 0;
}

/**
 * Report all events that were stored in @config->history.
 */
void report_events(Config *config)
{
	size_t length;
	size_t i;

	if (config->history == NULL)
		return;

	length = talloc_array_length(config->history);
	for (i = 0; i < length; i++) {
		const Event *event = &config->history[i];
		switch (event->action) {
#define CASE(a) case a:							\
			fprintf(stderr, "%d %s %s\n", event->pid, #a, event->load.path); \
			break;						\

		CASE(TRAVERSES)
		CASE(CREATES)
		CASE(DELETES)
		CASE(GETS_METADATA_OF)
		CASE(SETS_METADATA_OF)
		CASE(GETS_CONTENT_OF)
		CASE(SETS_CONTENT_OF)
		CASE(EXECUTES)
#undef CASE

		case MOVES:
			fprintf(stderr, "%d moves %s to %s\n", event->pid,
				event->load.path, event->load.path2);
			break;

		case IS_CLONED:
			fprintf(stderr, "%d is cloned (%s) into %d\n", event->pid,
				event->load.thread ? "thread" : "process", event->load.pid);
			break;

		case HAS_EXITED:
			fprintf(stderr, "%d has exited (status = %ld)\n", event->pid,
				event->load.status);
			break;
		}
	}
}