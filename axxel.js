
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

var axxel = {

	_acls: {},

	getAcl: function(name)
	{
		if (typeof axxel._acls[name] != "undefined") {
			return axxel._acls[name];
		}
		var acl = new Acl(name);
		axxel._acls[name] = acl;
		return acl;
	},

	isAcl: function(name)
	{
		if (typeof axxel._acls[name] != "undefined") {
			return true;
		}
		return false;
	}

};
