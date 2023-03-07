// test_socket.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>

//#include <openssl/ssl.h>
//#include <openssl/err.h>
//#include <openssl/crypto.h>
#include <openssl/applink.c>

#include <string.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include <stdio.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define RETURN_NULL(x) if ((x)==NULL) exit (1)
#define RETURN_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
//#define RETURN_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(1); }
#define RSA_CLIENT_CA_CERT "Zombie_Spotters_Ltd.crt"



/* ---------------------------------------------------------- *
 * create_socket() creates the socket & TCP-connect to server *
 * ---------------------------------------------------------- */
int create_socket(char url_str[], BIO *out) {
  int sockfd;
  //char hostname[256] = "127.0.0.1";
  char hostname[256] = "zombieserver";

  //char    portnum[6] = "6286";
  char    portnum[6] = "17504"; // todo: configure 17504 for stunnel and 17502 for the erlang tls port
  char      proto[6] = "";
  char      *tmp_ptr = NULL;
  int           port;
  struct hostent *host;
  struct sockaddr_in dest_addr;

  WSADATA wsa;
  SOCKET s;

  if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}

  if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}

  /* ---------------------------------------------------------- *
   * Remove the final / from url_str, if there is one           *
   * ---------------------------------------------------------- */
  //if(url_str[strlen(url_str)] == '/')
  //  url_str[strlen(url_str)] = '\0';

  /* ---------------------------------------------------------- *
   * the first : ends the protocol string, i.e. http            *
   * ---------------------------------------------------------- */
  //strncpy(proto, url_str, (strchr(url_str, ':')-url_str));

  /* ---------------------------------------------------------- *
   * the hostname starts after the "://" part                   *
   * ---------------------------------------------------------- */
  //strncpy(hostname, strstr(url_str, "://")+3, sizeof(hostname));

  /* ---------------------------------------------------------- *
   * if the hostname contains a colon :, we got a port number   *
   * ---------------------------------------------------------- */
  //if(strchr(hostname, ':')) {
   // tmp_ptr = strchr(hostname, ':');
    /* the last : starts the port number, if avail, i.e. 8443 */
    //strncpy(portnum, tmp_ptr+1,  sizeof(portnum));
    //*tmp_ptr = '\0';
  //}

  printf("2.0\n");

  port = atoi(portnum);

  //if ( (host = gethostbyname(hostname)) == NULL ) {
  //  BIO_printf(out, "Error: Cannot resolve hostname %s.\n",  hostname);
  //  abort();
  //}

  printf("2.0a\n");
  /* ---------------------------------------------------------- *
   * create the basic TCP socket                                *
   * ---------------------------------------------------------- */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  printf("2.0aa\n");

  dest_addr.sin_family=AF_INET;
  dest_addr.sin_port=htons(port);
  //dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);
  dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


  printf("2.0b\n");

  /* ---------------------------------------------------------- *
   * Zeroing the rest of the struct                             *
   * ---------------------------------------------------------- */
  memset(&(dest_addr.sin_zero), '\0', 8);

  tmp_ptr = inet_ntoa(dest_addr.sin_addr);

  printf("2.1\n");

  /* ---------------------------------------------------------- *
   * Try to make the host connect here                          *
   * ---------------------------------------------------------- */
  if ( connect(sockfd, (struct sockaddr *) &dest_addr,
                              sizeof(struct sockaddr)) == -1 ) {
    BIO_printf(out, "Error: Cannot connect to host %s [%s] on port %d.\n",
             hostname, tmp_ptr, port);
  }

  printf("2.2\n");

  return sockfd;
}

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile, char* CACertFile)
{
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        printf("abc\n");
		ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        printf("bcd\n");
		ERR_print_errors_fp(stderr);
        abort();
    }

	/* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        printf("fgh\n");
		fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }

	/* set the CA certificate */
    if ( SSL_CTX_use_certificate_chain_file(ctx, CACertFile) <= 0)
    {
        printf("def\n");
		ERR_print_errors_fp(stderr);
        abort();
    }
}

void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}



