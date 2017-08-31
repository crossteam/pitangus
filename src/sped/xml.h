/* Copyright (c) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of LivreNFE.
 *
 * LivreNFE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LivreNFE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LivreNFE.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef	XML_H
#define	XML_H

#include <libxml/parser.h>
#include <libxml/tree.h>

/**
 * Get single element from XML
 */
extern char *get_xml_element(xmlDocPtr doc, char *element);


/**
 * Get subtree from XML
 */
extern char *get_xml_subtree(xmlDocPtr doc, char *xpath);

/**
 * Get single node from XML using xpath
 */
extern xmlNodePtr get_xml_node(xmlDocPtr doc, char *xpath);

#endif
