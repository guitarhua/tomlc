#ifndef TOML_H
#define TOML_H

struct toml_val {
	char *key; /* UTF-8 string, NULL if is an array element */

	enum TOML_TYPE { /* TOML_NULL used only during parsing */
			 TOML_NULL,
			 TOML_TABLE,
			 TOML_ARRAY,
			 TOML_INT,
			 TOML_FLOAT,
			 TOML_STRING,
			 TOML_BOOL,
			 TOML_DATETIME,
			 TOML_DATE,
			 TOML_TIME
	} type;

	union {
		long long int64;
		double float64;
		char *string; /* UTF-8 string */
		int boolean;
		struct {
			int year, month, day;
			int hour, min, sec, usec;
			char *z; /* z is NULL iff date-time is local */
		} datetime;
		struct {
			int year, month, day;
		} date;
		struct {
			int hour, min, sec, usec;
		} time;
		struct {
			struct char **
				keys; /* pointers to vals[i].key, NULL if empty table*/
			struct toml_val **
				vals; /* list of subelts for table, NULL if empty table */
			size_t len; /* length of keys and vals, 0 if empty table */
		} table;
		struct {
			struct toml_val **
				vals; /* list of subelts for array, NULL if empty array */
			size_t len; /* length of vals, 0 if empty table */
		} array;
	} u;
};

/*
 * parses and returns a TOML representation as a struct toml_val from a FILE*
 * returns NULL and writes to err[err_len] on failure
 * allocates memory using falloc and ffree if they are non-NULL
 * uses ffree to free any allocated memory if parsing fails
 * falloc and ffree must accept NULL input and do nothing (same as free(3))
 * defaults to malloc(3) and free(3) if they are NULL
 */
struct toml_val *toml_parse_file(FILE *fp, char *err, size_t err_len,
				 void *(*falloc)(size_t size),
				 void (*ffree)(void *ptr));

/*
 * parses and returns a TOML representation as a struct toml_val from a char[]
 * returns NULL and writes to err[err_len] on failure
 * allocates memory using falloc and ffree if they are non-NULL
 * uses ffree to free any allocated memory if parsing fails
 * falloc and ffree must accept NULL input and do nothing (same as free(3))
 * defaults to malloc(3) and free(3) if they are NULL
 */
struct toml_val *toml_parse_file(char *str, size_t len, char *err,
				 size_t err_len, void *(*falloc)(size_t size),
				 void (*ffree)(void *ptr));

/*
 * recursively free the toml_val if valid, do nothing if NULL
 * use ffree for freeing if not-NULL, otherwise default to free(3)
 */
void free_toml_val(struct toml_val *v, void (*ffree)(void *ptr));

#endif /* TOML_H */
