/**
*	Projekt ISA - 2019/2020
*	Varianta - Whois tazatel
*	Autor - Adrian Boros (xboros03)
*	Datum - 
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h> //
#include <string.h> //
#include <netdb.h> //
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/utsname.h> //
#include <arpa/nameser.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <fcntl.h>

#define BUF_SIZE 128
/**
*	Struktury na spracovane argumenty a ulozenie IP adries pre rozne potreby
*/
struct Arguments {
   char hostname[BUF_SIZE];
   char whois_server[BUF_SIZE];
   char dns_server[BUF_SIZE];
};

struct IP_addresses {
   char IP_add_4[INET6_ADDRSTRLEN];
   char IP_add_6[INET6_ADDRSTRLEN];
   char reversed_ip[INET6_ADDRSTRLEN + 15];
   char ptr_host[INET6_ADDRSTRLEN];
   bool is_IPv4 = false;
};

Arguments args_proc;
IP_addresses ip_addr;

const char *helpMSG = "***************************************************************************\n\t\t\t\t" 
            		"USAGE\n***************************************************************************\n"
            		"./isa-tazatel {-q <IP|hostname>} {-w <IP|hostname of WHOIS server>} [-d <IP>]\n"
            		"***************************************************************************\n"
            		"-q <IP|hostname>   IP address or hostname for which we are looking for info\n"
            		"-w <IP|hostname>   Whois server which will be queried\n"
            		"-d <IP>            IP address of DNS server which will be queried\n"
            		"                   by default, DNS resolver is used from the operating system\n"
            		"***************************************************************************\n"
            		"Note:              { argument } = compulsory argument\n"
            		"                   [ argument ] = optional argument\n"
            		"***************************************************************************\n";

void hostname_to_ip(char *host, char *ip, bool is_whois_server);
int is_IP_addr(char *host);
void DNS_query(char *host, int type, char* server, const char* output_type);
char *reverse_string(char *str, int length);
void reverse_ip(char *ip);
void modify_IPv6_rev_DNS(char *ip);
void args_process(int argc, char *argv[]);
void whois_query(char *query , char *server, char **odpoved);
void ipv6_expander(const struct in6_addr * addr);
bool print_whois_data(char *response);
void whois_query_edit(char *hostname, char* result);
void perform_whois_query(char *host, char *server, char *response, char *ip_address);

