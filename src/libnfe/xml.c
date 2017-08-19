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

#include "xml.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <string.h>
#include <stdlib.h>

static xmlXPathObjectPtr getnodeset(xmlDocPtr doc, xmlChar *xpath){
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		printf("Error in xmlXPathNewContext\n");
		return NULL;
	}
	xmlXPathRegisterNs(context, BAD_CAST "nfe", 
		BAD_CAST "http://www.portalfiscal.inf.br/nfe");
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) {
		printf("Error in xmlXPathEvalExpression\n");
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
                printf("No result\n");
		return NULL;
	}
	return result;
}

char *get_xml_element(xmlDocPtr doc, char *element){
	xmlChar *xpath = malloc(sizeof(xmlChar) * strlen(element) + 3);
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	strcpy((char*)xpath, "//");
	strcat((char*)xpath, element);
	xmlChar *content = NULL;

	result = getnodeset(doc, xpath);
	if(result) {
		nodeset = result->nodesetval;
		content = xmlNodeListGetString(doc, 
			nodeset->nodeTab[0]->xmlChildrenNode, 1);
	}
	//free(xpath);
	//xmlFreeDoc(doc);
	xmlCleanupParser();
	return (char*)content;
}

char *get_xml_subtree(xmlDocPtr doc, char *xpath){
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	xmlBufferPtr buf;
	xmlNodeSetPtr nodeset;
	char *subtree = NULL;

	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		printf("Error in xmlXPathNewContext\n");
		return NULL;
	}
	xmlXPathRegisterNs(context, BAD_CAST "nfe", 
		BAD_CAST "http://www.portalfiscal.inf.br/nfe");
	result = xmlXPathEvalExpression((xmlChar*)xpath, context);
	if(result) {
		nodeset = result->nodesetval;
		buf = xmlBufferCreate();
		xmlNodeDump(buf, doc, nodeset->nodeTab[0], 0, 0);
		subtree = strdup((char*)buf->content);
	}
	
	xmlXPathFreeContext(context);
	return subtree;
}

xmlNodePtr get_xml_node(xmlDocPtr doc, char *xpath){
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	xmlNodePtr node = NULL;

	result = getnodeset(doc, (xmlChar*)xpath);
	if(result) {
		nodeset = result->nodesetval;
		return nodeset->nodeTab[0];
	}
	//xmlFreeDoc(doc);
	//xmlCleanupParser();
	return NULL;
}
