#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <errno.h>
#include <poll.h>
#include <sys/ioctl.h>

/* kafka libraries */
#include "rdkafka.h"
/* cassandra interface libraries */
#include "cassandra.h"

/* default values for the cassandra and kafka clusters */
const char *cassandra_cluster = "ec2-54-69-240-86.us-west-2.compute.amazonaws.com";
const char *kafka_cluster = "ec2-54-68-169-227.us-west-2.compute.amazonaws.com:9092";
const char *topic = "sensor_data";

static int debug = 0; 
static int run = 1;
static rd_kafka_t *rk;

static void stop (int sig) { run = 0; }

static void sig_usr1 (int sig) { rd_kafka_dump(stdout, rk); }

void error(const char *msg) { perror(msg); exit(1); }

/* parses an integer from a string in the form HEADER=1234.
	input: str is the string to parse
	input: hdr is the string corresponding to the header.
	in the example within this comment str = "HEADER=1234" and hdr = "HEADER" */
static int parse_integer(char *str, char *hdr) {
	char *p = strstr(str, hdr);
	if(p == NULL) return -1;
	int i, len = strlen(hdr);
	p += len + 2; /* get past the header + "=[" */
	len = strlen(p);
	for(i=0;i<len;++i) {
		if(p[i] == ']') {
			char buff[32];
			strncpy(buff, p, i);
			printf("BUFF = [%s]\n", buff);
			i = atoi(buff);
			return i;
		}
	}
	return -2;
}

#define BUFFDIM 8192
#define ERRSTRDIM 512 
#define POLLSIZE 32

int get_new_trip_id(int user_id, int device_id) {
	int trip_id = 1; /* starting index for new trip_id's */
	char query_str[512];
	sprintf(query_str, 
		"SELECT trip_id FROM amphor_keyspace.last_sensor_data WHERE user_id = %d AND device_id = %d;",
		user_id, device_id);

/* setup and connect to cluster */
	CassFuture* connect_future = NULL;
	CassCluster* cluster = cass_cluster_new();
	cass_cluster_set_contact_points(cluster, cassandra_cluster);

	connect_future = cass_cluster_connect(cluster);

	if(cass_future_error_code(connect_future) == CASS_OK) {
		CassFuture *close_future = NULL;
		CassSession *session = cass_future_get_session(connect_future);

	/* build statement and execute query */
		CassString query = cass_string_init(query_str);
		CassStatement *statement = cass_statement_new(query, 0);

		CassFuture *result_future = cass_session_execute(session, statement);

		if(cass_future_error_code(result_future) == CASS_OK) {
	/* retrieve result set and iterator over the rows */
			const CassResult *result = cass_future_get_result(result_future);
			CassIterator *rows = cass_iterator_from_result(result);

			int trip_id_max = -1;
			while(cass_iterator_next(rows)) {
				const CassRow *row = cass_iterator_get_row(rows);
				int temp_trip_id;
				cass_int64_t t64;
				cass_int32_t t32;
				const CassValue *value = cass_row_get_column_by_name(row, "trip_id");
				if(sizeof(temp_trip_id) == 4) {
					cass_value_get_int32(value, &t32);
					temp_trip_id = t32;
				} else if(sizeof(temp_trip_id) == 8) {
					cass_value_get_int64(value, &t64);
					temp_trip_id = t64;
				}
				else temp_trip_id = -1;
				printf("trip id = %d already used\n", trip_id);
				if(temp_trip_id > trip_id_max) trip_id_max = temp_trip_id;
			}

			if(trip_id_max > 0) trip_id = trip_id_max + 1;
			cass_result_free(result);
			cass_iterator_free(rows);
		} else {
		/* Handle error */
			CassString message = cass_future_error_message(result_future);
			fprintf(stderr, "Unable to run query: '%.*s'\n", (int)message.length, message.data);
		}

		cass_future_free(result_future);

	/* Close the session */
		close_future = cass_session_close(session);
		cass_future_wait(close_future);
		cass_future_free(close_future);
	} else {
	/* Handle error */
		CassString message = cass_future_error_message(connect_future);
		fprintf(stderr, "Unable to connect: '%.*s'\n", (int)message.length,
		message.data);
	}

	cass_future_free(connect_future);
	cass_cluster_free(cluster);

	return trip_id;
}

/* message delivery report callback, called for each message */
static void msg_delivered(rd_kafka_t *rk, void *payload, size_t len, int error_code, void *opaque, void *msg_opaque) {
	if(error_code) fprintf(stderr, "%% Message delivery failed: %s\n", rd_kafka_err2str(error_code));
	else if(debug) fprintf(stderr, "%% Message delivered (%zd bytes)\n", len);
}

/* kafka logger callback (optional) */
static void logger(const rd_kafka_t *rk, int level, const char *fac, const char *buf) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	fprintf(stderr, "%u.%03u RDKAFKA-%i-%s: %s: %s\n",
		(int)tv.tv_sec, (int)(tv.tv_usec / 1000),
		level, fac, rd_kafka_name(rk), buf);
}