int main(int argc, char* argv[])
{
	
	/*WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char *message;

	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}

	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 6286 );

	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	message = "lumba2";
	if( send(s , message , strlen(message) , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	
	printf("Hello World!\n");

	closesocket(s);
	WSACleanup();*/
	


	
	/*
	SSL_library_init();
	SSL_load_error_strings();
	
	const SSL_METHOD *meth;
	meth = TLS_method();

	SSL_CTX *ctx;
	ctx = SSL_CTX_new(meth);

	RETURN_NULL(ctx);

	if (!SSL_CTX_load_verify_locations(ctx, RSA_CLIENT_CA_CERT, NULL)) {
	  ERR_print_errors_fp(stderr);
	  exit(1);
	}

	SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL);
	SSL_CTX_set_verify_depth(ctx,1);

	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char *message;

	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}

	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 6286 );

	printf("here I am 1\n");

	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	//message = "lumba2";
	//if( send(s , message , strlen(message) , 0) < 0)
	//{
	//	puts("Send failed");
	//	return 1;
	//}

	printf("here I am 2\n");

	SSL *ssl;
	ssl = SSL_new (ctx);

	//RETURN_NULL(ssl);

	int sock = 0;
	SSL_set_fd(ssl, sock);

	printf("here I am 3\n");

	int err;
	err = SSL_connect(ssl);

	int z;
	z = SSL_get_error(ssl, err);

	if (z == SSL_ERROR_NONE)
	{
		printf("ayam");
	}

	printf("%d\n", z);

	//RETURN_SSL(err);

	printf("%d\n", err);

	printf("here I am 4\n");

	printf ("SSL connection using %s\n", SSL_get_cipher (ssl));

	/*X509 *server_cert;
	server_cert = SSL_get_peer_certificate (ssl);

	printf("here I am 5\n");

	if (server_cert)
	{
		char *str;

		printf ("Server certificate:\n");
		str = X509_NAME_oneline(X509_get_subject_name(server_cert),0,0);
		RETURN_NULL(str);
		printf ("\t subject: %s\n", str);
		free (str);
		str = X509_NAME_oneline(X509_get_issuer_name(server_cert),0,0);
		RETURN_NULL(str);
		printf ("\t issuer: %s\n", str);
		free(str);
		X509_free (server_cert);

	}
	*/


	char           dest_url[] = "zombieserver";
	BIO              *certbio = NULL;
	BIO               *outbio = NULL;
	X509                *cert = NULL;
	X509_NAME       *certname = NULL;
	const SSL_METHOD *method;
	SSL_CTX *ctx;
	SSL *ssl;
	int server = 0;
	int ret, i;

	printf("1\n");
	
	  /* ---------------------------------------------------------- *
	   * These function calls initialize openssl for correct work.  *
	   * ---------------------------------------------------------- */
	  OpenSSL_add_all_algorithms();
	  ERR_load_BIO_strings();
	  ERR_load_crypto_strings();
	  SSL_load_error_strings();

	  certbio = BIO_new(BIO_s_file());
  outbio  = BIO_new(BIO_s_file());
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);


  if(SSL_library_init() < 0)
    BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");

  method = SSLv23_client_method();

  if ( (ctx = SSL_CTX_new(method)) == NULL)
    BIO_printf(outbio, "Unable to create a new SSL context structure.\n");


   SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
	LoadCertificates(ctx, "zombieclient.crt", "zombieclient.key", "Zombie_Spotters_Ltd.crt");

   printf("2\n");

   ssl = SSL_new(ctx);

   server = create_socket(dest_url, outbio);
	
   if(server != 0)
    BIO_printf(outbio, "Successfully made the TCP connection to: %s.\n", dest_url);


   printf("3");

	printf("here I am");

	SSL_set_fd(ssl, server);

	 if ( SSL_connect(ssl) != 1 )
    BIO_printf(outbio, "Error: Could not build a SSL session to: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Successfully enabled SSL/TLS session to: %s.\n", dest_url);
	

  cert = SSL_get_peer_certificate(ssl);
  if (cert == NULL)
    BIO_printf(outbio, "Error: Could not get a certificate from: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Retrieved the server's certificate from: %s.\n", dest_url);


   /* ---------------------------------------------------------- *
   * extract various certificate information                    *
   * -----------------------------------------------------------*/
  certname = X509_NAME_new();
  certname = X509_get_subject_name(cert);


  /* ---------------------------------------------------------- *
   * display the cert subject here                              *
   * -----------------------------------------------------------*/
  BIO_printf(outbio, "Displaying the certificate subject data:\n");
  X509_NAME_print_ex(outbio, certname, 0, 0);
  BIO_printf(outbio, "\n");


  char acClientRequest[1024] = {0};

	strcpy(acClientRequest, "hujan yang turun");


  SSL_write(ssl,acClientRequest, strlen(acClientRequest));   /* encrypt & send message */


  /* ---------------------------------------------------------- *
   * Free the structures we don't need anymore                  *
   * -----------------------------------------------------------*/
  SSL_free(ssl);
  close(server);
  SSL_CTX_free(ctx);
  BIO_printf(outbio, "Finished SSL/TLS connection with server: %s.\n", dest_url);

	return 0;
}


