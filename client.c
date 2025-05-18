#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson/parson.h"

#define HOST "63.32.125.183"
#define PORT 8081
#define MAX_TOKENS 100

char **cookies;
int cookies_len = 0;
char **tokens;
int tokens_len = 0;

void free_structures() {
    cookies_len = 0;

        for (int i = 0; i < LINELEN; i++) {
            free(cookies[i]);
        }
        free(cookies);

        cookies = calloc(LINELEN, sizeof(char *));
        for (int i = 0; i < LINELEN; i++) {
            cookies[i] = calloc(1, LINELEN);
        }

        for (int i = 0; i < MAX_TOKENS; i++) {
            free(tokens[i]);
        }
        free(tokens);

        tokens = calloc(MAX_TOKENS, sizeof(char *));
        for (int i = 0; i < MAX_TOKENS; i++) {
            tokens[i] = calloc(1, LINELEN);
        }

        tokens_len = 0;
}

int connect_to_server() {
    // Obtinem un socket TCP pentru conectarea la server
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "socket");

    // Completăm in serv_addr adresa serverului, familia de adrese si portul
    // pentru conectare
    struct sockaddr_in serv_addr;
    socklen_t socket_len = sizeof(struct sockaddr_in);

    memset(&serv_addr, 0, socket_len);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    int rc = inet_pton(AF_INET, HOST, &serv_addr.sin_addr.s_addr);
    DIE(rc <= 0, "inet_pton");

    // Ne conectăm la server
    rc = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    DIE(rc < 0, "connect");

    printf("Connected to server\n");

    return sockfd;

}

void GET(char url[], int has_cookies, int has_token,  char **response) {

    int sockfd = connect_to_server();
    char *message = calloc(1, LINELEN);

    if (has_cookies == 1) {
        if (has_token == 1) {
            message = compute_get_request(HOST, url, NULL, cookies, cookies_len, tokens, tokens_len);
        } else {
            message = compute_get_request(HOST, url, NULL, cookies, cookies_len, NULL, 0);
        }
    } else {
        if (has_token == 1) {
            message = compute_get_request(HOST, url, NULL, NULL, 0, tokens, tokens_len);

        } else {
            message = compute_get_request(HOST, url, NULL, NULL, 0, NULL, 0);
        }
    }

    printf("Msg : \n%s\n", message);

    send_to_server(sockfd, message);
    *response = receive_from_server(sockfd);

    close_connection(sockfd);

}

void POST(char serialized_string[], int len, char url[], char content_type[], int has_cookies, int has_token ,char **response) {

    int sockfd = connect_to_server();
    char *message;

    char **body_data = calloc(len, LINELEN);
    for (int i = 0; i < len; i++) {
        body_data[i] = calloc(1, LINELEN);
        strcpy(body_data[i], serialized_string);
    }
    int body_data_fields_count = len;
    if (has_cookies == 0) {
        if (has_token == 0) {
            message = compute_post_request(HOST, url, content_type, body_data, body_data_fields_count, NULL, 0, NULL, 0);
        } else {
            message = compute_post_request(HOST, url, content_type, body_data, body_data_fields_count, NULL, 0, tokens, tokens_len);
        }
    } else {
        if (has_token == 0) {

            message = compute_post_request(HOST, url, content_type, body_data, body_data_fields_count, cookies, cookies_len, NULL, 0);
        } else {
            message = compute_post_request(HOST, url, content_type, body_data, body_data_fields_count, cookies, cookies_len, tokens, tokens_len);
        }
    }
    printf("Message sent: %s\n ", message);
    send_to_server(sockfd, message);
    *response = receive_from_server(sockfd);

    close_connection(sockfd);
}

void PUT(char serialized_string[], int len, char url[], char content_type[], int has_cookies, int has_token ,char **response) {

    int sockfd = connect_to_server();
    char *message;

    char **body_data = calloc(len, LINELEN);
    for (int i = 0; i < len; i++) {
        body_data[i] = calloc(1, LINELEN);
        strcpy(body_data[i], serialized_string);
    }
    int body_data_fields_count = len;
    if (has_cookies == 0) {
        if (has_token == 0) {
            message = compute_put_request(HOST, url, content_type, body_data, body_data_fields_count, NULL, 0, NULL, 0);
        } else {
            message = compute_put_request(HOST, url, content_type, body_data, body_data_fields_count, NULL, 0, tokens, tokens_len);
        }
    } else {
        if (has_token == 0) {

            message = compute_put_request(HOST, url, content_type, body_data, body_data_fields_count, cookies, cookies_len, NULL, 0);
        } else {
            message = compute_put_request(HOST, url, content_type, body_data, body_data_fields_count, cookies, cookies_len, tokens, tokens_len);
        }
    }
    printf("Message sent: %s\n ", message);
    send_to_server(sockfd, message);
    *response = receive_from_server(sockfd);

    close_connection(sockfd);
}

