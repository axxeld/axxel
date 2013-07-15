Axxel - Acl In-Memory Cache Server
==================================

Axxel is an in-memory cache server for Access-Control-Lists.

Axxel provides a specialized server for ACLs allowing web applications to check permissions in a 
faster and scalable way.

Axxel works as a daemon which interacts with clients using JSON messages. Networking in Axxel is powered
by libevent2 (as Chrome/Memcached/Tor) providing non-blocking/asynchronous I/O.

Axxel does not have persistence, it uses in-memory [HashTables](http://en.wikipedia.org/wiki/Hash_table) 
allowing clients to perform queries over the Access-Control-Lists with high performance.

#### Requirements
We need some packages previously installed.

* [libevent2](http://libevent.org/)
* [json-c](https://github.com/json-c/json-c)

### Example

```ruby
require 'axxel'

a = Axxel.new("my-acl", "/tmp/axxel-socket")

# Create the ACL if it does not exist
if not a.exist? then

	a.default_action(:deny)

	a.add_role("Administrators")
	a.add_role("Guests")

	a.add_resource("Products", ["index", "search", "update"])

	a.allow("Administrators", "Products", ["index", "search", "update"])
	a.allow("Guests", "Products", "index")
end

# Check for permissions
if a.is_allowed?("Administrators", "Products", "search") then
	puts "It's Allowed"
else
	puts "It's not Allowed"
end
```

