
Axxel - Acl Memory Cache
========================

Axxel is an in-memory cache server for Access-Control-Lists.

Axxel provides a specialized server for ACLs allowing web applications to check permissions in a faster and scalable way.

Axxel works as a daemon which interact with clients using JSON messages. Networking in Axxel is powered
by libevent2 providing non-blocking/asynchronous I/O.

Axxel does not have persistence, it uses in-memory HashTables to perform queries over the Access-Control-Lists 
resulting in a high performance.

#### Requirements
We need some packages previously installed.

* libevent2
* json-c