int main(int argc, char *argv[])  
{
	args_process(argc, argv);
	char *response = NULL;
	if (is_IP_addr(args_proc.hostname) == 0)	// Ak je zadany hostname
    {
    	//hostname_to_ip(args_proc.hostname, ip_addr.IP_add_4, false);
    	printf("=== DNS ===\n");
    	DNS_query(args_proc.hostname, ns_t_a, args_proc.dns_server, "A:");
    	DNS_query(args_proc.hostname, ns_t_aaaa, args_proc.dns_server, "AAAA:");
		DNS_query(args_proc.hostname, ns_t_mx, args_proc.dns_server, "MX:");
		DNS_query(args_proc.hostname, ns_t_cname, args_proc.dns_server, "CNAME:");
		DNS_query(args_proc.hostname, ns_t_ns, args_proc.dns_server, "NS:");
		DNS_query(args_proc.hostname, ns_t_soa, args_proc.dns_server, "SOA:");
		if (is_IP_addr(ip_addr.IP_add_4) == 1)
			reverse_ip(ip_addr.IP_add_4);
		DNS_query(ip_addr.reversed_ip, ns_t_ptr, args_proc.dns_server, "PTR:");
    }
    else if (is_IP_addr(args_proc.hostname) == 1 || is_IP_addr(args_proc.hostname) == 2)
    {
    	if (is_IP_addr(args_proc.hostname) == 2)	// Ak je to IPv6 adresa
    	{
    		hostname_to_ip(args_proc.hostname, args_proc.hostname, false);
	    	strcpy(ip_addr.IP_add_6, args_proc.hostname);
	    	struct sockaddr_in6 sa;
	    	inet_pton(AF_INET6, ip_addr.IP_add_6, &(sa.sin6_addr));
	    	ipv6_expander(&(sa.sin6_addr));
	    	strcpy(ip_addr.reversed_ip, reverse_string(ip_addr.IP_add_6, INET6_ADDRSTRLEN)); 
	    	modify_IPv6_rev_DNS(ip_addr.reversed_ip);
	    	strcat(ip_addr.reversed_ip, "ip6.arpa");
    	}
    	else	// V opacnom pripade sa jedna o IPv4 adresu
    	{
    		hostname_to_ip(args_proc.hostname, ip_addr.IP_add_4, false);
    		reverse_ip(ip_addr.IP_add_4);	// Otoci sa IP adresa pre potreby reverzneho dotazu
    	}
    	printf("=== DNS ===\n");
    	DNS_query(ip_addr.reversed_ip, ns_t_ptr, args_proc.dns_server, "PTR:");
    	if (ip_addr.ptr_host == NULL)
    	{
    		fprintf(stderr, "ERROR: No data found\n");
    		exit(EXIT_FAILURE);
    	}
		DNS_query(ip_addr.ptr_host, ns_t_a, args_proc.dns_server, "A:");
		DNS_query(ip_addr.ptr_host, ns_t_aaaa, args_proc.dns_server, "AAAA:");
		DNS_query(ip_addr.ptr_host, ns_t_mx, args_proc.dns_server, "MX:");
		DNS_query(ip_addr.ptr_host, ns_t_cname, args_proc.dns_server, "CNAME:");
		DNS_query(ip_addr.ptr_host, ns_t_ns, args_proc.dns_server, "NS:");
		DNS_query(ip_addr.ptr_host, ns_t_soa, args_proc.dns_server, "SOA:");
    }
    hostname_to_ip(args_proc.whois_server, args_proc.whois_server, true);
    if (is_IP_addr(args_proc.hostname) == 0)
    {
    	perform_whois_query(args_proc.hostname, args_proc.whois_server, response, ip_addr.IP_add_4);
    	free(response);
    }
    else
    {
    	perform_whois_query(args_proc.hostname, args_proc.whois_server, response, ip_addr.IP_add_6);
    	free(response);
    }
    return 0;  
} 

/**
*	@brief Funkcia na prevedenie Whois dotazu a na vypis ziskanych informacii
*	Ak nie je mozne ziskat informacie z IP adresy, dotazuje sa pomocou hostname,
*	bud s "www." alebo bez
*	@param char* host - hostname ktoreho informacie sa pozaduju
*	@param char* server - whois server ktory sa ma dotazovat
*	@param char* response - premenna do ktorej sa ulozi odpoved
*	@param char* ip_address - IP adresa hostname-u ktoreho informacie sa pozaduju
*/
void perform_whois_query(char *host, char *server, char *response, char *ip_address)
{	
	whois_query(host, server, &response);
    if (print_whois_data(response) == false)
    {
    	whois_query_edit(host, host);
        whois_query(host, server, &response);
        if (print_whois_data(response) == false)
        {
           	whois_query(ip_address, server, &response);
           	if (print_whois_data(response) == false)
            {
                printf("=== WHOIS ===\n");
                printf("WARNING: No data found\n");
                free(response);
            }
        }
    }
}

/**
*	@brief Funkcia na pridanie alebo odstranenie "www." pre potreby Whois dotazu 
*	@param char* hostname - hostname z ktoreho sa ma pridat alebo odstranit "www."
*	@param char* result - vysledok po pridani alebo odstraneni "www."
*/
void whois_query_edit(char* hostname, char* result)
{
	char *domain = strstr(hostname, "www.");
	char host[(int)strlen(hostname)+5];
    if (domain == NULL)
   	{
   		strcpy(host, "www.");
   		strcat(host, hostname);
   	}
   	else
   	{
   		hostname = strstr(hostname, ".");
   		strncpy(host, hostname+1, (int)strlen(hostname));
   	}
   	strcpy(result, host);
}

