#include <stdio.h>
#include <sqlite3.h>

#define MATCH_QUERY "SELECT * FROM index WHERE index MATCH ?", 

static int
callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i = 0; i < argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	puts("");
	return 0;
}

int
main(int argc, char *argv[])
{
	int rc = -1;
	sqlite3 *db;
	char *err = 0;
	sqlite3_stmt *match_stmt;
	
	if(sqlite3_open(argv[1], &db)) {
		fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
		goto out_close_db;
	}
	
	if(sqlite3_prepare_v2(db, MATCH_QUERY, sizeof(MATCH_QUERY), &match_stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "sqlite3_prepare_v2 failed: %s\n", sqlite3_errmsg(db));
		goto out_close_db;
	}
	
	if(sqlite3_bind_text(match_stmt, 1, argv[2], strlen(argv[2]), SQLITE_STATIC) != SQLITE_OK) {
		fprintf(stderr, "sqlite3_bind_text failed: %s\n", sqlite3_errmsg(db));
		goto out_finalize_stmt;
	}

	while((rc = sqlite3_step(insert_stmt) == SQLITE_ROW)) {
		// show results
	}
	if(rc != 0) {
		fprintf(stderr, "sqlite3_step failed: %s\n", sqlite3_errmsg(db));
	}

out_finalize_stmt:
	sqlite3_finalize(match_stmt);
out_close_db:
	sqlite3_close(db);
	return rc;
}
