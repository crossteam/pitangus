/* Copyright (c) 2016 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of Pitangus.
 *
 * Pitangus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pitangus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Pitangus.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "tool_db_interface.h"
#include "tool_db.h"
#include <pitangus/errno.h>
#include <sqlite3.h>

int set_db(char *path){
	int rc = 0;
	sqlite3 *d;
	db_file = path;
	rc = sqlite3_open(db_file, &d);
	if(rc)
		return -ESQL;
	db = d;
	return rc;
}