int main(int argc, char **argv) {

	int random_seed = 1, sockfd, client_sockfd, port = 9000;
	socklen_t clilen;
	char buffer[BUFFDIM], errstr[ERRSTRDIM];
	struct sockaddr_in serv_addr, cli_addr;
	int i, n, partition = 1;
	rd_kafka_conf_t *conf;
	rd_kafka_topic_conf_t *topic_conf;

	for(i=0;i<argc;++i) {
		if(strcmp(argv[i], "-debug") == 0) { debug = 1;
		} else if(strcmp(argv[i], "-port") == 0) { port = atoi(argv[++i]);
		} else if(strcmp(argv[i], "-seed") == 0) { random_seed = atoi(argv[++i]);
		} else if(strcmp(argv[i], "-partition") == 0) { partition = atoi(argv[++i]);
		} else if(strcmp(argv[i], "-topic") == 0) { topic = argv[++i];
		} else if(strcmp(argv[i], "-cassandra") == 0) { cassandra_cluster = argv[++i];
		} else if(strcmp(argv[i], "-kafka") == 0) { kafka_cluster = argv[++i];
		}
	}

	srandom(random_seed); /* random number generator */

/* exception handling */
	signal(SIGINT, stop); /* ^C */
	signal(SIGUSR1, sig_usr1);

/* setup kafka */
	rd_kafka_topic_t *rkt;
	const char *brokers = kafka_cluster; 
	conf = rd_kafka_conf_new();
	topic_conf = rd_kafka_topic_conf_new();
	rd_kafka_conf_set_dr_cb(conf, msg_delivered);

	if (!(rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr)))) {
		fprintf(stderr, "%% Failed to create new producer: %s\n", errstr);
		exit(1);
	}

	/* set logger */
	rd_kafka_set_logger(rk, logger);
	rd_kafka_set_log_level(rk, LOG_DEBUG);

	/* add brokers */
	if(rd_kafka_brokers_add(rk, brokers) == 0) { fprintf(stderr, "%% No valid brokers specified\n"); exit(1); }

	rkt = rd_kafka_topic_new(rk, topic, topic_conf); /* create topic */

	if(port < 0) {
	    fprintf(stderr,"ERROR, no port provided\n");
	    exit(1);
	}

	struct pollfd poll_set[POLLSIZE];
	int num_fds = 0;
	memset(poll_set, 0, sizeof(poll_set));

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	listen(sockfd,5);

	poll_set[num_fds].fd = sockfd;
	poll_set[num_fds].events = POLLIN;
	++num_fds;

/* the following loop is complex for a good reason. 
	to avoid calls to the sockets, which are blocking, we implement a polling scheme.
	if data comes in, it's produced into Kafka.
	additionally, during the negotiation process for a new trip, a unique id must be established for the trip.
	we must query cassandra to brute force find a trip id which has not been taken */
	clilen = sizeof(cli_addr);
	while(run) {
		int n_read;
		sleep(1);
		printf("polling %d\n", num_fds);
		poll(poll_set, num_fds, -1);
		for(i=0;i<num_fds;++i) {
			printf("%d vs %d\n", poll_set[i].revents, POLLIN);
			if((poll_set[i].revents & POLLIN) == 0) continue;
			if(poll_set[i].fd == sockfd) {
				client_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
				if(client_sockfd < 0) error("ERROR on accept");
				poll_set[num_fds].fd = client_sockfd;
				poll_set[num_fds].events = POLLIN;
				++num_fds;
				printf("adding client on %d. N = %d\n", client_sockfd, num_fds);
			} else {
				ioctl(poll_set[i].fd, FIONREAD, &n_read);
				if(n_read == 0) {
					close(poll_set[i].fd);
					poll_set[i].events = 0;
					printf("Removing client on fd %d\n", poll_set[i].fd);
					poll_set[i] = poll_set[i + 1];
					num_fds--;
				} else {
					bzero(buffer,BUFFDIM);
					n = read(poll_set[i].fd, buffer, BUFFDIM-1);
					if(n < 0) error("ERROR reading from socket");
					printf("received message: [%s]\n", buffer);

					char *request_trip_id_hdr = "REQUEST TRIP ID";
					if(strncmp(buffer, request_trip_id_hdr, strlen(request_trip_id_hdr)) == 0) {
						int user_id = parse_integer(buffer, "user_id");
						int device_id = parse_integer(buffer, "device_id");
						int trip_id = 0;
						if(user_id < 0 || device_id < 0) {
							printf("unable to parse incoming trip_id request\n");
						} else {
							printf("processing trip_id request for user_id=%d device_id=%d\n",
								user_id, device_id);
							trip_id = get_new_trip_id(user_id, device_id);
						}

						char msg[256];
						sprintf(msg, "TRIP ID = %d", trip_id);
						write(poll_set[i].fd, msg, strlen(msg) + 1);
						printf("sent trip id = %d\n", trip_id);
					} else { /* produce message to kafka */
						if(rd_kafka_produce(rkt, partition, RD_KAFKA_MSG_F_COPY,
							buffer, n, /* payload and length */
							NULL, 0, /* optional key and its length */
							NULL) == -1) {
								fprintf(stderr, "Failed to produce to topic %s partition %i: %s\n",
									rd_kafka_topic_name(rkt), partition, 
									rd_kafka_err2str(rd_kafka_errno2err(errno)));
								rd_kafka_poll(rk, 0); /* poll to handle delivery reports */
						}
					}

				}
			}
		}
	}

	fprintf(stdout, "%s exiting loop for graceful shutdown\n", argv[0]);

	close(sockfd);

	rd_kafka_topic_destroy(rkt); /* destroy topic */

	rd_kafka_destroy(rk); /* destroy the handle */

	rd_kafka_wait_destroyed(2000); /* let background threads clean up and terminate cleanly. */

	return 0; 
}