void DELETE(char url[], int has_cookies, int has_token ,char **response) {

    int sockfd = connect_to_server();
    char *message;

    if (has_cookies == 0) {
        if (has_token == 0) {
            message = compute_delete_request(HOST, url, NULL, 0, NULL, 0);
        } else {
            message = compute_delete_request(HOST, url, NULL, 0, tokens, tokens_len);
        }
    } else {
        if (has_token == 0) {

            message = compute_delete_request(HOST, url, cookies, cookies_len, NULL, 0);
        } else {
            message = compute_delete_request(HOST, url, cookies, cookies_len, tokens, tokens_len);
        }
    }
    printf("Message sent: %s\n ", message);
    send_to_server(sockfd, message);
    *response = receive_from_server(sockfd);

    close_connection(sockfd);
}

void login_request(char admin_username[], char username[], char password[]) {

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "admin_username", admin_username);
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(root_value);

    char *response = calloc(1, LINELEN);

    POST(serialized_string, 1,  "/api/v1/tema/user/login", "application/json", 0, 0, &response);

    char *p = strstr(response, "Set-Cookie: ");
    if (p) {
        p = p + strlen("Set-Cookie: ");
        int len = 0;
        while(*p != ';') {
            cookies[cookies_len][len] = *p;
            p++;
            len++;
        }
        cookies[cookies_len][len] = '\0';
        cookies_len++;
    }

    if (strstr(response, "HTTP/1.1 2")) {
        printf("SUCCESS: Autentificare reușită \n");
    } else {
        printf("ERROR: \n");
    }

    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