/**
*	@brief Funkcia na expandovanie IPv6 adresy pre potreby reverzneho DNS lookupu
*	@param struct in6_addr * addr - obsahuje IPv6 adresu
*	Poznamka: kod prebrany a nasledne upraveny z: https://stackoverflow.com/questions/3727421/expand-an-ipv6-address-so-i-can-print-it-to-stdout
*/
void ipv6_expander(const struct in6_addr * addr)
{
    char str[40];
    char new_str[256];
    sprintf(str,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
    (int)addr->s6_addr[0], (int)addr->s6_addr[1],
    (int)addr->s6_addr[2], (int)addr->s6_addr[3],
    (int)addr->s6_addr[4], (int)addr->s6_addr[5],
    (int)addr->s6_addr[6], (int)addr->s6_addr[7],
    (int)addr->s6_addr[8], (int)addr->s6_addr[9],
    (int)addr->s6_addr[10], (int)addr->s6_addr[11],
    (int)addr->s6_addr[12], (int)addr->s6_addr[13],
    (int)addr->s6_addr[14], (int)addr->s6_addr[15]);
    
	strcpy(ip_addr.IP_add_6, str);
}

/**
*	@brief Funkcia na modifikaciu IPv6 adresy pre reverse DNS lookup
*	@brief Funkcia najprv vymaze dvojbodky medzi oktetmi a nasledne za kazdu cislovku umiestni .(bodku)
*	@param char *ip - IPv6 adresa ktora sa ma modifikovat
*/
void modify_IPv6_rev_DNS(char *ip)
{
	int j;
	for (int i=j=0; i<(int)strlen(ip); i++){
	    if (ip[i] != ':') 
		    ip[j++] = ip[i]; 
	}  
    ip[j] = '\0';	
	int k = 0;
	char new_str[64];
	for(int i = 0; i<(int)strlen(ip); i++)
	{
		new_str[k] = ip[i];
		k++;
		new_str[k] = '.';
		k++;
	}
	new_str[k] = '\0';
	strcpy(ip_addr.reversed_ip, new_str);
}

/**
*	@brief Funkcia, ktora reverzne IPv4 adresu pre reverse DNS lookup a nasledne k reverznej IP adrese prida ".in-addr.arpa"
*	takze IP adresa sa potom moze hned pouzit na reverse lookup
*	@param char *ip - IPv4 adresa ktora sa ma reverznut
*	Poznamka: kod bol prebrany z: https://stackoverflow.com/questions/16373248/convert-ip-for-reverse-ip-lookup
*/
void reverse_ip(char *ip)
{
	in_addr_t addr_ipp;
	inet_pton(AF_INET, ip, &addr_ipp);
	addr_ipp =
	   ((addr_ipp & 0xff000000) >> 24) |
	   ((addr_ipp & 0x00ff0000) >>  8) |
	   ((addr_ipp & 0x0000ff00) <<  8) |
	   ((addr_ipp & 0x000000ff) << 24);
	inet_ntop(AF_INET, &addr_ipp, ip_addr.reversed_ip, sizeof(ip_addr.reversed_ip));
	strcat(ip_addr.reversed_ip, ".in-addr.arpa");
}

/**
*	@brief Funkcia na reverzovanie retazca 
*	@param char *str - retazec ktory sa ma otocit
*	@param int lenght - dlzka retazca
*	Poznamka: kod prebrany z: https://www.includehelp.com/c-programs/c-program-to-reverse-a-string.aspx
*/
char *reverse_string(char *str, int length)
{
	char *rev_str = (char*)malloc(sizeof(char)*length);
	char revStr[length];
	int j=0;
    for(int i=(strlen(str)-1); i>=0;i--)
        revStr[j++]=str[i];
     
    // pridanie '\0' na koniec revStr
    revStr[j]='\0';
    strcpy(rev_str, revStr);
    return rev_str;
    free(rev_str);
}

