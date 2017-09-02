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

#include "crypto_interface.h"
#include "db_interface.h"
#include <libnfe/nfe.h>
#include <libnfe/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libp11.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>


static int smartcard_login(const char *password, PKCS11_SLOT **s, 
		unsigned int *nc, PREFS *prefs){

	PKCS11_CTX *ctx;
	PKCS11_SLOT *slots;
	
	int rc = 0, logged_in;
	unsigned int nslots;

	ctx = PKCS11_CTX_new();

	/* load pkcs #11 module */
	rc = PKCS11_CTX_load(ctx, prefs->card_reader_lib);
	if (rc) {
		fprintf(stderr, "loading pkcs11 engine failed: %s\n",
			ERR_reason_error_string(ERR_get_error()));
		rc = 1;
		return -ENOCRYPTOLIB;
	}
	/* get information on all slots */
	rc = PKCS11_enumerate_slots(ctx, &slots, &nslots);
	if (rc < 0) {
		fprintf(stderr, "no slots available\n");
		rc = 2;
		return -ENOSLOT;
	}
	/* get first slot with a token */
	*s = PKCS11_find_token(ctx, slots, nslots);
	if (*s == NULL || (*s)->token == NULL) {
		fprintf(stderr, "no token available\n");
		rc = 3;
		return -ENOTOKEN;
	}

	/* check if user is logged in */
	rc = PKCS11_is_logged_in(*s, 0, &logged_in);
	if (rc != 0) {
		fprintf(stderr, "PKCS11_is_logged_in failed\n");
		return -ELIBP11;
	}

	/* perform pkcs #11 login */
	rc = PKCS11_login(*s, 0, password);
	//memset(password, 0, strlen(password));
	if (rc != 0) {
		fprintf(stderr, "PKCS11_login failed\n");
		return -ELIBP11;
	}

	/* check if user is logged in */
	rc = PKCS11_is_logged_in(*s, 0, &logged_in);
	if (rc != 0) {
		fprintf(stderr, "PKCS11_is_logged_in failed\n");
		return -ELIBP11;
	}
	if (!logged_in) {
		fprintf(stderr, "PKCS11_is_logged_in says user is not logged in, expected to be logged in\n");
		return -ELIBP11;
	}
	return 0;
}

static int get_private_key_a3(EVP_PKEY **k, X509 **c, const char *password,
		PREFS *prefs){
	unsigned int ncerts;
	int rc;
	PKCS11_SLOT *slot;
	PKCS11_CERT *certs, *authcert;
	PKCS11_KEY *authkey;

	rc = smartcard_login(password, &slot, &ncerts, prefs);
	if(rc)
		return -ELIBP11;

	/* get all certs */
	rc = PKCS11_enumerate_certs(slot->token, &certs, &ncerts);
	if (rc) {
		fprintf(stderr, "PKCS11_enumerate_certs failed\n");
		return -ELIBP11;
	}
	if (ncerts <= 0) {
		fprintf(stderr, "no certificates found\n");
		return -ELIBP11;
	}
	int i;
	for(i = 0;i < ncerts; i++){
		authcert=&certs[i];
		authkey = PKCS11_find_key(authcert);
		if(authkey != NULL)
			break;
	}
	*c = X509_dup(authcert->x509);
	if (authkey == NULL) {
		fprintf(stderr, "no key matching certificate available\n");
		return -ELIBP11;
	}
	*k = PKCS11_get_private_key(authkey);
	return 0;
}

static int get_private_key_a1(EVP_PKEY **k, X509 **c, const char *password,
		PREFS *prefs){
	char *file = prefs->cert_file;
	FILE *p12_file;
	PKCS12 *p12_cert = NULL;
	STACK_OF(X509) *additional_certs = NULL;

	p12_file = fopen(file, "rb");
	d2i_PKCS12_fp(p12_file, &p12_cert);
	fclose(p12_file);

	PKCS12_parse(p12_cert, password, k, c, &additional_certs);
	return 0;
}

int get_private_key(EVP_PKEY **k, X509 **c, const char *password){
	PREFS *prefs = get_prefs();
	switch(prefs->cert_type){
		case CERT_TYPE_A1:
			return get_private_key_a1(k, c, password, prefs);
			break;
		case CERT_TYPE_A3:
			return get_private_key_a3(k, c, password, prefs);
			break;
	}
}
