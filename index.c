#include <stdio.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i = 0; i < argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	puts("");
	return 0;
}

int main(int argc, char *argv[])
{
	sqlite3 *db;
	char *err = 0;
	int rc = -1;
	
	if(sqlite3_open(argv[1], &db)) {
		fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
		goto out_close_db;
	}
	
	if(sqlite3_exec(db, argv[2], callback, 0, &err) != SQLITE_OK) {
		fprintf(stderr, "sqlite3_exec failed: %s\n", err);
		sqlite3_free(zErrMsg);
		goto out_close_db;
	}
	
	rc = 0;
	
out_close_db:
	sqlite3_close(db);
	return rc;
}