/**
*	@brief Funkcia na ziskanie DNS informacii
*	@param char *host - hostname ktoreho informacie sa maju ziskat
*	@param int type - typ DNS dotazu
*	@param char *server - DNS server ktory sa ma dotazovat
*	@param char *output_type - retazec ktory sa vypisuje pri vypise informacii
*	Poznamka: pri implementacii som vyuzil stranky: 
*	https://stackoverflow.com/questions/54720831/res-query-not-following-res-init-unsets-what-was-set?fbclid=IwAR0wnW6sfuTie3VBp9JrTnDkUyNDOnseDxNeBVmxPtcioA6u9MgdhD8hFTs
*	http://www.cs.tau.ac.il/~eddiea/samples/Resolving/res_query.c.html?fbclid=IwAR2kFcPyKf3rt72rcor5qG4hOYf5Pc6EsDoA3J6tobruUd-6iDoss-3EFQE
*	https://docstore.mik.ua/orelly/networking_2ndEd/dns/ch15_02.htm?fbclid=IwAR2_RyLrrdvINMckAGAMmRaQABH1UNdx5GmqboMOkyrjSjtZoik5rtrXnvU
*/
void DNS_query(char *host, int type, char* server, const char* output_type)
{
	char dispbuf[4096];
    char DNS_server[INET_ADDRSTRLEN];   // Dlzka IPv4 adresy
    u_char resolv[1024];      			// Resolv odpoved
    int res_len=1024;					// Resolv dlzka
    ns_msg msg; 
    ns_rr rr;
    res_init();

    if ((int)strlen(server) == 0)		// Ziskanie DNS resolveru z operacneho systemu, ak nebol zadany DNS server
   	{
   		for (int i = 0 ; i < _res.nscount; i++){
        	inet_ntop(AF_INET,&(_res.nsaddr_list[i].sin_addr.s_addr), DNS_server, INET_ADDRSTRLEN);
    	}
   	}
   	else{
   		hostname_to_ip(server, server, false);
   		strcpy(DNS_server, server);
   	}

   	// Nastavenie poctu nameserverov, zdrojovej IP adresy a portu, casoveho intervalu
   	// opakovaneho prenosu a poctu opakovania prenosu
   	_res.nscount = 1;
    _res.nsaddr_list[0].sin_family = AF_INET;
    _res.nsaddr_list[0].sin_addr.s_addr = inet_addr(DNS_server);
    _res.nsaddr_list[0].sin_port = htons(53);
    _res.retrans = 1;
    _res.retry = 1;

    int l = res_query(host, ns_c_in, type, resolv, res_len);
    if (l >= 0)	// Ak sa nejake informacie nasli
    {	
    	char *output = NULL;
    	int s = ns_initparse (resolv, l, &msg);
	    s = ns_msg_size(msg);
	    int ns = ns_msg_count (msg, ns_s_an);
	    for (int j = 0; j < ns; j++) 
	    {
	        ns_parserr (&msg, ns_s_an, j, &rr);
	        ns_sprintrr (&msg, &rr, NULL, NULL, dispbuf, sizeof (dispbuf));
	        for(int i = 0; i <= (int)strlen(dispbuf); i++)	// Nahradenie tabulatoru za medzeru v ziskanych informaciach
		  	{
		  		if(dispbuf[i] == '\t') 
		  			dispbuf[i] = ' ';
			}
			if (type == ns_t_soa)	// Ziskanie SOA informacii a admin mailu
		    {
		    	char *reversed_str = NULL;
		    	int pch = strcspn (dispbuf, "(");
			    char dst[pch];
			    strncpy(dst, dispbuf, pch);
			    char *ret = strstr(dst, "SOA ");
			    if (ret != NULL)
			    {
			    	ret = strchr(ret, ' ');
					reversed_str = reverse_string(ret, strlen(ret));
					strcpy(reversed_str,strchr(reversed_str, '.'));
					reversed_str = reverse_string(reversed_str, strlen(ret));
					char *admin_mail = strrchr(reversed_str, ' ');
					printf("%s\t\t", output_type);
					printf("%s\n", reversed_str);
					for(int i = 0; i < (int)strlen(admin_mail); i++)
				  	{
				  		if(admin_mail[i] == '.')  
						{
				  			admin_mail[i] = '@';
				  			break;
				 		}
					}
					printf("admin email: \t");
					printf("%s\n", admin_mail);
			    }	
		    }
		    else if (type == ns_t_a && j == 0)
		    {
		    	output = strrchr(dispbuf, ' ');
		    	for (int i = 0; i < (int)strlen(output); ++i)
		    	{
		    		output[i] = output[i + 1];
		    	}
		    	strcpy(ip_addr.IP_add_4, output);
		    	printf("%s\t\t ", output_type);
		       	printf ("%s\n", output);
		    }
		    else if (type == ns_t_ptr)	// Ziskanie PTR odpovede a ulozenie do premennej v strukture IP_addresses
		    {
		    	output = strrchr(dispbuf, ' ');
		    	printf("%s\t\t", output_type);		
		        printf ("%s\n", output);
		        strncpy(ip_addr.ptr_host, output+1, (int)strlen(output));
		    }
		    else 	// Vypis informacii ostatnych typov
		    {
				output = strrchr(dispbuf, ' ');
		    	printf("%s\t\t", output_type);
		       	printf ("%s\n", output);
	       	}
	    }
    }
    else	// V opacnom pripade sa jedna o chybu
    {
    	printf("%s\t\t", output_type);
	    printf (" No data found\n");
    }
}


