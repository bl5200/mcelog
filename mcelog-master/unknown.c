/* Copyright (C) 20014 Intel Corporation
   Author: Rui Wang
   Handle all other unknown error requests.

   mcelog is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; version
   2.

   mcelog is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should find a copy of v2 of the GNU General Public License somewhere
   on your Linux system. */
#define _GNU_SOURCE 1
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "memutil.h"
#include "mcelog.h"
#include "config.h"
#include "trigger.h"
#include "unknown.h"

static char *unknown_trigger;

enum {
	MAX_ENV = 20,
};

void unknown_setup(void)
{
	unknown_trigger = config_string("socket", "unknown-threshold-trigger");
	if (unknown_trigger && trigger_check(unknown_trigger) < 0) {
		SYSERRprintf("Cannot access unknown threshold trigger `%s'",
				unknown_trigger);
		exit(1);
	}
}

void run_unknown_trigger(int socket, int cpu, struct mce *log)
{
	int ei = 0;
	char *env[MAX_ENV];
	int i;
	char *msg;
	char *location;

	if (socket >= 0)
		asprintf(&location, "CPU %d on socket %d", cpu, socket);
	else
		asprintf(&location, "CPU %d", cpu);
	asprintf(&msg, "%s received unknown error", location);
	asprintf(&env[ei++], "LOCATION=%s", location);
	free(location);

	if (!unknown_trigger)
		goto out;

	if (socket >= 0)
		asprintf(&env[ei++], "SOCKETID=%d", socket);
	asprintf(&env[ei++], "MESSAGE=%s", msg);
	asprintf(&env[ei++], "CPU=%d", cpu);
	asprintf(&env[ei++], "STATUS=%llx", log->status);
	asprintf(&env[ei++], "MISC=%llx", log->misc);
	asprintf(&env[ei++], "ADDR=%llx", log->addr);
	asprintf(&env[ei++], "MCGSTATUS=%llx", log->mcgstatus);
	asprintf(&env[ei++], "MCGCAP=%llx", log->mcgcap);
	env[ei] = NULL;
	assert(ei < MAX_ENV);

	run_trigger(unknown_trigger, NULL, env);
	for (i = 0; i < ei; i++)
		free(env[i]);
out:
	free(msg);
}

