#include <stdio.h>
#include <stdlib.h>

/*
 * all elements of the TOML, including itself, are represented as a toml_elt
 * all sub-table and sub-array elts are owned by their parent and should not
 * be directly freed
 */
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

static char file_error_str[] = "error in reading from file stream";
static size_t f_err_str_len =
	sizeof(file_error_str) / sizeof(file_error_str[0]);
static char alloc_error_str[] = "error in memory allocation";
static size_t a_err_str_len =
	sizeof(alloc_error_str) / sizeof(alloc_error_str[0]);

struct toml_val *toml_parse_file(FILE *fp, char *err, size_t err_len,
				 void *(*falloc)(size_t size),
				 void (*ffree)(void *ptr))
{
	size_t flen;
	char *str;

	if ((fseek(fp, 0, SEEK_END) < 0) || ((flen = ftell(fp)) < 0) ||
	    (fseek(fp, 0, SEEK_SET) < 0)) {
		strncpy(err, file_error_str,
			err_len < f_err_str_len ? err_len : f_err_str_len);
		return NULL;
	}
	if (!falloc) {
		falloc = &malloc;
	}
	if (!ffree) {
		ffree = &free;
	}

	if ((str = falloc(flen + 1)) == NULL) {
		strncpy(err, alloc_error_str,
			err_len < a_err_str_len ? err_len : a_err_str_len);
		return NULL;
	}
	if (fread(str, 1, flen, fp) < flen) {
		ffree(str);
		strncpy(err, file_error_str,
			err_len < f_err_str_len ? err_len : f_err_str_len);
		return NULL;
	}
	str[flen] = '\0';

	return toml_parse_file(str, flen + 1, err, err_len, falloc, ffree);
}

struct toml_val *toml_parse_file(char *str, size_t len, char *err,
				 size_t err_len, void *(*falloc)(size_t size),
				 void (*ffree)(void *ptr))
{
	return NULL;
}

void free_toml_val(struct toml_val *v, void (*ffree)(void *ptr))
{
	size_t i;

	if (!v)
		return;
	if (!ffree)
		ffree = &free;
	switch (v->type) {
	case TOML_STRING:
		(*ffree)(v->u.string);
		break;
	case TOML_DATETIME:
		(*ffree)(v->u.datetime.z);
		break;
	case TOML_ARRAY:
		for (i = 0; i < v->u.array.len; i++)
			free_toml_val(v->u.array.vals[i], ffree);
		(*ffree)(v->u.array.vals);
		break;
	case TOML_TABLE:
		for (i = 0; i < v->u.table.len; i++)
			free_toml_val(v->u.table.vals[i], ffree);
		(*ffree)(v->u.table.vals);
		(*ffree)(
			v->u.table.keys); /* individual keys are owned by subvals and freed with them */
	}
	(*ffree)(v->key);
	(*ffree)(v);
}