/**
*	@brief Funkcia na kontrolu ci sa jedna o hostname, IPv4 alebo IPv6 adresu
*	@param char *host - retazec ktory obsahuje IPv4, IPv6 adresu alebo hostname
*	@return - 0 ak sa jedna o hostname
*			  1 ak sa jedna o IPv4 adresu
*			  2 ak sa jedna o IPv6 adresu
*	Poznamka: funkcia prebrana a nasledne upravena z: 
*	https://stackoverflow.com/questions/3736335/tell-whether-a-text-string-is-an-ipv6-address-or-ipv4-address-using-standard-c-s
*/
int is_IP_addr(char *host)
{
	struct addrinfo hint, *res = NULL;
    int ret;

    memset(&hint, '\0', sizeof hint);

    hint.ai_family = PF_UNSPEC;
    hint.ai_flags = AI_NUMERICHOST;

    ret = getaddrinfo(host, NULL, &hint, &res);
    if (ret)
        return 0;
    if(res->ai_family == AF_INET) 
        return 1;
    else if(res->ai_family == AF_INET6)
    	return 2;

   freeaddrinfo(res);
}

/**
*	@brief Funkcia na skontrolovanie vstupnych parametrov, v pripade chybne zadanych parametrov program ihned konci
*	@param argc - pocet argumentov
*	@param argv - jednotlive argumenty
*/
void args_process(int argc, char *argv[])
{
    int opt; 
    int q_opt = -1;
    int w_opt = -1;
    if (argc > 8)
	{
		fprintf(stderr, "ERROR: Wrong number of parameters\n");
		exit(EXIT_FAILURE);
	}
    while((opt = getopt(argc, argv, ":d:w:q:h")) != -1)  
    {  
        switch(opt)  
        {  
            case 'd':  
            	strcpy(args_proc.dns_server, optarg); 
            	if (is_IP_addr(args_proc.dns_server) == 0)
            	{
            		fprintf(stderr, "ERROR: Parameter -d can be just IP address\n");  
                	exit(EXIT_FAILURE);
            	}
                break; 
            case 'h':
            	printf("%s", helpMSG);
            	exit(EXIT_SUCCESS);
            case 'w': 
            	w_opt++;
            	strcpy(args_proc.whois_server, optarg); 
                break;   
            case 'q': 
            	q_opt++; 
            	strcpy(args_proc.hostname, optarg); 
                break;   
            case ':':  
                fprintf(stderr, "ERROR: Parameter needs a value\n");  
                exit(EXIT_FAILURE);  
            case '?':  
                fprintf(stderr, "ERROR: Unknown parameter: %c\n", optopt); 
                exit(EXIT_FAILURE);  
        }  
    }
    for(; optind < argc; optind++)
    {   
    	if (argv[optind] != NULL)
    	{
    		fprintf(stderr, "ERROR: Wrong parameter used\n");
    		exit(EXIT_FAILURE);
    	} 
    }
    if (q_opt == -1 || w_opt == -1)
    {
    	fprintf(stderr, "ERROR: Missing mandatory parameter\n");
    	exit(EXIT_FAILURE);
    }
    else if (q_opt != 0 || w_opt != 0)
    {
    	fprintf(stderr, "ERROR: Param was entered more than once\n");
    	exit(EXIT_FAILURE);
    }
}

