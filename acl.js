
/**
 *
 * Axxel - Acl memory caching
 *
 * Copyright (c) 2013 Axxel Team
 *
 * Use and distribution licensed under the MIT license.
 * See the LICENSE file for full text.
 *
 * Authors: Andres Gutierrez <andres@phalconphp.com>
 */

/*function print() {
    for (var i = 0; i < arguments.length; i++) {
       var value = arguments[i];
       java.lang.System.out.print( value );
    }
    java.lang.System.out.println();
}*/

function Acl(name){
	this.name = name;
	this.resources = [];
	this.roles = [];
};

Acl.prototype.getName = function()
{
	return this.name;
};

Acl.prototype.addRole = function(name) {
	this.roles[this.roles.length] = new Roles(name);
};

Acl.prototype.getRoles = function() {
	return this.roles;
}

Acl.prototype.countRoles = function() {
	return this.roles.length;
}

Acl.prototype.isRole = function(name)
{
	var length = this.roles.length;
	for (var i = 0; i < length; i++) {
		if (this.roles[i].name == name) {
			return true;
		}
	};
	return false;
};

