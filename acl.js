
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
	this.resources = {};
	this.roles = {};
	this.list = {};
};

Acl.prototype.setDefaultAction = function(action)
{
	this.defaultAction = action;
};

Acl.prototype.getName = function()
{
	return this.name;
};

Acl.prototype.addRole = function(name)
{
	if (typeof this.roles[name] == "undefined") {
 		this.roles[name] = new Roles(name);
 		return this;
 	}
 	return this;
};

Acl.prototype.getRoles = function()
{
	var allRoles = [];
	for (role in this.roles) {
		allRoles[allRoles.length] = role;
	}
	return allRoles;
};

Acl.prototype.countRoles = function()
{
	return this.roles.length;
}

Acl.prototype.isRole = function(name)
{
	return typeof this.roles[name] != "undefined";
};

Acl.prototype.addResource = function(name)
{
	if (typeof this.resources[name] == "undefined") {
 		this.resources[name] = new Resources(name);
 		return this;
 	}
 	return this;
};

Acl.prototype.getResources = function()
{
	var allResources = [];
	for (resource in this.resources) {
		allResources[allResources.length] = resource;
	}
	return allResources;
};

Acl.prototype.countResources = function()
{
	return this.resources.length;
}

Acl.prototype.isResource = function(name)
{
	return typeof this.resources[name] != "undefined";
};

Acl.prototype.allow = function(role, resource, action)
{

	if (typeof this.roles[role] == 'undefined') {
		throw new Error('This is an exception');
	}

	if (typeof this.list[role] == 'undefined') {
		this.list[role] = {};
	};

	if (typeof this.list[role][resource] == 'undefined') {
		this.list[role][resource] = {};
	};

	this.list[role][resource][action] = true;

	return this;
};

Acl.prototype.deny = function(role, resource, permission)
{

	if (typeof this.roles[role] == 'undefined') {
		throw new Error('This is an exception');
	}

	if (typeof this.list[role] == 'undefined') {
		this.list[role] = {};
	};

	if (typeof this.list[role][resource] == 'undefined') {
		this.list[role][resource] = {};
	};

	this.list[role][resource][permission] = false;

	return this;
};

Acl.prototype.checkPermission = function(role, resource, permission)
{
	if (typeof this.list[role] == 'undefined') {
		return false;
	};

	if (typeof this.list[role][resource] == 'undefined') {
		return false;
	};

	if (typeof this.list[role][resource][permission] == 'undefined') {
		return false;
	}

	return this.list[role][resource][permission];
};

Acl.prototype.allowed = function(role, resource, permission)
{
	if (typeof permission == 'array') {
		var allowed = true;
		for (var i = 0; i <= permission.length; i++) {
			this.checkPermission(role, resource, permission);
		}
	} else {
		return this.checkPermission(role, resource, permission);
	}
};