void login_admin(char username[], char password[]) {

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(root_value);

    char *response = calloc(1, LINELEN);

    POST(serialized_string, 1,  "/api/v1/tema/admin/login", "application/json", 0, 0, &response);

    char *p = strstr(response, "Set-Cookie: ");
    if (p) {
        p = p + strlen("Set-Cookie: ");
        int len = 0;
        while(*p != ';') {
            cookies[cookies_len][len] = *p;
            p++;
            len++;
        }
        cookies[cookies_len][len] = '\0';
        cookies_len++;
    }

    if (strstr(response, "HTTP/1.1 2")) {
        printf("SUCCESS: Admin autentificat cu succes \n");
    } else {
        printf("ERROR: ");
    }

    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

void add_user(char username[], char password[]) {

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(root_value);

    char *response = calloc(1, LINELEN);

    POST(serialized_string, 1,  "/api/v1/tema/admin/users", "application/json", 1, 0, &response);

    if (strstr(response, "HTTP/1.1 2")) {
        printf("SUCCESS: Utilizator adăugat cu succes \n");
    } else {
        printf("ERROR: ");
    }

    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

void parse_users(const char *json_string) {

    JSON_Value *root_value = json_parse_string(json_string);
    JSON_Object *root_object = json_value_get_object(root_value);

    JSON_Array *users = json_object_get_array(root_object, "users");
    size_t count = json_array_get_count(users);

    for (size_t i = 0; i < count; i++) {
        JSON_Object *user = json_array_get_object(users, i);

        int id = (int)json_object_get_number(user, "id");
        const char *username = json_object_get_string(user, "username");
        const char *password = json_object_get_string(user, "password");

        printf("#%d: %s:%s\n", id, username, password);
    }

    json_value_free(root_value);
}

void get_users() {

    char *response = calloc(1, LINELEN);
    GET("/api/v1/tema/admin/users", 1, 0, &response);

    if (strstr(response, "HTTP/1.1 2")) {
        parse_users(strstr(response, "{\"users\""));
        printf("SUCCES: \n");
    } else {
        printf("ERROR: ");
    }
}

void logout_admin() {

    char *response = calloc(1, LINELEN);
    GET("/api/v1/tema/admin/logout", 1, 0, &response);

    printf("%s\n", response);

    if (strstr(response, "HTTP/1.1 2")) {
        printf("SUCCES: Admin delogat\n");
        free_structures();

    } else {
        printf("ERROR: ");
    }
}

void get_acces() {

    char *response = calloc(1, LINELEN);
    GET("/api/v1/tema/library/access", 1, 0, &response);

    printf("%s\n", response);

    if (strstr(response, "HTTP/1.1 2")) {

        char *p = strstr(response, "{\"token\"");
        p = p + 10;
        int i = 0;
        while (*p != '"') {
            tokens[tokens_len][i] = *p;
            p++;
            i++;
        }
        tokens[tokens_len][i] = '\0';
        tokens_len++;

        printf("SUCCES: Token JWT primit\n");
    } else {
        printf("ERROR : Utilizator neautorizat \n");
    }

}

void parse_movies(const char *json_string) {

    JSON_Value *root_value = json_parse_string(json_string);
    JSON_Object *root_object = json_value_get_object(root_value);

    JSON_Array *movies = json_object_get_array(root_object, "movies");
    size_t count = json_array_get_count(movies);

    for (size_t i = 0; i < count; i++) {
        JSON_Object *movie = json_array_get_object(movies, i);

        int id = (int)json_object_get_number(movie, "id");
        const char *title = json_object_get_string(movie, "title");

        printf("#%d: %s\n", id, title);
    }

    json_value_free(root_value);
}


void get_movies() {

    char *response = calloc(1, LINELEN);
    GET("/api/v1/tema/library/movies", 0, 1, &response);

    if (strstr(response, "HTTP/1.1 2")) {
        parse_movies(strstr(response, "{\"movies\""));
        printf("SUCCES: \n");
    } else {
        printf("ERROR: ");
    }
}

void add_movie(char title[], double year, char description[], double rating) {

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "title", title);
    json_object_set_number(root_object, "year", year);
    json_object_set_string(root_object, "description", description);
    json_object_set_number(root_object, "rating", rating);

    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(root_value);

    char *response = calloc(1, LINELEN);

    POST(serialized_string, 1,  "/api/v1/tema/library/movies", "application/json", 0, 1, &response);

    if (strstr(response, "HTTP/1.1 2")) {
        printf("SUCCESS: Film adaugat\n");
    } else {
        printf("ERROR: ");
    }

    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

}

void get_movie(double id) {

    char url[LINELEN];
    sprintf(url, "/api/v1/tema/library/movies/%d", (int)id);

    char *response = calloc(1, LINELEN);
    GET(url, 0, 1, &response);

    if (strstr(response, "HTTP/1.1 2")) {
        printf("SUCCESS: Detalii film\n");
        printf("%s\n", response);
    } else {
        printf("ERROR: ");
    }

}

void update_movie(double id, char title[], double year, char description[], double rating) {

    char url[LINELEN];
    sprintf(url, "/api/v1/tema/library/movies/%d", (int)id);

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "title", title);
    json_object_set_number(root_object, "year", year);
    json_object_set_string(root_object, "description", description);
    json_object_set_number(root_object, "rating", rating);

    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(root_value);

    char *response = calloc(1, LINELEN);

    PUT(serialized_string, 1,  url, "application/json", 0, 1, &response);

    if (strstr(response, "HTTP/1.1 2")) {
        printf("SUCCESS: Film actualizat\n");
    } else {
        printf("ERROR: ");
    }

    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

void delete_movie(double id) {

    char url[LINELEN];
    sprintf(url, "/api/v1/tema/library/movies/%d", (int)id);

    char *response = calloc(1, LINELEN);

    DELETE(url, 0, 1, &response);

    if (strstr(response, "HTTP/1.1 2")) {
        printf("SUCCESS: Film șters cu succes\n");
    } else {
        printf("ERROR: ");
    }
}

void logout() {

    char *response = calloc(1, LINELEN);
    GET("/api/v1/tema/library/access", 1, 0, &response);

    printf("%s\n", response);

    if (strstr(response, "HTTP/1.1 2")) {
        printf("SUCCES: Utilizator delogat\n");
        free_structures();

    } else {
        printf("ERROR: ");
    }

}

int main(int argc, char *argv[])
{

    char *message;
    char *response;
    int sockfd = connect_to_server();

    cookies = calloc(LINELEN, sizeof(char *));
    for (int i = 0; i < LINELEN; i++) {
        cookies[i] = calloc(1, LINELEN);
    }

    tokens = calloc(MAX_TOKENS, sizeof(char *));
    for (int i = 0; i < MAX_TOKENS; i++) {
        tokens[i] = calloc(1, LINELEN);
    }

    while(1) {
        char line[LINELEN];
        fgets(line, LINELEN, stdin);
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        printf("Linie citita: %s\n", line);
        if (strncmp(line, "login_admin", 11) == 0) {

            char username[LINELEN], password[LINELEN];
            printf("username=");
            fgets(username, LINELEN, stdin);
            size_t len = strlen(username);
            if (len > 0 && username[len-1] == '\n') {
                username[len-1] = '\0';
            }
            printf("password=");
            fgets(password, LINELEN, stdin);
            len = strlen(password);
            if (len > 0 && password[len-1] == '\n') {
                password[len-1] = '\0';
            }
            printf("Username citit: %s\n", username);
            printf("Parola citita: %s\n", password);
            login_admin(username, password);

        } else if (strncmp(line, "login", 5) == 0) {

            char admin_username[LINELEN], username[LINELEN], password[LINELEN];
            printf("admin_username=");
            fgets(admin_username, LINELEN, stdin);
            admin_username[strlen(admin_username) - 1] = '\0';
            printf("username=");
            fgets(username, LINELEN, stdin);
            username[strlen(username) - 1] = '\0';
            printf("password=");
            fgets(password, LINELEN, stdin);
            password[strlen(password) - 1] = '\0';
            login_request(admin_username, username, password);

        } else if (strncmp(line, "add_user", 8) == 0) {

            char username[LINELEN], password[LINELEN];
            printf("username=");
            fgets(username, LINELEN, stdin);
            size_t len = strlen(username);
            if (len > 0 && username[len-1] == '\n') {
                username[len-1] = '\0';
            }
            printf("password=");
            fgets(password, LINELEN, stdin);
            len = strlen(password);
            if (len > 0 && password[len-1] == '\n') {
                password[len-1] = '\0';
            }
            printf("Username citit: %s\n", username);
            printf("Parola citita: %s\n", password);
            add_user(username, password);

        } else if (strncmp( line, "get_users", 9) == 0){
            get_users();

        } else if (strncmp(line, "logout_admin", 12) == 0) {
            logout_admin();

        } else if (strncmp(line, "get_acces", 9) == 0) {
            get_acces();

        } else if (strncmp(line, "logout", 7) == 0) {
            logout();

        } else if (strncmp(line, "get_movies", 10) == 0) {
            get_movies();

        } else if (strncmp(line, "add_movie", 9) == 0) {

            char title[LINELEN], year[LINELEN], description[LINELEN], rating[LINELEN];
            double year_double, rating_double;
            printf("title=");
            fgets(title, LINELEN, stdin);
            title[strlen(title) - 1] = '\0';
            printf("year=");
            fgets(year, LINELEN, stdin);
            year[strlen(year) - 1] = '\0';
            printf("description=");
            fgets(description, LINELEN, stdin);
            description[strlen(description) - 1] = '\0';
            printf("rating=");
            fgets(rating, LINELEN, stdin);
            rating[strlen(rating) - 1] = '\0';
            char *p, *p1;
            year_double = strtod(year, &p);
            rating_double = strtod(rating, &p1);
            add_movie(title, year_double, description, rating_double);

        } else if (strncmp(line, "get_movie", 9) == 0) {

            char id[LINELEN];
            double id_movie;
            printf("id=");
            fgets(id, LINELEN, stdin);
            id[strlen(id) - 1] = '\0';
            char *p;
            id_movie = strtod(id, &p);
            get_movie(id_movie);

        } else if (strncmp(line, "update_movie", 12) == 0) {

            char id[LINELEN], title[LINELEN], year[LINELEN], description[LINELEN], rating[LINELEN];
            double year_double, rating_double, id_movie;
            printf("id=");
            fgets(id, LINELEN, stdin);
            id[strlen(id) - 1] = '\0';
            char *p2;
            id_movie = strtod(id, &p2);
            printf("title=");
            fgets(title, LINELEN, stdin);
            title[strlen(title) - 1] = '\0';
            printf("year=");
            fgets(year, LINELEN, stdin);
            year[strlen(year) - 1] = '\0';
            printf("description=");
            fgets(description, LINELEN, stdin);
            description[strlen(description) - 1] = '\0';
            printf("rating=");
            fgets(rating, LINELEN, stdin);
            rating[strlen(rating) - 1] = '\0';
            char *p, *p1;
            year_double = strtod(year, &p);
            rating_double = strtod(rating, &p1);
            update_movie(id_movie, title, year_double, description, rating_double);

        } else if (strncmp(line, "delete_movie", 12) == 0 ) {

            char id[LINELEN];
            double id_movie;
            printf("id=");
            fgets(id, LINELEN, stdin);
            id[strlen(id) - 1] = '\0';
            char *p;
            id_movie = strtod(id, &p);
            delete_movie(id_movie);

        } else if (strncmp(line, "exit", 4) == 0) {
            break;
        }
    }

    return 0;
}
