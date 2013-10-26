
/**
 *
 * Axxel - Acl memory caching
 *
 * Copyright (c) 2013 Axxel Team
 *
 * Use and distribution licensed under the MIT license.
 * See the LICENSE file for full text.
 *
 * Authors: Andres Gutierrez <andres@axxeld.com>
 */

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "js/js/src/jsapi.h"

#include "json/json.h"
#include "hash.h"
#include "axxel.h"
#include "protocol.h"
#include "logger.h"

/**
 * This function is called when a read event is generated on a network socket
 */
void handle_read_cb(struct bufferevent *buffer_ev, void *ctx) {

	JSContext *cx;
	struct evbuffer *input = bufferevent_get_input(buffer_ev);
	struct evbuffer *output = bufferevent_get_output(buffer_ev);
	json_object *response;
	char *line, *pend;
	const char *response_json;
	size_t n;
	long length;
	int status;
	p_hash_table *acl_lists;

	acl_lists = (p_hash_table *) ctx;
	cx = init_proto();

	while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF))) {
		response = parse_proto(cx, acl_lists, line, n);
		response_json = json_object_to_json_string(response);
		evbuffer_add(output, response_json, strlen(response_json));
		evbuffer_add(output, SL("\n"));
		free(line);
		json_object_put(response);
		break;
	}

}

void handle_event_cb(struct bufferevent *bev, short events, void *ctx)
{
	if (events & BEV_EVENT_ERROR) {
		perror("Error from bufferevent");
	}
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
		bufferevent_free(bev);
	}
}

void accept_connection_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx)
{
	struct event_base *base = evconnlistener_get_base(listener);
	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	//fprintf(stderr, "%s\n", address->s_addr);

	bufferevent_setcb(bev, handle_read_cb, NULL, handle_event_cb, ctx);

	bufferevent_enable(bev, EV_READ|EV_WRITE);
}

void accept_error_cb(struct evconnlistener *listener, void *ctx)
{
	struct event_base *base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();
	fprintf(stderr, "Error %d (%s).\n", err, evutil_socket_error_to_string(err));

	event_base_loopexit(base, NULL);
}

/**
 * Starts the network server
 */
int start_server(p_hash_table *acls)
{

	struct event_base *base;
	struct evconnlistener *listener;
	struct sockaddr_in sin;
	struct sockaddr_un sun;

	int port = 1589;

	/*if (argc > 1) {
		port = atoi(argv[1]);
	}*/

	base = event_base_new();
	if (!base) {
		puts("base error");
		return 1;
	}

	memset(&sin, 0, sizeof(sin));

	/* This is an INET address */
	sin.sin_family = AF_INET;

	/* Listen on 0.0.0.0 */
	sin.sin_addr.s_addr = htonl(0);

	/* Listen on the given port. */
	sin.sin_port = htons(port);

	//memset(&sun, 0, sizeof(sun));
	//sun.sun_family = AF_LOCAL;
	//strcpy(sun.sun_path, "/tmp/axxel-socket");

	listener = evconnlistener_new_bind(base, accept_connection_cb, acls, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1, (struct sockaddr*)&sin, sizeof(sin));
	if (!listener) {
		perror("can't create listener");
		return 1;
	}
	evconnlistener_set_error_cb(listener, accept_error_cb);

	event_base_dispatch(base);
	return 0;
}
