#include <stdlib.h>


/*
 * all elements of the TOML, including itself, are represented as a toml_elt
 * all sub-table and sub-array elts are owned by their parent and should not
 * be directly freed
 */
struct toml_val {
	char *key; /* UTF-8 string */

	enum TOML_TYPE { /* TOML_NULL used only during parsing */
		TOML_NULL, TOML_TABLE, TOML_ARRAY, TOML_INT, TOML_FLOAT,
		TOML_STRING, TOML_BOOL, TOML_DATETIME, TOML_DATE, TOML_TIME
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
			struct char **keys; /* pointers to vals[i].key */
			struct toml_val *vals; /* list of subelts for a table */
			size_t len;
		} table;
		struct {
			struct toml_val *vals; /* list of subelts for an array */
			size_t len;
		} array;
	} u;
};

/*
 * parses and returns a TOML representation as a struct toml_val from a FILE*
 * returns NULL and writes to err[err_len] on failure
 * allocates memory using falloc and ffree if they are non-NULL
 * defaults to malloc(3) and free(3) otherwise
 */
struct toml_val *toml_parse_file(FILE *fp, char *err, size_t err_len,
		void *(*falloc)(size_t size), void (*free)(void *ptr));

/*
 * parses and returns a TOML representation as a struct toml_val from a char[]
 * returns NULL and writes to err[err_len] on failure
 * allocates memory using falloc and ffree if they are non-NULL
 * defaults to malloc(3) and free(3) otherwise
 */
struct toml_val *toml_parse_file(char *str, size_t len, char *err,
		size_t err_len, void *(*falloc)(size_t size),
		void (*free)(void *ptr));


void free_toml_val(struct toml_val *v, void (*ffree)(void *ptr))
{
	if (!v)
		return;
	if (!ffree)
		ffree = &free;
	switch (v->type) {
	case TOML_NULL:
	case TOML_INT:
	case TOML_FLOAT:
	case TOML_BOOL:
	case TOML_DATE:
	case TOML_TIME:
		break;
	case TOML_STRING:


	}








	free(v->key);
	free(v);
}