/**
*	@brief Funkcia na vykonanie WHOIS dotazu na zadany server
*	@param char *query - IP adresa ktorej informacie sa maju ziskat
*	@param char *server - WHOIS server ktory sa ma dotazovat
*	Poznamka: cast kodu prebrana a upravena z:
*	https://www.binarytides.com/c-code-to-perform-ip-whois/
*/
void whois_query(char *query , char *server, char **odpoved)
{
	char message[100], buffer[1024];
	int sock, read_size = 0, total_size = 0, opt = 0;
	
	struct sockaddr_in dest;
	struct sockaddr_in6 dest6;
	struct timeval timeout;

	// Priprava struktur na pripojenie sa na whois server
	if (ip_addr.is_IPv4 == false)
	{
	    sock = socket(AF_INET6, SOCK_STREAM, 0);
	    memset(&dest6, 0, sizeof(dest6));
	    dest6.sin6_family = AF_INET6;
	    inet_pton(AF_INET6, server, &dest6.sin6_addr);
	    dest6.sin6_port = htons(43);
	}
	else if (ip_addr.is_IPv4 == true)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		memset(&dest, 0, sizeof(dest));
		dest.sin_family = AF_INET;
		inet_pton(AF_INET, server, &dest.sin_addr);
		dest.sin_port = htons(43);
	}

	// Ziskanie socket flagov
	if ((opt = fcntl (sock, F_GETFL, NULL)) < 0) {
		exit(EXIT_FAILURE);
	}

	timeout.tv_sec  = 5;  // Po 5 sekundach connect() sa timeoutne
	timeout.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	// Pripojenie na whois server
	if (ip_addr.is_IPv4 == false)
	{
		if(connect(sock, (const struct sockaddr*) &dest6, sizeof(dest6)) < 0)
		{
			fprintf(stderr, "ERROR: Connection failed, no data available\n");
			exit(EXIT_FAILURE);
		}
	}
	else if (ip_addr.is_IPv4 == true)
	{
		if(connect(sock, (const struct sockaddr*) &dest, sizeof(dest)) < 0)
		{
			fprintf(stderr, "ERROR: Connection failed, no data available\n");
			exit(EXIT_FAILURE);
		}
	}
	
	// Resetovanie socket flagov
	if (fcntl (sock, F_SETFL, opt) < 0) {
		exit(EXIT_FAILURE);
	}
	
	// Posielanie spravy whois serveru
	sprintf(message, "%s\r\n" , query);
	if(send(sock, message, strlen(message), 0) < 0)
	{
		fprintf(stderr, "ERROR: Data sending failed\n");
		exit(EXIT_FAILURE);
	}
	
	// Ziskanie odpovede
	while((read_size = recv(sock, buffer, sizeof(buffer), 0)))
	{
		//total_size+=read_size;
		if ((int)total_size == 1)
		{
			fprintf(stderr, "WARNING: Missing WHOIS information\n");
			exit(EXIT_FAILURE);
		}
		else 
		{
			*odpoved = (char*)realloc(*odpoved, read_size + total_size);
			memcpy(*odpoved + total_size, buffer , read_size);
			total_size += read_size;
		}
	}
	*odpoved = (char*)realloc(*odpoved, total_size+1);
	*(*odpoved+total_size) = '\0';
	
	fflush(stdout);	
	close(sock);
}

