
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

Acl.prototype.grant = function(role, resource, permission, action)
{
	if (typeof this.roles[role] == 'undefined') {
		throw new Error('Role "' + role + '" does not exist');
	}

	if (typeof this.list[role] == 'undefined') {
		this.list[role] = {};
	};

	if (typeof this.list[role][resource] == 'undefined') {
		this.list[role][resource] = {};
	};

	if (typeof permission == 'object') {
		for (var i = 0; i < permission.length; i++) {
			this.list[role][resource][permission[i]] = action;
		}
		return;
	}

	this.list[role][resource][permission] = action;
};

Acl.prototype.allow = function(role, resource, permission)
{
	this.grant(role, resource, permission, true);
	return this;
};

Acl.prototype.deny = function(role, resource, permission)
{
	this.grant(role, resource, permission, false);
	return this;
};

Acl.prototype.checkPermission = function(role, resource, permission)
{
	if (typeof permission != "string") {
		print("not here (0)");
		return false;
	}

	if (typeof this.list[role] == 'undefined') {
		print("not here (1)");
		return false;
	};

	if (typeof this.list[role][resource] == 'undefined') {
		print("not here (2)");
		return false;
	};

	if (typeof this.list[role][resource][permission] == 'undefined') {
		if (typeof this.list[role][resource]['*'] != 'undefined') {
			return this.list[role][resource]['*'];
		}
		print("not here (3)");
		return false;
	}

	return this.list[role][resource][permission];
};

Acl.prototype.allowed = function(role, resource, permission)
{
	if (typeof permission == 'object') {
		var allowed = true;
		for (var i = 0; i < permission.length; i++) {
			allowed = this.checkPermission(role, resource, permission[i]) && allowed;
		}
		return allowed;
	}
	return this.checkPermission(role, resource, permission);
};

