// LibeventTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "event2/event-config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#endif
#include <errno.h>

#include "event2/event.h"
#include "event2/event_compat.h"
#include "event2/event_struct.h"

int called = 0;

#define NEVENT	20000

struct event *ev[NEVENT];

static int
	rand_int(int n)
{
#ifdef WIN32
	return (int)(rand() % n);
#else
	return (int)(random() % n);
#endif
}

static void
	time_cb(evutil_socket_t fd, short event, void *arg)
{
	struct timeval tv;
	int i, j;

	called++;
	printf("%d \n", called);

	if (called < 10*NEVENT) {
		for (i = 0; i < 10; i++) {
			j = rand_int(NEVENT);
			tv.tv_sec = 0;
			tv.tv_usec = rand_int(50000);
			if (tv.tv_usec % 2)
				evtimer_add(ev[j], &tv);
			else
				evtimer_del(ev[j]);
		}
	}
}

int
	main(int argc, char **argv)
{
	struct timeval tv;
	int i;
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	(void) WSAStartup(wVersionRequested, &wsaData);
#endif

	/* Initalize the event library */
	event_init();

	for (i = 0; i < NEVENT; i++) {
		ev[i] = (struct event*)malloc(sizeof(struct event));

		/* Initalize one event */
		evtimer_set(ev[i], time_cb, ev[i]);
		tv.tv_sec = 0;
		tv.tv_usec = rand_int(50000);
		evtimer_add(ev[i], &tv);
	}

	event_dispatch();

	return (called < NEVENT);
}