/**
*	@brief Funkcia na upravu a nasledny vypis ziskanych informacii od Whois serveru
*	@param char* response - premenna v ktorej sa nachadza odpoved
*	@return - vrati sa false ak sa neziskali ziadne informacie, v opacnom pripade sa vracia true
*/
bool print_whois_data(char *response)
{
	char data[strlen(response)];
	int counter = 0;
	strcpy(data, response);
	char *ptr = strtok(data, "\n"); 	// Rozdelenie ziskanych dat po riadkoch
	char dest[strlen(response)] = {};
	while (ptr != NULL)	 // Ak je prvym znakom v riadku %, #, medzera alebo carriage return tak tie riadky sa nevypisu
	{
	    if (ptr[0] != '%' && ptr[0] != '#' && ptr[0] != '\r' && ptr[0] != ' ')
	    {
	    	strcat(dest, ptr);
	    	strcat(dest, "\n");
	    	counter++;	
	    	if (counter == 10)
	    	{
	    		strcat(dest, "\n");		// Po kazdych 10 riadkoch informacii sa do vypisu prida prazdny riadok pre vacsiu prehladnost
	    		counter = 0;
	    	}
	    }
	    ptr = strtok (NULL, "\n");
	}
	dest[strlen(dest)-1] = '\0';
	if ((int)strlen(dest) == 0) {	// Ak sa nenasli informacie vracia sa false
		return false;
	}
	else {
		printf("=== WHOIS ===\n");	// V opacnom pripade sa vracia true a vypisuju sa informacie
		printf("%s\n", dest);
		free(response);
		return true;
	}
}

/**
*	@brief Funkcia na prevod hostname-u na IP adresu
*	@param hostname - nazov domeny ktory sa ma prelozit
*	@return ip - IP adresa domeny
*	Poznamka: cast kodu prebrana z: https://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/
*/
void hostname_to_ip(char *host, char *ip, bool is_server)
{
	struct addrinfo hints, *res = NULL, *p = NULL;
	struct sockaddr_in *h;
	struct sockaddr_in6 *h6;
    int ret;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     // Povoluje IPv4 alebo IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = 0;
    hints.ai_protocol = 0;      

    ret = getaddrinfo(host, NULL, &hints, &res);	// Ak sa jedna o zly hostname alebo IP adresu, konci sa chybou
    if (ret) {
        fprintf(stderr, "ERROR: Wrong IP address or hostname\n");
        exit(EXIT_FAILURE);
    }

	for (p=res; p!=NULL; p=p->ai_next) { 
    	struct in_addr *addr;  
	    if (p->ai_family == AF_INET) {	// Ak je zadana IPv4 adresa
	    	ip_addr.is_IPv4 = true;
	        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr; 
	        strcpy(ip, inet_ntoa(ipv4->sin_addr));
	    } 
	    else if (p->ai_family == AF_INET6) {  // Ak je zadany hostname ktory ma aj IPv4 aj IPv6
	    	struct hostent *he;
	    	if ((he = gethostbyname(host)) == NULL)  // IPv6
	    	{
	    		if (is_server == true)
	        	{
	        		ip_addr.is_IPv4 = false;
	        	}
	    		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr; 
		        addr = (struct in_addr *) &(ipv6->sin6_addr); 
		        inet_ntop(p->ai_family, addr, ip, sizeof ip);
		    }
	        else	//IPv4
	        {
	        	if (is_server == true)
	        	{
	        		ip_addr.is_IPv4 = true;
	        	}
	        	struct in_addr **addr_list;
				addr_list = (struct in_addr **)he->h_addr_list;
			    for(int i = 0; addr_list[i] != NULL; i++) 
			    {
			        strcpy(ip, inet_ntoa(*addr_list[i]) );
			    }
	        }
	    }
	} 
	freeaddrinfo(res);
}
