Axxel - In-Memory Acl Server
============================

Axxel provides a specialized server for ACLs allowing web applications to check permissions in a
faster and scalable way.

Axxel works as a daemon which interacts with clients using JSON messages. Networking in Axxel is powered
by [libevent2](http://libevent.org/) (as Chrome/Memcached/Tor) providing non-blocking/asynchronous I/O.

Axxel does not have disk persistence, it stores ACLs in memory
allowing clients to perform queries over the Access-Control-Lists with minimum overhead.

#### Requirements
We need some packages previously installed:

[SpiderMonkey](https://developer.mozilla.org/en-US/docs/Mozilla/Projects/SpiderMonkey)

```sh
mkdir mozilla
cd mozilla
wget http://ftp.mozilla.org/pub/mozilla.org/js/js185-1.0.0.tar.gz
tar xzf js185-1.0.0.tar.gz
rm -f js185-1.0.0.tar.gz
cd js-1.8.5/js/src
autoconf2.13
./configure
make
sudo make install
```

[libevent2](http://libevent.org/):

```sh
mkdir libevent
cd libevent
wget http://github.com/downloads/libevent/libevent/libevent-2.0.18-stable.tar.gz
tar -xzvf libevent-2.0.18-stable.tar.gz
cd libevent-2.0.18-stable
./configure
sudo make install
```

[json-c](https://github.com/json-c/json-c)

```sh
wget https://github.com/json-c/json-c/archive/master.zip
./configure
make
make install
```

### Installation
Once requirements are installed use the following command:

```sh
./install
```

### Usage
You can execute the daemon as follows:

```sh
./axxeld
```

Setting a port:

```sh
./axxeld -p 50001
```
